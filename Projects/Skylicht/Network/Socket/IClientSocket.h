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
			virtual ~IClientSocket() = default;

			/// Access common socket state, callbacks, and polling without knowing the protocol.
			virtual CBaseSocket& getSocket() = 0;
			virtual const CBaseSocket& getSocket() const = 0;

			/// Start communication with a numeric IPv4 server; success does not guarantee a handshake.
			virtual bool open(const char* serverAddress, irr::u16 serverPort,
				irr::u16 localPort = 0, const char* bindAddress = "0.0.0.0") = 0;

			/// Return the borrowed server peer, or nullptr when unavailable.
			virtual CConnection* getServer() const = 0;

			/// Queue bytes for the server. Payload limits and delivery semantics depend on transport.
			virtual bool send(const void* data, std::size_t size) = 0;

			/// Queue the complete stream for the server, independent of its read position.
			virtual bool send(CMemoryStream& packet) = 0;
		};
	}
}
