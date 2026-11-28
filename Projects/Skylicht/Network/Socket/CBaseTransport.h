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

#include "ITransport.h"
#include "CSocketUtil.h"

namespace Skylicht
{
	namespace Network
	{
		/// @brief Exclusive native socket ownership and state shared by socket-backed transports.
		/// @ingroup Network
		class CBaseTransport : public ITransport
		{
		protected:
			using Socket = CSocketUtil::Socket; ///< Platform native handle type.
			Socket m_socket = CSocketUtil::InvalidSocket; ///< Exclusively owned native handle.
			ETransportMode m_mode; ///< Fixed stream/datagram semantics.
			ETransportState m_state = ETransportState::Closed; ///< Current lifecycle state.
			STransportError m_lastError; ///< Captured provider error.
#if defined(_WIN32)
			bool m_wsaReady = false; ///< This object's successful WSAStartup reference.
#endif
			/** @brief Initialize a closed transport.
			 * @param mode Payload delivery semantics.
			 */
			CBaseTransport(ETransportMode mode);
			/** @brief Capture a native error and optionally fail the channel.
			 * @param code Platform error code.
			 * @param fatal True to transition to Failed.
			 * @return Error status.
			 */
			ETransportStatus setError(int code, bool fatal = false);
			/** @brief Acquire a Windows socket runtime reference when required.
			 * @return True on success; otherwise records the startup error.
			 */
			bool initializeRuntime();
		public:
			/// @brief Close the owned native socket and release the runtime reference.
			~CBaseTransport() override;
			/// @brief Disable copying of exclusively owned native resources.
			CBaseTransport(const CBaseTransport&) = delete;
			/// @brief Disable assignment of exclusively owned native resources.
			CBaseTransport& operator=(const CBaseTransport&) = delete;
			/// @return Fixed payload delivery mode.
			ETransportMode getMode() const override { return m_mode; }
			/// @return Current channel lifecycle state.
			ETransportState getState() const override { return m_state; }
			/// @return Captured provider error.
			STransportError getLastError() const override { return m_lastError; }
			/// @brief Close the handle once and reset state; preserve the last error.
			void close() override;
		};
	}
}
