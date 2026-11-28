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
	class CMemoryStream;

	namespace Network
	{
		/// @brief Non-blocking IPv4 TCP transport. Call update() on the owning thread.
		/// @ingroup Network
		/**
		 * Raw TCP OnReceived delivers arbitrary byte chunks; applications frame their protocol.
		 * Message transports deliver a complete message with CMemoryStream payload metadata.
		 * Streams are valid only in the callback.
		 * Peer pointers are borrowed and invalid after removal/close. Callbacks must not
		 * throw, destroy this object, or recursively call update(). They may remove peers
		 * or close/reopen the transport. UserData remains application-owned.
		 */
		class CTCPSocket : public CBaseSocket
		{
		protected:
			bool m_listening = false;

			/** @brief Initialize a closed transport with its protocol queue policy. */
			CTCPSocket();

			/** @brief Release native handles and protocol state without callbacks. */
			void release() override;

			/**
			 * @brief Close the previous transport and bind a non-blocking socket.
			 * @param bindAddress Numeric local IPv4 address.
			 * @param port Local port, or zero for an OS-selected port.
			 * @param maxConnections Maximum tracked peers; must be nonzero.
			 * @return True on successful validation, creation, configuration, and bind.
			 */
			bool openSocket(const char* bindAddress, irr::u16 port, std::size_t maxConnections);

		public:
			/** @brief Release native handles without invoking callbacks. */
			~CTCPSocket() override;

			/**
			 * @brief Check for an owned listener, pending connect, or connected socket.
			 * @return True if open; a pending connection may not have completed.
			 */
			bool isOpen() const override { return m_transport != nullptr || !m_connections.empty(); }

			/**
			 * @brief Poll connection setup, receive bytes, expire peers, and flush sends.
			 *
			 * Never waits for readiness or resolves DNS. Connect completion is always polled.
			 * Call regularly on the owning thread. Peer errors disconnect only that peer.
			 * Timeouts use CDateTimeUtils with one-second resolution; zero disables them.
			 * @param maxReceivePackets Maximum accepts and maximum receive attempts per peer; zero skips these phases.
			 * @param maxSendPackets Maximum total send attempts; zero skips sends.
			 * @return False if closed, closed/reopened in a callback, or an I/O error occurs.
			 */
			bool update(std::size_t maxReceivePackets = 64, std::size_t maxSendPackets = 64) override;
		};
	}
}
