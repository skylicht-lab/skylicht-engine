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

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include "irrTypes.h"

namespace Skylicht
{
	namespace Network
	{
		struct STransportEndpoint;

		/// @brief Platform socket operations shared by TCP and UDP transports.
		/// @ingroup Network
		class CSocketUtil
		{
		public:
#if defined(_WIN32)
			using Socket = SOCKET;
			using AddressLength = int; ///< Native address length type.
			static constexpr Socket InvalidSocket = INVALID_SOCKET;
#else
			using Socket = int;
			using AddressLength = socklen_t; ///< Native address length type.
			static constexpr Socket InvalidSocket = -1;
#endif
			/**
			 * @brief Read the last platform socket error.
			 * @return WSAGetLastError() on Windows, or errno on POSIX.
			 */
			static int getSocketError();

			/**
			 * @brief Get the platform invalid-argument error.
			 * @return WSAEINVAL on Windows, or EINVAL on POSIX.
			 */
			static int invalidArgument();

			/**
			 * @brief Check whether a socket operation would block.
			 * @param error Platform socket error code.
			 * @return True for a temporarily unavailable socket.
			 */
			static bool wouldBlock(int error);

			/**
			 * @brief Check whether a socket operation was interrupted.
			 * @param error Platform socket error code.
			 * @return True for WSAEINTR or EINTR.
			 */
			static bool interrupted(int error);

			/**
			 * @brief Convert a numeric IPv4 endpoint to its native representation.
			 * @param ip Numeric IPv4 address; hostnames are not resolved.
			 * @param port Port in host byte order; zero is allowed.
			 * @param result Output address, with family and port initialized even on failure.
			 * @return True if ip is a valid numeric IPv4 address.
			 */
			static bool parseAddress(const char* ip, irr::u16 port, sockaddr_in& result);

			/**
			 * @brief Convert a native IPv4 endpoint to the transport endpoint representation.
			 * @param address Native IPv4 address and port in network byte order.
			 * @param result Output canonical address, host-order port, and cached native SocketAddress.
			 * @return True on successful conversion; false leaves result unchanged.
			 */
			static bool copyEndpoint(const sockaddr_in& address, STransportEndpoint& result);

			/**
			 * @brief Validate and normalize an IPv4 endpoint, synchronizing its native cache.
			 * @param ip Numeric IPv4 address; no DNS lookup.
			 * @param port Port in host byte order; zero is allowed for local endpoints.
			 * @param result Output canonical address, port, and native SocketAddress.
			 * @return True on success; false leaves result unchanged.
			 */
			static bool parseEndpoint(const char* ip, irr::u16 port, STransportEndpoint& result);

			/**
			 * @brief Close a valid native socket handle.
			 *
			 * Does not call WSACleanup() or modify the caller's handle.
			 * @param socket Handle owned by the caller; must not be InvalidSocket.
			 */
			static void closeHandle(Socket socket);

			/**
			 * @brief Enable non-blocking I/O and suppress SIGPIPE where SO_NOSIGPIPE exists.
			 *
			 * On POSIX platforms without SO_NOSIGPIPE, stream sends must use MSG_NOSIGNAL.
			 * @param socket Valid native socket handle to configure.
			 * @return True on success; otherwise read getSocketError() and close the handle.
			 */
			static bool setNonBlocking(Socket socket);

		};
	}
}
