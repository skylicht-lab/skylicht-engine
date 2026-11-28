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
		/// @brief Protocol-independent client API. Use a TCP or UDP implementation.
		/// @ingroup Network
		class IClientSocket
		{
		public:
			/**
			 * @brief Destroy the client interface.
			 */
			virtual ~IClientSocket() = default;

			/**
			 * @brief Start communication with a numeric IPv4 server.
			 * @param serverAddress Numeric IPv4 server address; hostnames are not resolved.
			 * @param serverPort Nonzero remote port in host byte order.
			 * @param localPort Local port, or zero to let the OS select one.
			 * @param bindAddress Numeric local IPv4 address; 0.0.0.0 binds all interfaces.
			 * @return True if setup succeeds; this does not guarantee a completed handshake.
			 */
			virtual bool open(const char* serverAddress, irr::u16 serverPort,
				irr::u16 localPort = 0, const char* bindAddress = "0.0.0.0") = 0;

			/**
			 * @brief Get the tracked server endpoint.
			 * @return Borrowed peer pointer, or nullptr when unavailable. Peer removal invalidates it.
			 */
			virtual CConnection* getServer() const = 0;

			/**
			 * @brief Copy bytes into the send queue for the server.
			 * @param data Payload buffer; may be nullptr when size is zero.
			 * @param size Payload size in bytes, within the transport payload limit.
			 * @return True if accepted; false for an invalid peer, payload, or full queue.
			 * @note Success does not confirm delivery. Delivery semantics depend on the transport.
			 */
			virtual bool send(const void* data, std::size_t size) = 0;

			/**
			 * @brief Queue a copy of the full contents of a memory stream for the server.
			 * @param packet Stream to copy, independent of its read position.
			 * @return True if accepted into the queue; this does not confirm delivery.
			 */
			virtual bool send(CMemoryStream& packet) = 0;
		};
	}
}
