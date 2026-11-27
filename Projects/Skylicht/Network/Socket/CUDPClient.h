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

#include "CUDPSocket.h"

namespace Skylicht
{
	namespace Network
	{
		/// @brief Send and receive non-blocking UDP datagrams with one server endpoint.
		/// @ingroup Network
		/**
		 * Only datagrams from the configured server IP and port are accepted.
		 * Call update() regularly on the owning thread to send queued packets,
		 * receive packets, and check timeouts.
		 */
		class CUDPClient : public CUDPSocket
		{
		protected:
			/// ID of the tracked server endpoint; zero before the first successful open.
			irr::u32 m_serverID;

		public:
			/**
			 * @brief Constructor. The socket is initially closed.
			 */
			CUDPClient();

			/**
			 * @brief Open a non-blocking socket and track the server endpoint.
			 *
			 * OnConnected runs synchronously during this call, without a UDP handshake.
			 * Call open() again explicitly after the server peer times out or is removed.
			 *
			 * @param serverAddress Numeric IPv4 server address. Hostnames are not resolved.
			 * @param serverPort Nonzero server port in host byte order.
			 * @param localPort Local port, or zero to let the OS choose a port.
			 * @param bindAddress Numeric local IPv4 address; 0.0.0.0 binds all interfaces.
			 * @return True if the server endpoint remains tracked after OnConnected returns.
			 */
			bool open(const char* serverAddress, irr::u16 serverPort,
				irr::u16 localPort = 0, const char* bindAddress = "0.0.0.0");

			/**
			 * @brief Get the tracked server endpoint.
			 * @return Borrowed pointer, or nullptr if the server is no longer tracked.
			 */
			CConnection* getServer() const;

			/**
			 * @brief Copy a datagram into the send queue for the server.
			 * @param data Payload buffer; nullptr is allowed only when size is zero.
			 * @param size Payload size in bytes, up to 65507.
			 * @return True if queued; false if the peer, payload, or queue capacity is invalid.
			 */
			bool send(const void* data, std::size_t size);

			/**
			 * @brief Queue the full contents of a memory stream for the server.
			 * @param packet Stream to copy, independent of its current read position.
			 * @return True if queued. This does not confirm delivery.
			 */
			bool send(CMemoryStream& packet);
		};
	}
}
