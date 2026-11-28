#pragma once

#include "IWebsocket.h"

#ifndef __EMSCRIPTEN__

#include "Socket/CTCPClient.h"
#include <deque>

namespace Skylicht
{
	namespace Network
	{
		/** @brief Adapt the TCP WebSocket transport to the Socket.IO poll/dispatch interface.
		 * connect() resolves ws:// hostnames synchronously, then starts a non-blocking connection.
		 * poll() progresses the connection; OnConnected runs only after HTTP Upgrade succeeds.
		 * Received Text messages are owned until dispatch() delivers them; Binary messages are ignored.
		 */
		class CWebsocket : public IWebsocket
		{
		protected:
			CTCPClient m_client;				///< Owns the per-connection CWebSocketClientTransport.
			std::deque<std::string> m_messages; ///< Complete Text payloads waiting for dispatch().
			std::size_t m_messageBytes;			///< Total payload bytes queued for dispatch().
			std::size_t m_revision;				///< Changes on reconnect so dispatch stops delivering old messages.
			bool m_closed;						///< True after setup failure or connection termination.
			bool m_connecting; ///< True until the handshake succeeds or connection failure is reported.

			/// @brief Report a pending connection failure once through OnConnectFailed.
			void failConnect();

		public:
			/// @brief Create a closed adapter and install the TCP lifecycle callbacks.
			CWebsocket();
			/// @brief Release the client without application callbacks.
			~CWebsocket() override;

			/** @brief Start a connection using a ws:// URL and an optional HTTP Origin header.
			 * @param url WebSocket URL with a hostname or numeric IPv4 address, port, and request target.
			 * @param origin Optional Origin header value.
			 * @return True when initiation succeeds; poll() reports eventual handshake success/failure.
			 */
			bool connect(const std::string& url, const std::string& origin) override;
			/// @brief Queue a UTF-8 Text message; queue overflow or invalid size closes the client.
			void send(const std::string& message) override;
			/** @brief Progress the non-blocking TCP/WebSocket transport and queue received payloads.
			 * @param timeout Retained for API compatibility; polling does not wait.
			 */
			void poll(int timeout = 0) override;
			/** @brief Deliver queued messages in order, removing each before calling the handler.
			 * @param callable Payload handler; an empty handler leaves the queue unchanged.
			 */
			void dispatch(std::function<void(const std::string&)> callable) override;
			/// @return True when closed or failed; false while connecting or ready.
			bool isClosed() override;
		};
	} // namespace Network
} // namespace Skylicht

#endif
