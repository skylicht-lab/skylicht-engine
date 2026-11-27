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

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <cerrno>
#include <sys/socket.h>
#endif

namespace Skylicht
{
	namespace Network
	{
		void CUDPSocket::release()
		{
			m_remoteEndpoint = {};
			m_remotePort = 0;
			CBaseSocket::release();
		}

		CUDPSocket::CUDPSocket() : CBaseSocket(65507, true)
		{
		}

		CUDPSocket::~CUDPSocket()
		{
			release();
		}

		void CUDPSocket::setRemote(const sockaddr_in& endpoint)
		{
			m_remoteEndpoint = endpoint;
			m_remotePort = ntohs(endpoint.sin_port);
		}

		bool CUDPSocket::openSocket(const char* bindAddress, irr::u16 port, std::size_t maxConnections)
		{
			return CBaseSocket::openSocket(bindAddress, port, maxConnections, SOCK_DGRAM, IPPROTO_UDP);
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
			for (std::size_t i = 0; i < maxReceivePackets; ++i)
			{
				sockaddr_in source = {};
#if defined(_WIN32)
				int length = sizeof(source);
#else
				socklen_t length = sizeof(source);
#endif
				int received = static_cast<int>(recvfrom(m_socket,
					reinterpret_cast<char*>(m_buffer.data()), static_cast<int>(m_buffer.size()), 0,
					reinterpret_cast<sockaddr*>(&source), &length));
				if (received < 0)
				{
					int error = CSocketUtil::getSocketError();
					// No datagram is available on the non-blocking socket; stop receiving this update.
					if (CSocketUtil::wouldBlock(error))
						break;
					// The system call was interrupted (e.g. by a POSIX signal); retry within the receive budget.
					if (CSocketUtil::interrupted(error))
						continue;
					m_lastError = error;
					success = false;
					break;
				}
				if (source.sin_family != AF_INET)
					continue;
				// Client only: accept datagrams from the configured server endpoint.
				// A server has m_remotePort == 0 and accepts datagrams from multiple endpoints.
				if (m_remotePort)
				{
					if (m_remoteEndpoint.sin_addr.s_addr != source.sin_addr.s_addr ||
						m_remoteEndpoint.sin_port != source.sin_port)
						continue;
					// A removed/timed-out client session requires an explicit open().
					if (m_connections.empty())
						continue;
				}
				CConnection* raw = getConnectionByEndpoint(source);
				bool isNewConnection = raw == nullptr;
				if (isNewConnection)
					raw = addConnection(source);
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
				CMemoryStream packet(m_buffer.data(), static_cast<unsigned int>(received));
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
					const sockaddr_in& target = connection->m_socketAddress;
					const char* bytes = packet.Data.empty() ? "" : reinterpret_cast<const char*>(packet.Data.data());
					int sent = static_cast<int>(sendto(m_socket, bytes, static_cast<int>(packet.Data.size()), 0,
						reinterpret_cast<const sockaddr*>(&target), sizeof(target)));
					if (sent < 0)
					{
						int error = CSocketUtil::getSocketError();
						if (CSocketUtil::wouldBlock(error) || CSocketUtil::interrupted(error))
							break;
						m_lastError = error;
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
