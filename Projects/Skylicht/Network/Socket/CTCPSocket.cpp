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
#include "CTCPSocket.h"
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

#include <set>
#if !defined(_WIN32)
#include <poll.h>
#endif

namespace Skylicht
{
	namespace Network
	{
		void CTCPSocket::release()
		{
			m_listening = m_connecting = false;
			CBaseSocket::release();
		}

		CTCPSocket::CTCPSocket() : CBaseSocket(MaxQueueBytes, false)
		{
		}

		CTCPSocket::~CTCPSocket()
		{
			release();
		}

		bool CTCPSocket::openSocket(const char* bindAddress, irr::u16 port, std::size_t maxConnections)
		{
			return CBaseSocket::openSocket(bindAddress, port, maxConnections, SOCK_STREAM, IPPROTO_TCP);
		}

		void CTCPSocket::releaseConnection(CConnection* connection)
		{
			if (m_connecting)
			{
				CSocketUtil::closeHandle(m_socket);
				m_socket = InvalidSocket;
				m_connecting = false;
			}
			CBaseSocket::releaseConnection(connection);
		}

		bool CTCPSocket::update(std::size_t maxReceivePackets, std::size_t maxSendPackets)
		{
			if (!isOpen())
				return false;
			m_lastError = 0;
			const auto revision = m_revision;
			bool success = true;
			if (m_connecting)
			{
#if defined(_WIN32)
				fd_set writable, errors;
				FD_ZERO(&writable);
				FD_ZERO(&errors);
				FD_SET(m_socket, &writable);
				FD_SET(m_socket, &errors);
				timeval wait = {};
				int ready = select(0, nullptr, &writable, &errors, &wait);
#else
				pollfd descriptor = {};
				descriptor.fd = m_socket;
				descriptor.events = POLLOUT;
				int ready = poll(&descriptor, 1, 0);
#endif
				int error = 0;
				if (ready < 0)
				{
					error = CSocketUtil::getSocketError();
					if (CSocketUtil::interrupted(error))
						error = 0;
				}
				else if (ready > 0)
				{
#if defined(_WIN32)
					int length = sizeof(error);
#else
					socklen_t length = sizeof(error);
#endif
					if (getsockopt(m_socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &length) != 0)
						error = CSocketUtil::getSocketError();
				}
				irr::u32 id = m_connections.begin()->first;
				if (error)
				{
					m_lastError = error;
					disconnect(id, EDisconnectReason::Error);
					success = false;
				}
				else if (ready > 0)
				{
					getConnection(id)->m_socket = m_socket;
					m_socket = InvalidSocket;
					m_connecting = false;
					notifyConnected(id);
				}
				if (m_revision != revision)
					return false;
			}

			if (m_listening)
			{
				for (std::size_t i = 0; i < maxReceivePackets; ++i)
				{
					sockaddr_in endpoint = {};
#if defined(_WIN32)
					int length = sizeof(endpoint);
#else
					socklen_t length = sizeof(endpoint);
#endif
					Socket peer = accept(m_socket, reinterpret_cast<sockaddr*>(&endpoint), &length);
					if (peer == InvalidSocket)
					{
						int error = CSocketUtil::getSocketError();
						if (CSocketUtil::wouldBlock(error))
							break;
						if (CSocketUtil::interrupted(error))
							continue;
						m_lastError = error;
						success = false;
						break;
					}
					if (!CSocketUtil::setNonBlocking(peer))
					{
						m_lastError = CSocketUtil::getSocketError();
						CSocketUtil::closeHandle(peer);
						success = false;
						continue;
					}
					CConnection* connection = addConnection(endpoint);
					if (!connection)
					{
						CSocketUtil::closeHandle(peer);
						continue;
					}
					irr::u32 id = connection->getID();
					connection->m_socket = peer;
					notifyConnected(id);
					if (m_revision != revision)
						return false;
				}
			}

			// Snapshot IDs because callbacks may remove any peer.
			std::vector<irr::u32> ids;
			for (auto& item : m_connections)
				ids.push_back(item.first);
			for (auto id : ids)
			{
				for (std::size_t i = 0; i < maxReceivePackets; ++i)
				{
					CConnection* peer = getConnection(id);
					if (!peer || peer->m_socket == InvalidSocket)
						break;
					int received = static_cast<int>(recv(peer->m_socket,
						reinterpret_cast<char*>(m_buffer.data()), static_cast<int>(m_buffer.size()), 0));
					if (received <= 0)
					{
						int error = received < 0 ? CSocketUtil::getSocketError() : 0;
						if (received < 0 && CSocketUtil::wouldBlock(error))
							break;
						if (received < 0 && CSocketUtil::interrupted(error))
							continue;
						if (error)
						{
							m_lastError = error;
							success = false;
						}
						disconnect(id, error ? EDisconnectReason::Error : EDisconnectReason::RemoteClosed);
						if (m_revision != revision)
							return false;
						break;
					}
					CConnection* connection = getConnection(id);
					connection->m_lastReceiveTime = static_cast<irr::u64>(CDateTimeUtils::getTimeBySecond()) * 1000;
					CMemoryStream packet(m_buffer.data(), static_cast<unsigned int>(received));
					auto callback = OnReceived;
					if (callback)
						callback(connection, packet);
					if (m_revision != revision)
						return false;
				}
			}

			if (!expireConnections())
				return false;

			// Track peers that cannot finish their current queued packet in this update
			// because send() would block, was interrupted, or sent only part of the data.
			// Skip all later packets for those peers so a later packet cannot be sent
			// before the remaining bytes of an earlier packet. TCP preserves the order
			// of bytes submitted to the socket; we must submit them in the correct order.
			// Other peers may continue sending instead of waiting for the blocked peer.
			// This set is local to update(), so blocked peers are retried next update.
			std::set<irr::u32> blocked;
			std::size_t attempts = 0;
			for (auto packet = m_queue.begin(); packet != m_queue.end() && attempts < maxSendPackets;)
			{
				irr::u32 id = packet->ConnectionID;
				CConnection* peer = getConnection(id);
				if (!peer || peer->m_socket == InvalidSocket || blocked.count(id))
				{
					++packet;
					continue;
				}
				++attempts;
				int flags = 0;
#if defined(MSG_NOSIGNAL)
				flags = MSG_NOSIGNAL;
#endif
				int sent = static_cast<int>(::send(peer->m_socket,
					reinterpret_cast<const char*>(packet->Data.data() + packet->Offset),
					static_cast<int>(packet->Data.size() - packet->Offset), flags));
				if (sent <= 0)
				{
					int error = sent < 0 ? CSocketUtil::getSocketError() : CSocketUtil::invalidArgument();
					if (CSocketUtil::wouldBlock(error) || CSocketUtil::interrupted(error))
					{
						blocked.insert(id);
						++packet;
						continue;
					}
					m_lastError = error;
					success = false;
					disconnect(id, EDisconnectReason::Error);
					if (m_revision != revision)
						return false;
					// Disconnect callbacks can mutate the entire queue.
					packet = m_queue.begin();
					continue;
				}
				packet->Offset += static_cast<std::size_t>(sent);
				if (packet->Offset == packet->Data.size())
				{
					m_queueBytes -= packet->Data.size();
					packet = m_queue.erase(packet);
				}
				else
				{
					blocked.insert(id);
					++packet;
				}
			}
			return success && isOpen();
		}
	}
}
