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

#include "pch.h"
#include "CSocketUtil.h"
#include "ITransport.h"

#if defined(_WIN32)
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
#include <cstring>

namespace Skylicht
{
	namespace Network
	{
		void CSocketUtil::closeHandle(Socket socket)
		{
#if defined(_WIN32)
			closesocket(socket);
#else
			::close(socket);
#endif
		}

		bool CSocketUtil::setNonBlocking(Socket socket)
		{
#if defined(_WIN32)
			u_long nonBlocking = 1;
			return ioctlsocket(socket, FIONBIO, &nonBlocking) == 0;
#else
			int flags = fcntl(socket, F_GETFL, 0);
			if (flags == -1 || fcntl(socket, F_SETFL, flags | O_NONBLOCK) != 0)
				return false;
#if defined(SO_NOSIGPIPE)
			int enabled = 1;
			if (setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &enabled, sizeof(enabled)) != 0)
				return false;
#endif
			return true;
#endif
		}

		int CSocketUtil::getSocketError()
		{
#if defined(_WIN32)
			return WSAGetLastError();
#else
			return errno;
#endif
		}

		int CSocketUtil::invalidArgument()
		{
#if defined(_WIN32)
			return WSAEINVAL;
#else
			return EINVAL;
#endif
		}

		bool CSocketUtil::wouldBlock(int error)
		{
#if defined(_WIN32)
			return error == WSAEWOULDBLOCK;
#else
			return error == EAGAIN || error == EWOULDBLOCK;
#endif
		}

		bool CSocketUtil::interrupted(int error)
		{
#if defined(_WIN32)
			return error == WSAEINTR;
#else
			return error == EINTR;
#endif
		}

		bool CSocketUtil::copyEndpoint(const sockaddr_in& address, STransportEndpoint& result)
		{
			char text[INET_ADDRSTRLEN];
			if (address.sin_family != AF_INET || !inet_ntop(AF_INET, &address.sin_addr, text, sizeof(text)))
				return false;
			result.Address = text;
			result.Port = ntohs(address.sin_port);
			result.SocketAddress = address;
			return true;
		}

		bool CSocketUtil::parseEndpoint(const char* ip, irr::u16 port, STransportEndpoint& result)
		{
			sockaddr_in address;
			return parseAddress(ip, port, address) && copyEndpoint(address, result);
		}

		bool CSocketUtil::parseAddress(const char* ip, irr::u16 port, sockaddr_in& result)
		{
			std::memset(&result, 0, sizeof(result));
			result.sin_family = AF_INET;
			result.sin_port = htons(port);
			return ip && inet_pton(AF_INET, ip, &result.sin_addr) == 1;
		}

	}
}
