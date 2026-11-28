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

#pragma once

#include "CConnection.h"
#include "CSocketUtil.h"
#include "ITransport.h"
#include <cstddef>
#include <deque>
#include <functional>
#include <map>
#include <vector>

namespace Skylicht
{
	class CMemoryStream;

	namespace Network
	{
		/// @brief Shared peer ownership, events, timeouts, and send queues for IPv4 transports.
		/// @ingroup Network
		/**
		 * Call update() on the owning thread. Callbacks must not throw, destroy the
		 * transport, or recursively call update(). They may remove peers or close/reopen
		 * the transport. Peer pointers are borrowed; retain IDs between callbacks.
		 * TCP delivers arbitrary byte chunks; UDP delivers complete datagrams.
		 */
		class CBaseSocket
		{
		public:
			/// Reason why a connected or pending peer was removed.
			enum class EDisconnectReason
			{
				Removed, ///< Application removed the peer.
				Timeout, ///< Receive inactivity or connect timeout expired.
				Closed, ///< Application closed the transport.
				RemoteClosed, ///< Remote endpoint closed its sending direction (EOF).
				Error ///< Connect, receive, or send failed; see getLastError().
			};

			/**
			 * @brief Notify that a peer became available.
			 *
			 * TCP notifies in update() after a handshake. UDP notifies when an endpoint is
			 * tracked, including synchronously during client open(), without a handshake.
			 * The argument is a borrowed peer pointer; removal or close invalidates it.
			 */
			std::function<void(CConnection*)> OnConnected;

			/**
			 * @brief Deliver received bytes and their source peer.
			 *
			 * The non-null stream pointer is borrowed and valid only during this callback.
			 * Do not delete or retain it; copy bytes if needed after the callback.
			 * TCP may split or combine sends; UDP preserves datagram boundaries.
			 * Validate lengths before reading values and frame TCP application messages.
			 * Removing the peer or closing the transport invalidates the borrowed peer.
			 */
			std::function<void(CConnection*, CMemoryStream*)> OnReceived;

			/**
			 * @brief Notify that a connected or pending peer was removed.
			 *
			 * Arguments are the departing peer and the disconnect reason. The peer remains
			 * valid during the callback. The application retains ownership of UserData.
			 * TCP closes both directions on EOF and discards queued sends.
			 */
			std::function<void(CConnection*, EDisconnectReason)> OnDisconnected;

		protected:
			ITransport* m_transport = nullptr; ///< Owned listener or shared UDP channel.
			std::function<ITransport*(ETransportMode)> m_transportFactory; ///< Creates exclusively owned closed channels.
			bool m_closing = false;
			struct SPacket
			{
				irr::u32 ConnectionID;
				std::vector<unsigned char> Data;
				std::size_t Offset = 0;
				bool Binary = true;
			};
			std::map<irr::u32, CConnection*> m_connections;
			std::map<STransportEndpoint, irr::u32> m_endpointConnections;
			std::deque<SPacket> m_queue;
			std::vector<unsigned char> m_buffer;
			std::size_t m_queueBytes = 0;
			std::size_t m_maxConnections = 16;
			irr::u32 m_defaultTimeout = 20000;
			irr::u32 m_nextID = 1;
			irr::u64 m_revision = 0;
			irr::u16 m_localPort = 0;
			int m_lastError = 0;
			const std::size_t m_maxPayloadSize;
			const bool m_queueEmptyPackets;
			static constexpr std::size_t MaxQueuePackets = 1024;
			static constexpr std::size_t MaxQueueBytes = 4 * 1024 * 1024;

			/**
			 * @brief Initialize a closed transport and its queue policy.
			 * @param maxPayloadSize Maximum bytes accepted in one send.
			 * @param queueEmptyPackets True to queue empty datagrams; false for stream no-ops.
			 */
			CBaseSocket(std::size_t maxPayloadSize, bool queueEmptyPackets);

			/**
			 * @brief Close all native handles and discard queued bytes without callbacks.
			 */
			virtual void release();

			/**
			 * @brief Close the previous transport and bind a new non-blocking IPv4 socket.
			 * @param bindAddress Numeric local IPv4 address.
			 * @param port Local port in host byte order, or zero for an OS-selected port.
			 * @param maxConnections Maximum tracked peers; must be nonzero.
			 * @param mode Stream or datagram semantics required from the factory.
			 * @return True if validation, creation, configuration, and binding succeed.
			 */
			bool openSocket(const char* bindAddress, irr::u16 port, std::size_t maxConnections,
				ETransportMode mode);

			/**
			 * @brief Find or create peer metadata without invoking callbacks.
			 * @param endpoint Numeric IPv4 endpoint with a nonzero remote port.
			 * @return Borrowed peer pointer, or nullptr if validation or capacity checks fail.
			 */
			CConnection* addConnection(const STransportEndpoint& endpoint);

			/**
			 * @brief Invoke OnConnected for a tracked peer.
			 * @param id Connection ID.
			 * @return True if the same transport and peer remain after the callback.
			 */
			bool notifyConnected(irr::u32 id);

			/**
			 * @brief Close a peer handle, discard its queued bytes, and notify the application.
			 * @param id Connection ID; unknown IDs are ignored.
			 * @param reason Reason passed to OnDisconnected.
			 */
			void disconnect(irr::u32 id, EDisconnectReason reason);

			/**
			 * @brief Release protocol-specific resources of a departing peer before callbacks.
			 * @param connection Departing peer, already detached from the peer and endpoint tables.
			 */
			virtual void releaseConnection(CConnection* connection);

			/**
			 * @brief Disconnect inactive peers using a snapshot safe for callback mutations.
			 * @return False if a callback closed or reopened the transport; true otherwise.
			 */
			bool expireConnections();

			/**
			 * @brief Validate payload and total queue capacity for all recipients.
			 * @param data Payload buffer; may be nullptr when size is zero.
			 * @param size Payload size in bytes, within the transport payload limit.
			 * @param count Number of recipients to enqueue.
			 * @return True if payload and available queue capacity are valid.
			 */
			bool canQueue(const void* data, std::size_t size, std::size_t count) const;

			/**
			 * @brief Copy validated payload bytes into the send queue.
			 * @param id Destination connection ID.
			 * @param data Payload buffer to copy.
			 * @param size Number of bytes to copy.
			 */
			void enqueue(irr::u32 id, const void* data, std::size_t size);

			/**
			 * @brief Queue bytes for all selected peers after checking total capacity.
			 * @param data Payload buffer; may be nullptr when size is zero.
			 * @param size Payload size in bytes, within the transport payload limit.
			 * @param ignoreID Connection ID to exclude, or zero for no exclusion.
			 * @return True if queued for all recipients; no partial enqueue on capacity failure.
			 */
			bool sendToAll(const void* data, std::size_t size, irr::u32 ignoreID);

		public:
			/**
			 * @brief Release handles and owned peers without invoking callbacks.
			 */
			virtual ~CBaseSocket();

			/**
			 * @brief Configure creation of transport channels while this socket is closed.
			 * @param factory Returns a new exclusively owned, closed channel of the requested
			 * mode. Must not throw or reenter this socket. Empty restores the raw backend.
			 * Accepted channels are created by the listener backend itself.
			 * @return False if open or closing; true when the factory is installed.
			 */
			bool setTransportFactory(const std::function<ITransport*(ETransportMode)>& factory);

			/**
			 * @brief Disable copying of owned socket resources.
			 */
			CBaseSocket(const CBaseSocket&) = delete;

			/**
			 * @brief Disable assignment of owned socket resources.
			 */
			CBaseSocket& operator=(const CBaseSocket&) = delete;

			/**
			 * @brief Close all sockets, discard queued bytes, and remove peers.
			 *
			 * OnDisconnected runs for each peer with reason Closed. Reentrant close() is ignored.
			 * Opening a transport from these Closed callbacks is rejected.
			 */
			void close();

			/**
			 * @brief Check whether a listener, pending connect, or connected socket is owned.
			 * @return True if open; this does not guarantee a completed connection.
			 */
			virtual bool isOpen() const { return m_transport != nullptr; }

			/**
			 * @brief Get the bound local port.
			 * @return Port in host byte order, or zero after close().
			 */
			irr::u16 getLocalPort() const { return m_localPort; }

			/**
			 * @brief Get the latest setup or update error.
			 *
			 * Would-block is normal and is not recorded as an error.
			 * @return Platform socket error, or zero if no error was recorded.
			 */
			int getLastError() const { return m_lastError; }

			/**
			 * @brief Get the number of tracked peers, including pending connects.
			 * @return Current peer count.
			 */
			std::size_t getConnectionCount() const { return m_connections.size(); }

			/**
			 * @brief Find a peer by ID.
			 * @param id Connection ID.
			 * @return Borrowed peer pointer, or nullptr if absent. Removal invalidates it.
			 */
			CConnection* getConnection(irr::u32 id) const;

			/**
			 * @brief Find a peer by its normalized IPv4 endpoint.
			 * @param endpoint Numeric address and port in host byte order.
			 * @return Borrowed peer pointer, or nullptr if absent or invalid.
			 */
			CConnection* getConnectionByEndpoint(const STransportEndpoint& endpoint) const;

			/** @brief Find a peer using a native IPv4 endpoint for compatibility.
			 * @param endpoint Native address and port in network byte order.
			 * @return Borrowed peer pointer, or nullptr if absent or invalid.
			 */
			CConnection* getConnectionByEndpoint(const sockaddr_in& endpoint) const;

			/**
			 * @brief Disconnect a peer with reason Removed.
			 * @param id Connection ID; unknown IDs are ignored.
			 */
			void removeConnection(irr::u32 id);

			/**
			 * @brief Set receive inactivity timeout for new peers, including pending connects.
			 *
			 * Existing peers keep their timeout; use CConnection::setTimeout() to change it.
			 * @param milliseconds Timeout in milliseconds; zero disables timeout.
			 */
			void setDefaultTimeout(irr::u32 milliseconds) { m_defaultTimeout = milliseconds; }

			/**
			 * @brief Copy bytes into the send queue for a connected or pending peer.
			 *
			 * The queue holds at most 1024 entries and 4 MiB. TCP empty sends are no-ops;
			 * UDP empty sends enqueue a datagram. Maximum payload is 4 MiB for TCP, 65507 for UDP.
			 * Success does not confirm delivery. Partial sends resume in update().
			 * @param connectionID ID of a tracked peer.
			 * @param data Payload buffer; may be nullptr when size is zero.
			 * @param size Payload size in bytes, within the transport payload limit.
			 * @return True if accepted; false for invalid input, peer, or full queue.
			 */
			bool send(irr::u32 connectionID, const void* data, std::size_t size);

			/**
			 * @brief Queue a copy of the full contents of a memory stream.
			 * @param connectionID ID of a tracked peer.
			 * @param packet Stream to copy, independent of its read position.
			 * @return True if accepted into the queue; this does not confirm delivery.
			 */
			bool send(irr::u32 connectionID, CMemoryStream& packet);

			/**
			 * @brief Poll connection setup, receive bytes, expire peers, and flush sends.
			 *
			 * Never waits for readiness or resolves DNS. TCP connect completion is always polled.
			 * Call regularly on the owning thread. TCP peer errors disconnect only that peer.
			 * Timeouts use CDateTimeUtils with one-second resolution; zero disables them.
			 * @param maxReceivePackets Receive attempt budget (per peer for TCP, total for UDP);
			 * also limits TCP accepts. Zero skips these phases.
			 * @param maxSendPackets Maximum total send attempts; zero skips sends.
			 * @return False if closed, closed/reopened in a callback, or an I/O error occurs.
			 */
			virtual bool update(std::size_t maxReceivePackets = 64, std::size_t maxSendPackets = 64) = 0;
		};
	}
}
