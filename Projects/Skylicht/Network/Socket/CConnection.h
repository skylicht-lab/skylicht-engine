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
#include "CSocketUtil.h"
#include <string>

namespace Skylicht
{
	namespace Network
	{
		class CBaseSocket;
		class CUDPSocket;
		class CTCPSocket;

		/// @brief Store peer information independently of the transport protocol.
		/// @ingroup Network
		/**
		 * The transport manages the optional per-peer handle and all I/O. Tracking a peer does not
		 * authenticate it or guarantee delivery. The transport owns this object;
		 * applications attach their own data through UserData.
		 */
		class CConnection final
		{
			friend class CBaseSocket;
			friend class CUDPSocket;
			friend class CTCPSocket;
		protected:
			/**
			 * @brief Dedicated TCP socket used to send and receive data for this peer.
			 *
			 * On a TCP server, this is the connected socket returned by accept(), used
			 * to receive requests from and send responses to the accepted client.
			 * It is separate from the server's listening socket.
			 * On a TCP client, this is the socket connected to the remote server,
			 * transferred here after the connection completes.
			 * UDP does not need a socket per peer: all peers share the transport's
			 * socket and use m_socketAddress with sendto()/recvfrom(). This handle
			 * therefore remains InvalidSocket for UDP and pending TCP connections.
			 * The owning transport closes this handle when the peer is removed.
			 */
			CSocketUtil::Socket m_socket = CSocketUtil::InvalidSocket;
			/// Unique nonzero connection ID within the owning transport.
			irr::u32 m_id;
			/// Numeric IPv4 address of the peer.
			std::string m_address;
			/// Remote port in host byte order.
			irr::u16 m_port;
			/// Cached native IPv4 endpoint, copied once and reused by socket operations.
			sockaddr_in m_socketAddress;
			/// Last receive time from CDateTimeUtils, in milliseconds with one-second resolution.
			irr::u64 m_lastReceiveTime;
			/// Receive inactivity timeout in milliseconds; zero disables timeout.
			irr::u32 m_timeout;

			/**
			 * @brief Initialize peer information for the owning transport.
			 * @param id Nonzero connection ID.
			 * @param address Numeric IPv4 address.
			 * @param socketAddress Validated native IPv4 endpoint, copied into the connection.
			 * @param now Initial receive time in milliseconds from CDateTimeUtils.
			 * @param timeout Receive inactivity timeout in milliseconds, or zero to disable.
			 */
			CConnection(irr::u32 id, const std::string& address,
				const sockaddr_in& socketAddress, irr::u64 now, irr::u32 timeout);

		public:
			CConnection(const CConnection&) = delete;
			CConnection& operator=(const CConnection&) = delete;

			/// Application-owned data, initially nullptr. The SDK never deletes this pointer.
			void* UserData;

			/**
			 * @brief Get the connection ID.
			 * @return Nonzero ID assigned by the owning transport.
			 */
			irr::u32 getID() const { return m_id; }

			/**
			 * @brief Get the remote address.
			 * @return Reference to the numeric IPv4 address.
			 */
			const std::string& getAddress() const { return m_address; }

			/**
			 * @brief Get the remote port.
			 * @return Port in host byte order.
			 */
			irr::u16 getPort() const { return m_port; }

			/**
			 * @brief Get the time of the last received data.
			 * @return Milliseconds from the engine epoch, with one-second resolution.
			 */
			irr::u64 getLastReceiveTime() const { return m_lastReceiveTime; }

			/**
			 * @brief Get the receive inactivity timeout.
			 * @return Timeout in milliseconds, or zero if disabled.
			 */
			irr::u32 getTimeout() const { return m_timeout; }

			/**
			 * @brief Set the receive inactivity timeout for this peer.
			 * @param milliseconds Allowed time without incoming data; zero disables timeout.
			 */
			void setTimeout(irr::u32 milliseconds) { m_timeout = milliseconds; }
		};
	}
}
