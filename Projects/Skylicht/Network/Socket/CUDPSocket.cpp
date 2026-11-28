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
#include "CUDPSocket.h"
#include "Utils/CMemoryStream.h"
#include "Utils/CDateTimeUtils.h"

namespace Skylicht
{
	namespace Network
	{
		void CUDPSocket::release()
		{
			m_remoteEndpoint = {};
			CBaseSocket::release();
		}

		CUDPSocket::CUDPSocket() : CBaseSocket(65507, true)
		{
		}

		CUDPSocket::~CUDPSocket()
		{
			release();
		}

		void CUDPSocket::setRemote(const STransportEndpoint& endpoint)
		{
			m_remoteEndpoint = endpoint;
		}

		bool CUDPSocket::openSocket(const char* bindAddress, irr::u16 port, std::size_t maxConnections)
		{
			return CBaseSocket::openSocket(bindAddress, port, maxConnections, ETransportMode::Datagram);
		}

		bool CUDPSocket::update(std::size_t maxReceivePackets, std::size_t maxSendPackets)
		{
			if (!isOpen())
				return false;
			m_lastError = 0;
			// Keep a value snapshot: callbacks such as OnReceived can close or reopen
			// this socket on the same thread, changing m_revision before they return.
			const auto revision = m_revision;
			bool success = true;
			ETransportStatus progress = m_transport->update(1);
			if (progress == ETransportStatus::Error || progress == ETransportStatus::Closed)
			{
				m_lastError = m_transport->getLastError().Code;
				return false;
			}
			for (std::size_t i = 0; i < maxReceivePackets; ++i)
			{
				STransportEndpoint remote;
				STransportResult received = m_transport->receiveFrom(remote, m_buffer.data(), m_buffer.size());
				if (received.Status == ETransportStatus::WouldBlock || received.Status == ETransportStatus::Pending)
					break;
				if (received.Status == ETransportStatus::Interrupted || received.Status == ETransportStatus::Truncated)
					continue;
				if (received.Status != ETransportStatus::Done)
				{
					m_lastError = m_transport->getLastError().Code;
					success = false;
					break;
				}
				// Client only: accept datagrams from the configured server endpoint.
				// A server has m_remoteEndpoint.Port == 0 and accepts datagrams from multiple endpoints.
				if (m_remoteEndpoint.Port)
				{
					if (m_remoteEndpoint.Address != remote.Address ||
						m_remoteEndpoint.Port != remote.Port)
						continue;
					// A removed/timed-out client session requires an explicit open().
					if (m_connections.empty())
						continue;
				}
				CConnection* raw = getConnectionByEndpoint(remote);
				bool isNewConnection = raw == nullptr;
				if (isNewConnection)
					raw = addConnection(remote);
				if (!raw)
					continue;
				CConnection* connection = raw;
				const irr::u32 connectionID = connection->getID();
				connection->m_lastReceiveTime = static_cast<irr::u64>(CDateTimeUtils::getTimeBySecond()) * 1000;
				if (isNewConnection)
				{
					auto callback = OnConnected;
					if (callback)
						callback(connection);
				}
				// Stop this update if the callback closed or reopened the socket.
				if (m_revision != revision)
					return false;
				// Resolve the ID again: OnConnected may have deleted the original peer.
				connection = getConnection(connectionID);
				if (!connection)
					continue;
				CMemoryStream packet(m_buffer.data(), static_cast<unsigned int>(received.Bytes));
				auto callback = OnReceived;
				if (callback)
					callback(connection, &packet);
				// Stop this update if the callback closed or reopened the socket.
				if (m_revision != revision)
					return false;
			}

			if (!expireConnections())
				return false;

			for (std::size_t i = 0; i < maxSendPackets && !m_queue.empty(); ++i)
			{
				const auto& packet = m_queue.front();
				CConnection* connection = getConnection(packet.ConnectionID);
				if (connection)
				{
					STransportResult sent = m_transport->sendTo(connection->m_endpoint, packet.Data.data(), packet.Data.size());
					if (sent.Status == ETransportStatus::WouldBlock || sent.Status == ETransportStatus::Pending ||
						sent.Status == ETransportStatus::Interrupted)
						break;
					if (sent.Status != ETransportStatus::Done || sent.Bytes != packet.Data.size())
					{
						m_lastError = m_transport->getLastError().Code;
						success = false;
					}
				}
				m_queueBytes -= packet.Data.size();
				m_queue.pop_front();
			}
			return success;
		}
	}
}
