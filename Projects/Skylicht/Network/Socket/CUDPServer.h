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
#include "IServerSocket.h"

namespace Skylicht
{
	namespace Network
	{
		/// @brief Manage multiple peers using a non-blocking UDP socket.
		/// @ingroup Network
		/**
		 * A peer is tracked when its first datagram arrives, up to the configured limit.
		 * Applications validate and authenticate incoming packets themselves.
		 * Call update() regularly on the owning thread to process network traffic.
		 */
		class CUDPServer : public CUDPSocket, public IServerSocket
		{
		public:
			bool send(irr::u32 connectionID, const void* data, std::size_t size) override
			{ return CBaseSocket::send(connectionID, data, size); }

			bool send(irr::u32 connectionID, CMemoryStream& packet) override
			{ return CBaseSocket::send(connectionID, packet); }

			/**
			 * @brief Open and bind a non-blocking UDP server socket.
			 * @param port Local port in host byte order, or zero to let the OS choose a port.
			 * @param maxConnections Maximum number of tracked peers; must be greater than zero.
			 * @param bindAddress Numeric local IPv4 address; 0.0.0.0 binds all interfaces.
			 * @return True on success; false if validation or socket setup fails.
			 */
			bool open(irr::u16 port, std::size_t maxConnections = 16,
				const char* bindAddress = "0.0.0.0") override;

			/**
			 * @brief Queue a separate datagram for each currently tracked peer.
			 *
			 * This sends to tracked endpoints, rather than using an IP broadcast address.
			 * Queue capacity is checked for all recipients before enqueueing any packets.
			 *
			 * @param data Payload buffer; nullptr is allowed only when size is zero.
			 * @param size Payload size in bytes, up to 65507.
			 * @param ignoreID Connection ID to exclude, or zero to include all peers.
			 * @return True if queued for all recipients; false if validation or capacity checks fail.
			 */
			bool broadcast(const void* data, std::size_t size, irr::u32 ignoreID = 0) override;

			/**
			 * @brief Queue the full contents of a memory stream for the tracked peers.
			 * @param packet Stream to copy, independent of its current read position.
			 * @param ignoreID Connection ID to exclude, or zero to include all peers.
			 * @return True if queued for all recipients. This does not confirm delivery.
			 */
			bool broadcast(CMemoryStream& packet, irr::u32 ignoreID = 0) override;
		};
	}
}
