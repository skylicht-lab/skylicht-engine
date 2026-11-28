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

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include "irrTypes.h"
#include <cstddef>
#include <string>

namespace Skylicht
{
	namespace Network
	{
		/// @brief Payload delivery semantics of a transport.
		enum class ETransportMode { Stream, Datagram };

		/// @brief Lifecycle of a non-blocking transport.
		enum class ETransportState { Closed, Bound, Listening, Connecting, Handshaking, Ready, Failed };

		/// @brief Operation outcome, independent of the number of transferred bytes.
		enum class ETransportStatus { Done, Pending, WouldBlock, Interrupted, Closed, Truncated, Unsupported, Error };

		/// @brief Origin of a transport error code.
		enum class ETransportErrorDomain { None, Socket, TLS, WebSocket };

		/// @brief Numeric IPv4 endpoint with its cached native representation.
		/// Use CSocketUtil::parseEndpoint() or copyEndpoint() to synchronize all fields.
		/// Address and Port are authoritative for input; SocketAddress is a normalized cache.
		struct STransportEndpoint
		{
			std::string Address = "0.0.0.0"; ///< Numeric IPv4 address; no DNS resolution.
			irr::u16 Port = 0; ///< Port in host byte order; zero allows an ephemeral local port.
			sockaddr_in SocketAddress = {}; ///< Cached AF_INET endpoint in network byte order.

			/** @brief Order normalized endpoints for associative containers.
			 * @param other Endpoint to compare; native padding is never compared.
			 * @return True when this numeric address/port precedes the other endpoint.
			 */
			bool operator<(const STransportEndpoint& other) const
			{
				return Address < other.Address || (Address == other.Address && Port < other.Port);
			}
		};

		/// @brief Error snapshot captured by the transport before another operation can overwrite it.
		struct STransportError
		{
			ETransportErrorDomain Domain = ETransportErrorDomain::None; ///< Error provider.
			int Code = 0; ///< Provider-specific code; zero means no error.
		};

		/// @brief Result of a payload operation.
		struct STransportResult
		{
			ETransportStatus Status; ///< Operation outcome, including EOF and temporary unavailability.
			std::size_t Bytes; ///< Payload bytes consumed or received, excluding protocol overhead.
			/** @brief Construct a payload operation result.
			 * @param status Operation outcome.
			 * @param bytes Number of payload bytes accepted or received.
			 */
			STransportResult(ETransportStatus status, std::size_t bytes = 0) : Status(status), Bytes(bytes) {}
		};

		/// @brief One non-blocking listener, stream channel, or shared datagram socket.
		/// @ingroup Network
		/**
		 * All operations run on the owning thread and never wait for readiness or DNS.
		 * The owner is CBaseSocket or CConnection; transport pointers are never shared.
		 * Stream backends may implement TLS or binary WebSocket payloads as a byte stream.
		 * Datagram backends must preserve packet boundaries. Unsupported operations return Unsupported.
		 */
		class ITransport
		{
		public:
			/// @brief Release the channel without application callbacks.
			virtual ~ITransport() = default;
			/// @return Payload delivery mode, fixed for this transport.
			virtual ETransportMode getMode() const = 0;
			/// @return Current lifecycle state; Ready means all protocol handshakes completed.
			virtual ETransportState getState() const = 0;
			/// @return Most recent hard error; temporary statuses do not overwrite it.
			virtual STransportError getLastError() const = 0;
			/** @brief Create, configure as non-blocking, and bind a channel.
			 * @param local Numeric bind endpoint; port zero selects an ephemeral port.
			 * @return Done when bound, or Error. Reopening closes the previous channel.
			 */
			virtual ETransportStatus open(const STransportEndpoint& local) = 0;
			/** @brief Begin a stream connection and any protocol handshake.
			 * @param remote Numeric remote endpoint with nonzero port.
			 * @return Done when ready, Pending while connecting/handshaking, or an error status.
			 */
			virtual ETransportStatus connect(const STransportEndpoint& remote) = 0;
			/** @brief Begin accepting stream clients.
			 * @param backlog Maximum pending native connections.
			 * @return Done on success, Unsupported for datagrams, or Error.
			 */
			virtual ETransportStatus listen(int backlog) = 0;
			/** @brief Accept one client without waiting.
			 * @param accepted Output pointer, initially nullptr. Done transfers exclusive ownership
			 * to the caller; every other result leaves it nullptr. The channel may need a handshake.
			 * @return Done, WouldBlock, Interrupted, Unsupported, or Error.
			 */
			virtual ETransportStatus accept(ITransport*& accepted) = 0;
			/** @brief Progress connect, handshakes, and bounded internal protocol buffers.
			 * @param budget Maximum backend work units; zero performs no work.
			 * @return Done when ready with no pending output, Pending while work remains,
			 * or Closed/Error. A backend handles its own read/write readiness, including
			 * TLS writes needing reads and reads needing writes.
			 */
			virtual ETransportStatus update(std::size_t budget) = 0;
			/** @brief Submit stream payload bytes without waiting.
			 * @param data Payload; may be nullptr only when size is zero.
			 * @param size Payload length.
			 * @return Bytes accepted by this transport, possibly partial. Accepted bytes must
			 * be sent or copied before return; the caller may release them. Internal copies
			 * must be bounded and flushed by update(). Zero-length sends are Done with zero bytes.
			 */
			virtual STransportResult send(const void* data, std::size_t size) = 0;
			/** @brief Receive stream payload, including already decoded internal data.
			 * @param buffer Destination buffer.
			 * @param capacity Nonzero buffer size.
			 * @return Done with bytes, WouldBlock/ Pending when unavailable, or Closed on EOF.
			 */
			virtual STransportResult receive(void* buffer, std::size_t capacity) = 0;
			/** @brief Submit exactly one datagram, including an empty datagram.
			 * @param destination Numeric remote endpoint with nonzero port.
			 * @param data Payload; may be nullptr when size is zero.
			 * @param size Datagram length.
			 * @return Done with the full length, or a failure with no payload accepted.
			 */
			virtual STransportResult sendTo(const STransportEndpoint& destination, const void* data, std::size_t size) = 0;
			/** @brief Receive one complete datagram without waiting.
			 * @param source Output normalized remote endpoint with native cache, valid on Done.
			 * @param buffer Destination buffer.
			 * @param capacity Nonzero buffer size.
			 * @return Done, including zero bytes for an empty packet; Truncated discards the
			 * oversized packet and must not be delivered as an application message.
			 * Truncated means the receive buffer was too small for the datagram. The OS
			 * consumed it and discarded excess bytes; a later update cannot receive the
			 * remainder. Recovery requires application-level retransmission by the sender.
			 */
			virtual STransportResult receiveFrom(STransportEndpoint& source, void* buffer, std::size_t capacity) = 0;
			/** @brief Query the bound endpoint.
			 * @param result Output normalized numeric endpoint with native cache.
			 * @return True if available.
			 */
			virtual bool getLocalEndpoint(STransportEndpoint& result) const = 0;
			/** @brief Query the connected peer endpoint.
			 * @param result Output normalized numeric endpoint with native cache.
			 * @return True if available; false for listeners and unconnected datagrams.
			 */
			virtual bool getRemoteEndpoint(STransportEndpoint& result) const = 0;
			/// @brief Abort the channel and discard internal buffers; safe to call repeatedly.
			virtual void close() = 0;
		};
	}
}
