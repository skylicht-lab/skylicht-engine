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

#include "CBaseSocket.h"

namespace Skylicht
{
	namespace Network
	{
		/// @brief Protocol-independent server API. Use a TCP or UDP implementation.
		/// @ingroup Network
		class IServerSocket
		{
		public:
			/**
			 * @brief Destroy the server interface.
			 */
			virtual ~IServerSocket() = default;

			/**
			 * @brief Copy bytes into the send queue for a tracked peer.
			 * @param connectionID ID of the destination peer.
			 * @param data Payload buffer; may be nullptr when size is zero.
			 * @param size Payload size in bytes, within the transport payload limit.
			 * @return True if accepted; false for an invalid peer, payload, or full queue.
			 * @note Success does not confirm delivery.
			 */
			virtual bool send(irr::u32 connectionID, const void* data, std::size_t size) = 0;

			/**
			 * @brief Queue a copy of the full contents of a memory stream for a tracked peer.
			 * @param connectionID ID of the destination peer.
			 * @param packet Stream to copy, independent of its read position.
			 * @return True if accepted into the queue; this does not confirm delivery.
			 */
			virtual bool send(irr::u32 connectionID, CMemoryStream& packet) = 0;

			/**
			 * @brief Bind a numeric IPv4 address and start accepting or tracking peers.
			 * @param port Local port in host byte order, or zero to let the OS select one.
			 * @param maxConnections Maximum number of tracked peers; must be nonzero.
			 * @param bindAddress Numeric local IPv4 address; 0.0.0.0 binds all interfaces.
			 * @return True if transport setup succeeds; false otherwise.
			 */
			virtual bool open(irr::u16 port, std::size_t maxConnections = 16,
				const char* bindAddress = "0.0.0.0") = 0;

			/**
			 * @brief Copy bytes into the send queue for all selected peers.
			 * @param data Payload buffer; may be nullptr when size is zero.
			 * @param size Payload size in bytes, within the transport payload limit.
			 * @param ignoreID Connection ID to exclude, or zero for no exclusion.
			 * @return True if accepted for all selected peers; this does not confirm delivery.
			 */
			virtual bool broadcast(const void* data, std::size_t size, irr::u32 ignoreID = 0) = 0;

			/**
			 * @brief Queue a copy of the full contents of a memory stream for all selected peers.
			 * @param packet Stream to copy, independent of its read position.
			 * @param ignoreID Connection ID to exclude, or zero for no exclusion.
			 * @return True if accepted for all selected peers; this does not confirm delivery.
			 */
			virtual bool broadcast(CMemoryStream& packet, irr::u32 ignoreID = 0) = 0;
		};
	}
}
