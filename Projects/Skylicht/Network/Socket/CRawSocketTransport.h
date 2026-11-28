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

#include "CBaseTransport.h"

namespace Skylicht
{
	namespace Network
	{
		/// @brief Non-blocking IPv4 TCP/UDP implementation using operating system sockets.
		/// @ingroup Network
		class CRawSocketTransport : public CBaseTransport
		{
		protected:
			/** @brief Normalize a failed native operation.
			 * @return WouldBlock, Interrupted, or Error with the native error captured.
			 */
			ETransportStatus socketFailure();
			/// @brief Non-blocking native byte I/O without the outer protocol's Ready check.
			/// Protocol subclasses use these while their own handshake is still pending.
			STransportResult sendBytes(const void* data, std::size_t size);
			STransportResult receiveBytes(void* buffer, std::size_t capacity);
		public:
			/** @brief Create a closed raw transport.
			 * @param mode Stream selects TCP; Datagram selects UDP.
			 */
			explicit CRawSocketTransport(ETransportMode mode);
			/// @copydoc ITransport::open
			ETransportStatus open(const STransportEndpoint& local) override;
			/// @copydoc ITransport::connect
			ETransportStatus connect(const STransportEndpoint& remote) override;
			/// @copydoc ITransport::listen
			ETransportStatus listen(int backlog) override;
			/// @copydoc ITransport::accept
			ETransportStatus accept(ITransport*& accepted) override;
			/// @copydoc ITransport::update
			ETransportStatus update(std::size_t budget) override;
			/// @copydoc ITransport::send
			STransportResult send(const void* data, std::size_t size, bool binary = true) override;
			/// @copydoc ITransport::receive
			STransportResult receive(void* buffer, std::size_t capacity) override;
			/// @copydoc ITransport::sendTo
			STransportResult sendTo(const STransportEndpoint& destination, const void* data, std::size_t size) override;
			/// @copydoc ITransport::receiveFrom
			/// @note UDP only; non-blocking reception returns WouldBlock immediately when no datagram is available.
			STransportResult receiveFrom(STransportEndpoint& source, void* buffer, std::size_t capacity) override;
			/// @copydoc ITransport::getLocalEndpoint
			bool getLocalEndpoint(STransportEndpoint& result) const override;
			/// @copydoc ITransport::getRemoteEndpoint
			bool getRemoteEndpoint(STransportEndpoint& result) const override;
		};
	}
}
