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
#include "Utils/CMemoryStream.h"
#include "Utils/CDateTimeUtils.h"
#if defined(_WIN32)
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#endif
#include <cstring>
#include <limits>

namespace Skylicht
{
	namespace Network
	{
		void CBaseSocket::release()
		{
			if (m_socket != InvalidSocket)
			{
				CSocketUtil::closeHandle(m_socket);
				m_socket = InvalidSocket;
			}
			for (auto& item : m_connections)
				CBaseSocket::releaseConnection(item.second);
#if defined(_WIN32)
			if (m_wsaReady)
			{
				WSACleanup();
				m_wsaReady = false;
			}
#endif
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

		bool CBaseSocket::openSocket(const char* bindAddress, irr::u16 port, std::size_t maxConnections,
			int socketType, int protocol)
		{
			if (m_closing)
				return false;
			close();
			m_lastError = 0;
			sockaddr_in local;
			if (!maxConnections || !CSocketUtil::parseAddress(bindAddress, port, local))
			{
				m_lastError = CSocketUtil::invalidArgument();
				return false;
			}
#if defined(_WIN32)
			WSADATA wsa;
			m_lastError = WSAStartup(MAKEWORD(2, 2), &wsa);
			if (m_lastError)
				return false;
			m_wsaReady = true;
#endif
			m_socket = socket(AF_INET, socketType, protocol);
			bool ready = m_socket != InvalidSocket;
			if (ready)
				ready = CSocketUtil::setNonBlocking(m_socket);
			if (ready)
				ready = bind(m_socket, reinterpret_cast<sockaddr*>(&local), sizeof(local)) == 0;
			if (ready)
			{
#if defined(_WIN32)
				int length = sizeof(local);
#else
				socklen_t length = sizeof(local);
#endif
				ready = getsockname(m_socket, reinterpret_cast<sockaddr*>(&local), &length) == 0;
			}
			if (!ready)
			{
				m_lastError = CSocketUtil::getSocketError();
				release();
				return false;
			}
			m_localPort = ntohs(local.sin_port);
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

		CConnection* CBaseSocket::getConnectionByEndpoint(const sockaddr_in& endpoint) const
		{
			if (endpoint.sin_family != AF_INET)
				return nullptr;
			auto it = m_endpointConnections.find(std::make_pair(endpoint.sin_addr.s_addr, endpoint.sin_port));
			return it == m_endpointConnections.end() ? nullptr : getConnection(it->second);
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
			const sockaddr_in& endpoint = connection->m_socketAddress;
			m_endpointConnections.erase(std::make_pair(endpoint.sin_addr.s_addr, endpoint.sin_port));
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

		CConnection* CBaseSocket::addConnection(const sockaddr_in& endpoint)
		{
			if (endpoint.sin_family != AF_INET || !endpoint.sin_port)
			{
				m_lastError = CSocketUtil::invalidArgument();
				return nullptr;
			}
			const auto key = std::make_pair(endpoint.sin_addr.s_addr, endpoint.sin_port);
			auto existing = m_endpointConnections.find(key);
			if (existing != m_endpointConnections.end())
				return getConnection(existing->second);
			if (!isOpen() || m_connections.size() >= m_maxConnections ||
				m_connections.size() >= std::numeric_limits<irr::u32>::max())
				return nullptr;
			char normalized[INET_ADDRSTRLEN];
			if (!inet_ntop(AF_INET, &endpoint.sin_addr, normalized, sizeof(normalized)))
			{
				m_lastError = CSocketUtil::getSocketError();
				return nullptr;
			}
			irr::u32 id;
			do
			{
				// Unsigned increment wraps to zero; zero is reserved for an invalid ID.
				id = m_nextID++;
				if (id == 0)
					id = m_nextID++;
			} while (m_connections.find(id) != m_connections.end());
			CConnection* connection = new CConnection(id, normalized, endpoint,
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
			if (connection->m_socket != InvalidSocket)
			{
				CSocketUtil::closeHandle(connection->m_socket);
				connection->m_socket = InvalidSocket;
			}
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
