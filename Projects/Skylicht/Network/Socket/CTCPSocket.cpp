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

#include <set>

namespace Skylicht
{
	namespace Network
	{
		void CTCPSocket::release()
		{
			m_listening = false;
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
			return CBaseSocket::openSocket(bindAddress, port, maxConnections, ETransportMode::Stream);
		}

		bool CTCPSocket::update(std::size_t maxReceivePackets, std::size_t maxSendPackets)
		{
			if (!isOpen())
				return false;
			m_lastError = 0;
			const auto revision = m_revision;
			bool success = true;
			if (m_listening)
			{
				for (std::size_t i = 0; i < maxReceivePackets; ++i)
				{
					ITransport* accepted = nullptr;
					/**
					 * @brief Poll the listener for one new client without waiting.
					 * Done with a non-null accepted pointer means a new connection was
					 * accepted; its transport may still need a protocol handshake.
					 * WouldBlock means no new client is waiting in the accept queue.
					 * Pending means the backend cannot provide a new connection yet.
					 * Both leave accepted null and stop accepting for this update.
					 * Interrupted leaves accepted null and retries within the accept budget;
					 * other failures are handled below, without creating a peer.
					 * Existing peers keep their CConnection transports and are never
					 * returned by accept(). The OS may reuse a handle value only after
					 * its previous socket has closed; that still represents a new socket.
					 */
					ETransportStatus status = m_transport->accept(accepted);
					if (status == ETransportStatus::WouldBlock || status == ETransportStatus::Pending)
					{
						// WouldBlock: no new client is waiting to connect.
						// Pending: the backend cannot provide a new connection yet.
						break;
					}
					if (status == ETransportStatus::Interrupted)
					{
						// Accept was temporarily interrupted (e.g. EINTR on POSIX).
						// No client was accepted; retry within this update's accept budget.
						continue;
					}
					if (status != ETransportStatus::Done || !accepted)
					{
						// Accept failed, returned an unexpected status, or the backend reported
						// Done without providing a transport. No peer is created in these cases.
						// This is an accept/backend failure, not necessarily a client handshake failure.
						delete accepted;
						m_lastError = m_transport->getLastError().Code;
						success = false;
						break;
					}
					STransportEndpoint remote;
					if (accepted->getMode() != ETransportMode::Stream || !accepted->getRemoteEndpoint(remote))
					{
						delete accepted;
						m_lastError = CSocketUtil::invalidArgument();
						success = false;
						continue;
					}
					// A live endpoint must never have its exclusively owned transport overwritten.
					if (getConnectionByEndpoint(remote))
					{
						delete accepted;
						continue;
					}
					CConnection* connection = addConnection(remote);
					if (!connection)
					{
						delete accepted;
						continue;
					}
					connection->m_transport = accepted;
					// Publish OnConnected only after update() completes any backend handshake.
				}
			}

			// Snapshot IDs because callbacks may remove any peer.
			std::vector<irr::u32> ids;
			for (auto& item : m_connections)
				ids.push_back(item.first);
			for (auto id : ids)
			{
				CConnection* channel = getConnection(id);
				if (!channel || !channel->m_transport)
					continue;
				// Update Connecting transports to check whether the non-blocking client
				// connection is still pending, has completed, or has failed.
				// Also progress backend handshakes, even when the receive/send budgets are zero.
				ETransportStatus progress = channel->m_transport->update(1);
				if (progress == ETransportStatus::Error || progress == ETransportStatus::Closed)
				{
					if (progress == ETransportStatus::Error)
					{
						m_lastError = channel->m_transport->getLastError().Code;
						success = false;
					}
					disconnect(id, progress == ETransportStatus::Error ? EDisconnectReason::Error : EDisconnectReason::RemoteClosed);
					if (m_revision != revision)
						return false;
					continue;
				}
				if (channel->m_transport->getState() != ETransportState::Ready)
					continue;
				if (!channel->m_connected)
				{
					channel->m_connected = true;
					if (!notifyConnected(id))
					{
						if (m_revision != revision)
							return false;
						continue;
					}
				}
				for (std::size_t i = 0; i < maxReceivePackets; ++i)
				{
					CConnection* peer = getConnection(id);
					if (!peer || !peer->m_transport)
						break;
					// The native socket is configured as non-blocking, so recv() never waits
					// for data. If none is available, WSAEWOULDBLOCK/EAGAIN/EWOULDBLOCK
					// is mapped to WouldBlock; stop receiving for this peer and retry next update.
					STransportResult received = peer->m_transport->receive(m_buffer.data(), m_buffer.size());
					if (received.Status == ETransportStatus::Truncated &&
						received.Bytes > m_buffer.size() && received.Bytes <= MaxQueueBytes)
					{
						m_buffer.resize(received.Bytes);
						received = peer->m_transport->receive(m_buffer.data(), m_buffer.size());
					}
					if (received.Status == ETransportStatus::WouldBlock || received.Status == ETransportStatus::Pending)
						break;
					if (received.Status == ETransportStatus::Interrupted)
						continue;
					if (received.Status != ETransportStatus::Done)
					{
						bool error = received.Status != ETransportStatus::Closed;
						if (error)
						{
							m_lastError = peer->m_transport->getLastError().Code;
							success = false;
						}
						disconnect(id, error ? EDisconnectReason::Error : EDisconnectReason::RemoteClosed);
						if (m_revision != revision)
							return false;
						break;
					}
					CConnection* connection = getConnection(id);
					connection->m_lastReceiveTime = static_cast<irr::u64>(CDateTimeUtils::getTimeBySecond()) * 1000;
					CMemoryStream packet(m_buffer.data(), static_cast<unsigned int>(received.Bytes));
					packet.setBinary(received.Binary);
					auto callback = OnReceived;
					if (callback)
						callback(connection, &packet);
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
				if (!peer || !peer->m_transport || !peer->m_connected || blocked.count(id))
				{
					++packet;
					continue;
				}
				++attempts;
				STransportResult sent = peer->m_transport->send(packet->Data.data() + packet->Offset,
					packet->Data.size() - packet->Offset, packet->Binary);
				if (sent.Status != ETransportStatus::Done || (!sent.Bytes && !packet->Data.empty()))
				{
					if (sent.Status == ETransportStatus::WouldBlock || sent.Status == ETransportStatus::Pending ||
						sent.Status == ETransportStatus::Interrupted || (sent.Status == ETransportStatus::Done && !sent.Bytes))
					{
						blocked.insert(id);
						++packet;
						continue;
					}
					m_lastError = peer->m_transport->getLastError().Code;
					success = false;
					disconnect(id, EDisconnectReason::Error);
					if (m_revision != revision)
						return false;
					packet = m_queue.begin();
					continue;
				}
				packet->Offset += sent.Bytes;
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
