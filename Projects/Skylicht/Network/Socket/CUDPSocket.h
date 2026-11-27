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
		/// @brief Provide non-blocking IPv4 UDP transport for clients and servers.
		/// @ingroup Network
		/**
		 * Call update() on the owning thread to process queued datagrams and timeouts.
		 * Callbacks run synchronously on that thread and must not throw, destroy this
		 * object, or recursively call update(). UDP does not guarantee delivery or ordering.
		 * Peer pointers are borrowed and must not be deleted by the application. Removing
		 * a peer or closing the socket invalidates its pointer; retain IDs between callbacks.
		 */
		class CUDPSocket : public CBaseSocket
		{
		protected:
			/// Configured server endpoint for client-side receive filtering.
			sockaddr_in m_remoteEndpoint = {};
			irr::u16 m_remotePort = 0;

			/** @brief Initialize a closed transport with its protocol queue policy. */
			CUDPSocket();

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

			/**
			 * @brief Restrict client reception to a remote endpoint.
			 * @param endpoint Native IPv4 endpoint; zero port disables filtering.
			 */
			void setRemote(const sockaddr_in& endpoint);

		public:
			/** @brief Release native handles without invoking callbacks. */
			~CUDPSocket() override;

			/**
			 * @brief Receive datagrams, expire inactive peers, and process queued sends.
			 * @param maxReceivePackets Maximum receive attempts in this update.
			 * @param maxSendPackets Maximum queued send attempts in this update.
			 * @return False if closed, changed by close/open in a callback,
			 * or a socket error occurs; true otherwise, including normal would-block.
			 *
			 * Never waits for socket readiness or resolves DNS. A hard send error drops
			 * that datagram; would-block retains it for a later update. Call regularly
			 * on the owning thread. Zero limits skip the corresponding I/O phase.
			 * Timeout checks use CDateTimeUtils::getTimeBySecond() with one-second resolution.
			 */
			bool update(std::size_t maxReceivePackets = 64, std::size_t maxSendPackets = 64) override;
		};
	}
}
