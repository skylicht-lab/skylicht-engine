/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CBaseSocket.h"
#include "CRawSocketTransport.h"
#include "Utils/CMemoryStream.h"
#include "Utils/CDateTimeUtils.h"
#include <cstring>

namespace Skylicht
{
	namespace Network
	{
		void CBaseSocket::release()
		{
			delete m_transport;
			m_transport = nullptr;
			for (auto& item : m_connections)
				CBaseSocket::releaseConnection(item.second);
			m_localPort = 0;
			m_queue.clear();
			m_queueBytes = 0;
			++m_revision;
		}

		bool CBaseSocket::canQueue(const void* data, std::size_t size, std::size_t count) const
		{
			return isOpen() && (!size || data) && size <= m_maxPayloadSize &&
				count <= MaxQueuePackets - m_queue.size() &&
				(!size || count <= (MaxQueueBytes - m_queueBytes) / size);
		}

		void CBaseSocket::enqueue(irr::u32 id, const void* data, std::size_t size)
		{
			SPacket packet;
			packet.ConnectionID = id;
			packet.Data.resize(size);
			if (size)
				std::memcpy(packet.Data.data(), data, size);
			m_queue.push_back(std::move(packet));
			m_queueBytes += size;
		}

		bool CBaseSocket::setTransportFactory(const std::function<ITransport*(ETransportMode)>& factory)
		{
			if (m_closing || isOpen())
				return false;
			m_transportFactory = factory;
			return true;
		}

		bool CBaseSocket::openSocket(const char* bindAddress, irr::u16 port, std::size_t maxConnections,
			ETransportMode mode)
		{
			if (m_closing)
				return false;
			close();
			m_lastError = 0;
			STransportEndpoint local;
			if (!maxConnections || !CSocketUtil::parseEndpoint(bindAddress, port, local))
			{
				m_lastError = CSocketUtil::invalidArgument();
				return false;
			}
			m_transport = m_transportFactory ? m_transportFactory(mode) : new CRawSocketTransport(mode);
			if (!m_transport || m_transport->getMode() != mode)
			{
				m_lastError = CSocketUtil::invalidArgument();
				release();
				return false;
			}
			STransportEndpoint endpoint;
			if (m_transport->open(local) != ETransportStatus::Done)
			{
				m_lastError = m_transport->getLastError().Code;
				release();
				return false;
			}
			if (!m_transport->getLocalEndpoint(endpoint))
			{
				m_lastError = CSocketUtil::invalidArgument();
				release();
				return false;
			}
			m_localPort = endpoint.Port;
			m_maxConnections = maxConnections;
			return true;
		}

		void CBaseSocket::close()
		{
			if (m_closing)
				return;
			m_closing = true;
			release();
			// Detach first so callbacks may safely inspect/remove other peers.
			auto connections = std::move(m_connections);
			m_connections.clear();
			m_endpointConnections.clear();
			for (auto& item : connections)
			{
				auto callback = OnDisconnected;
				if (callback)
					callback(item.second, EDisconnectReason::Closed);
				delete item.second;
			}
			m_closing = false;
		}

		CConnection* CBaseSocket::getConnection(irr::u32 id) const
		{
			auto it = m_connections.find(id);
			return it == m_connections.end() ? nullptr : it->second;
		}

		CConnection* CBaseSocket::getConnectionByEndpoint(const STransportEndpoint& endpoint) const
		{
			STransportEndpoint key;
			if (!CSocketUtil::parseEndpoint(endpoint.Address.c_str(), endpoint.Port, key))
				return nullptr;
			auto it = m_endpointConnections.find(key);
			return it == m_endpointConnections.end() ? nullptr : getConnection(it->second);
		}

		CConnection* CBaseSocket::getConnectionByEndpoint(const sockaddr_in& endpoint) const
		{
			STransportEndpoint key;
			return CSocketUtil::copyEndpoint(endpoint, key) ? getConnectionByEndpoint(key) : nullptr;
		}

		void CBaseSocket::removeConnection(irr::u32 id)
		{
			disconnect(id, EDisconnectReason::Removed);
		}

		void CBaseSocket::disconnect(irr::u32 id, EDisconnectReason reason)
		{
			auto it = m_connections.find(id);
			if (it == m_connections.end())
				return;
			CConnection* connection = it->second;
			m_endpointConnections.erase(connection->m_endpoint);
			m_connections.erase(it);
			releaseConnection(connection);
			for (auto packet = m_queue.begin(); packet != m_queue.end();)
			{
				if (packet->ConnectionID == id)
				{
					m_queueBytes -= packet->Data.size();
					packet = m_queue.erase(packet);
				}
				else
					++packet;
			}
			auto callback = OnDisconnected;
			if (callback)
				callback(connection, reason);
			delete connection;
		}

		CConnection* CBaseSocket::addConnection(const STransportEndpoint& endpoint)
		{
			STransportEndpoint key;
			if (!endpoint.Port || !CSocketUtil::parseEndpoint(endpoint.Address.c_str(), endpoint.Port, key))
			{
				m_lastError = CSocketUtil::invalidArgument();
				return nullptr;
			}
			auto existing = m_endpointConnections.find(key);
			if (existing != m_endpointConnections.end())
				return getConnection(existing->second);
			constexpr irr::u32 connectionIDLimit = 4000000000u;
			if (!isOpen() || m_connections.size() >= m_maxConnections ||
				m_connections.size() >= connectionIDLimit)
				return nullptr;
			irr::u32 id;
			do
			{
				// Reuse IDs after the limit, skipping IDs still assigned to peers.
				if (m_nextID > connectionIDLimit)
					m_nextID = 1;
				id = m_nextID++;
			} while (m_connections.find(id) != m_connections.end());
			CConnection* connection = new CConnection(id, key,
				static_cast<irr::u64>(CDateTimeUtils::getTimeBySecond()) * 1000, m_defaultTimeout);
			m_endpointConnections.emplace(key, id);
			m_connections.emplace(id, connection);
			return connection;
		}

		bool CBaseSocket::notifyConnected(irr::u32 id)
		{
			auto it = m_connections.find(id);
			if (it == m_connections.end())
				return false;
			CConnection* connection = it->second;
			const auto revision = m_revision;
			auto callback = OnConnected;
			if (callback)
				callback(connection);
			// The callback may remove the peer or close/reopen the socket.
			return m_revision == revision && getConnection(id) != nullptr;
		}

		bool CBaseSocket::send(irr::u32 connectionID, const void* data, std::size_t size)
		{
			if (!getConnection(connectionID) || !canQueue(data, size, 1))
				return false;
			if (size || m_queueEmptyPackets)
				enqueue(connectionID, data, size);
			return true;
		}

		bool CBaseSocket::sendToAll(const void* data, std::size_t size, irr::u32 ignoreID)
		{
			std::size_t count = m_connections.size() - (getConnection(ignoreID) ? 1 : 0);
			if (!canQueue(data, size, count))
				return false;
			for (auto& item : m_connections)
				if ((size || m_queueEmptyPackets) && item.first != ignoreID)
					enqueue(item.first, data, size);
			return true;
		}

		bool CBaseSocket::send(irr::u32 connectionID, CMemoryStream& packet)
		{
			return send(connectionID, packet.getData(), packet.getSize());
		}

		CBaseSocket::CBaseSocket(std::size_t maxPayloadSize, bool queueEmptyPackets) :
			m_buffer(65536), m_maxPayloadSize(maxPayloadSize), m_queueEmptyPackets(queueEmptyPackets)
		{
		}

		CBaseSocket::~CBaseSocket()
		{
			// Derived destructors release their own native handles first. No callbacks.
			CBaseSocket::release();
			for (auto& item : m_connections)
				delete item.second;
		}

		void CBaseSocket::releaseConnection(CConnection* connection)
		{
			delete connection->m_transport;
			connection->m_transport = nullptr;
		}

		bool CBaseSocket::expireConnections()
		{
			const auto revision = m_revision;
			std::vector<irr::u32> expired;
			const irr::u64 now = static_cast<irr::u64>(CDateTimeUtils::getTimeBySecond()) * 1000;
			for (auto& item : m_connections)
				if (item.second->m_timeout && now >= item.second->m_lastReceiveTime &&
					now - item.second->m_lastReceiveTime >= item.second->m_timeout)
					expired.push_back(item.first);
			for (auto id : expired)
			{
				disconnect(id, EDisconnectReason::Timeout);
				if (m_revision != revision)
					return false;
			}
			return true;
		}
	}
}
