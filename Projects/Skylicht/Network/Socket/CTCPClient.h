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

#include "CTCPSocket.h"
#include "IClientSocket.h"

namespace Skylicht
{
	namespace Network
	{
		/// @brief Connect to one server using non-blocking IPv4 TCP.
		/// @ingroup Network
		class CTCPClient : public CTCPSocket, public IClientSocket
		{
		protected:
			/// ID of the tracked server endpoint; zero before the first successful open.
			irr::u32 m_serverID;

		public:
			CBaseSocket& getSocket() override { return *this; }
			const CBaseSocket& getSocket() const override { return *this; }

			/**
			 * @brief Constructor. The socket is initially closed.
			 */
			CTCPClient();

			/**
			 * @brief Start a non-blocking connection to a numeric IPv4 endpoint (no DNS).
			 * True means initiation succeeded. OnConnected runs only from update() after
			 * handshake success; failures notify OnDisconnected(Error/Timeout).
			 * Call open() explicitly to reconnect. getServer() also exposes the pending peer.
			 * @param serverAddress Numeric IPv4 server address; hostnames are not resolved.
			 * @param serverPort Nonzero remote port in host byte order.
			 * @param localPort Local port, or zero to let the OS select one.
			 * @param bindAddress Numeric local IPv4 address; 0.0.0.0 binds all interfaces.
			 * @return True if connection initiation succeeds; false for setup/connect errors.
			 * @note An immediate connect error invokes OnDisconnected(Error) during open().
			 */
			bool open(const char* serverAddress, irr::u16 serverPort,
				irr::u16 localPort = 0, const char* bindAddress = "0.0.0.0") override;

			/**
			 * @brief Get the tracked server endpoint.
			 * @return Borrowed pointer, or nullptr if the server is no longer tracked.
			 */
			CConnection* getServer() const override;

			/**
			 * @brief Copy a byte chunk into the send queue for the server.
			 * @param data Payload buffer; nullptr is allowed only when size is zero.
			 * @param size Payload size in bytes, up to 4 MiB.
			 * @return True if queued; false if the peer, payload, or queue capacity is invalid.
			 */
			bool send(const void* data, std::size_t size) override;

			/**
			 * @brief Queue the full contents of a memory stream for the server.
			 * @param packet Stream to copy, independent of its current read position.
			 * @return True if queued. This does not confirm delivery.
			 */
			bool send(CMemoryStream& packet) override;
		};
	}
}
