/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

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

#include "CRawSocketTransport.h"
#include <vector>

namespace Skylicht
{
	namespace Network
	{
		/// RFC 6455 frame opcodes (wire values).
		enum class EWebSocketOpcode : unsigned char
		{
			Continuation = 0x0,
			Text = 0x1,
			Binary = 0x2,
			Close = 0x8,
			Ping = 0x9,
			Pong = 0xa
		};

		/**
		 * @brief Non-blocking RFC 6455 WebSocket client transport over IPv4 TCP.
		 * @ingroup Network
		 *
		 * Call inherited open() to bind a local endpoint, connect() with a numeric IP/port,
		 * then update() until the transport becomes Ready. The HTTP request path and Host
		 * header are configured separately; no WebSocket URL parsing or DNS is performed.
		 *
		 * send() queues one complete Text/Binary message. receive() removes protocol headers,
		 * reassembles fragmented messages, and returns one payload with Binary metadata.
		 * Ping/Pong and peer Close frames are handled internally. Extensions, subprotocols,
		 * and server-side listen/accept are unsupported. Operations run on the owning thread.
		 *
		 * For TLS, override handshakeSocket(), readSocket(), and writeSocket() using m_socket.
		 * Override close() to free the TLS session before calling CWebSocketClientTransport::close(),
		 * and call close() from the derived destructor while its TLS resources still exist.
		 */
		class CWebSocketClientTransport : public CRawSocketTransport
		{
		public:
			/// @brief Maximum payload size of one complete message, including all fragments, in bytes.
			static constexpr std::size_t MaxMessageSize = 4 * 1024 * 1024;

			/** @brief Create a closed WebSocket client transport.
			 * @param path HTTP request target starting with '/', optionally including a query string.
			 * @param host HTTP Host header, optionally including a port; empty uses the remote IP:port.
			 * @param origin Optional HTTP Origin header; empty omits the header.
			 */
			explicit CWebSocketClientTransport(const std::string& path = "/", const std::string& host = "",
				const std::string& origin = "");

			/** @brief Start a TCP connection; update() subsequently completes the protocol handshakes.
			 * @param remote Numeric IPv4 endpoint with a nonzero port; requires a successful open().
			 * @return Pending when connection/handshake work remains, or an error status.
			 */
			ETransportStatus connect(const STransportEndpoint& remote) override;

			/** @brief Progress TCP/TLS/HTTP handshakes and flush queued network output.
			 * @param budget Maximum protocol work steps; zero performs no work.
			 * @return Done when Ready with no queued output, Pending while work remains,
			 * or Closed/Error. Ready-state application input is processed by receive().
			 */
			ETransportStatus update(std::size_t budget) override;

			/** @brief Report that this client transport cannot listen for incoming connections.
			 * @param backlog Ignored.
			 * @return Unsupported.
			 */
			ETransportStatus listen(int /*backlog*/) override { return ETransportStatus::Unsupported; }

			/** @brief Report that this client transport cannot accept incoming connections.
			 * @param accepted Unchanged; no transport is created.
			 * @return Unsupported.
			 */
			ETransportStatus accept(ITransport*& /*accepted*/) override { return ETransportStatus::Unsupported; }

			/** @brief Copy one complete message into a masked frame queued for transmission.
			 * @param data Payload bytes; may be nullptr only when size is zero.
			 * @param size Payload length, at most MaxMessageSize; zero sends an empty message.
			 * @param binary True (default) for Binary, false for validated UTF-8 Text.
			 * @return Done with the full payload size when queued, WouldBlock if output is already
			 * pending, Pending during handshakes, or Closed/Error. No partial message is accepted.
			 * @note update() transmits queued bytes; Done does not confirm delivery to the peer.
			 */
			STransportResult send(const void* data, std::size_t size, bool binary = true) override;

			/** @brief Read network bytes and decode one complete application message.
			 * @param buffer Destination for the decoded payload; must be non-null.
			 * @param capacity Nonzero destination capacity in bytes.
			 * @return Done with payload Bytes and Binary metadata, including empty messages;
			 * Pending/WouldBlock when unavailable; or Closed/Error. Truncated reports the required
			 * capacity in Bytes and retains the message for a retry with a larger buffer.
			 * @note Performs at most 32 parse/read steps and exits early when the socket has no data.
			 * Control frames are handled internally and are never returned as application payloads.
			 */
			STransportResult receive(void* buffer, std::size_t capacity) override;

			/** @brief Abort the socket and discard protocol buffers without sending a Close frame.
			 * @note Safe to call repeatedly. Preserves the last error and the configured path/host.
			 */
			void close() override;

		protected:
			/** @brief Complete an optional underlying security handshake before HTTP Upgrade.
			 * @return Done by default. TLS overrides return Pending on WANT_READ/WANT_WRITE,
			 * or Closed/Error, and capture provider failures in m_lastError using the TLS domain.
			 */
			virtual ETransportStatus handshakeSocket() { return ETransportStatus::Done; }

			/** @brief Read raw protocol bytes from the underlying non-blocking channel.
			 * @param buffer Destination for HTTP or WebSocket wire bytes.
			 * @param capacity Maximum bytes to read; must be nonzero.
			 * @return Done with a positive byte count, WouldBlock/Pending/Interrupted, or Closed/Error.
			 * @note Defaults to native TCP receiveBytes(). TLS overrides use SSL_read and map
			 * WANT_READ/WANT_WRITE to Pending/WouldBlock, capturing hard TLS errors in m_lastError.
			 */
			virtual STransportResult readSocket(void* buffer, std::size_t capacity);

			/** @brief Write raw protocol bytes to the underlying non-blocking channel.
			 * @param data HTTP or WebSocket wire bytes to transmit.
			 * @param size Number of available bytes.
			 * @return Done with a positive, possibly partial byte count, a temporary status, or Closed/Error.
			 * @note Defaults to native TCP sendBytes(). TLS overrides use SSL_write and map
			 * WANT_READ/WANT_WRITE to Pending/WouldBlock, capturing hard TLS errors in m_lastError.
			 */
			virtual STransportResult writeSocket(const void* data, std::size_t size);

			/** @brief Generate random bytes for the upgrade nonce and outgoing frame masking keys.
			 * @param data Destination buffer.
			 * @param size Number of random bytes requested.
			 * @return True when all bytes were generated, false on random-source failure.
			 * @note Delegates to CWebSocketUtil::randomBytes(); override for another random source or tests.
			 */
			virtual bool randomBytes(unsigned char* data, std::size_t size);

			/** @brief Record a WebSocket error and transition the transport to Failed.
			 * @param code Error code; defaults to the protocol-error close code 1002.
			 * @return Error. No Close frame is queued by this helper.
			 */
			ETransportStatus protocolError(int code = 1002);

			/** @brief Attempt one write of the remaining m_tx bytes and advance m_txOffset.
			 * @return Done when output is empty, Pending after a partial write, or the channel status.
			 */
			ETransportStatus flush();

			/** @brief Read once from the network into an 8 KiB temporary buffer and append to m_rx.
			 * @return Done when bytes were appended, a temporary channel status, or Closed/Error.
			 * @note The temporary buffer does not limit message size; subsequent calls append more bytes.
			 */
			ETransportStatus readWire();

			/** @brief Parse at most one buffered frame, handling control frames and message fragments.
			 * @param buffer Destination used only when a complete application message is available.
			 * @param capacity Available destination capacity in bytes.
			 * @return Done with the complete payload and Binary metadata; Pending if more wire bytes
			 * are needed; WouldBlock after a control/non-final frame or when control output is blocked;
			 * Truncated with required Bytes when the destination is too small; or Error.
			 * @note Truncated leaves the final frame and accumulated fragments intact for retry.
			 */
			STransportResult parseFrame(void* buffer, std::size_t capacity);

			/** @brief Encode and queue a final, masked client frame without writing to the socket.
			 * @param opcode Frame type selected by the caller.
			 * @param data Payload bytes; may be nullptr only for an empty frame.
			 * @param size Payload length, at most MaxMessageSize; control callers must enforce 125 bytes.
			 * @return Done with the copied payload size, WouldBlock if m_tx is occupied, or Error.
			 */
			STransportResult queueFrame(EWebSocketOpcode opcode, const void* data, std::size_t size);

			/** @brief Generate a fresh nonce and queue the HTTP Upgrade request in m_tx.
			 * @return True when queued; false on nonce generation or request-size failure.
			 * @note Requires empty output and an initialized m_authority; failures record a protocol error.
			 */
			bool beginUpgrade();

			/** @brief Validate the HTTP 101 response, Upgrade headers, and Sec-WebSocket-Accept challenge.
			 * @param headers Complete HTTP response headers, including the terminating blank line.
			 * @return True for a valid upgrade without unsolicited extensions or subprotocols.
			 */
			bool validateUpgrade(const std::string& headers);
			std::string m_path, m_host, m_authority, m_acceptKey, m_origin;
			/// @brief Raw network bytes awaiting parsing: HTTP Upgrade response or WebSocket headers and payloads.
			std::vector<unsigned char> m_rx;
			/// @brief Encoded bytes queued for network transmission: HTTP Upgrade request or WebSocket frames.
			std::vector<unsigned char> m_tx;
			/// @brief Decoded application payload accumulated across frames until a complete message can be delivered.
			std::vector<unsigned char> m_message;
			/// @brief Number of bytes already sent from m_tx; the next write starts at this offset.
			std::size_t m_txOffset = 0;
			EWebSocketOpcode m_opcode = EWebSocketOpcode::Continuation;
			bool m_socketReady = false, m_upgradeSent = false, m_closing = false;
		};
	} // namespace Network
} // namespace Skylicht
