/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

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
#include "CWebSocketUtil.h"
#include "CSocketUtil.h"
#include "ITransport.h"
#if defined(_WIN32)
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

#include "Crypto/base64.h"
#include "aesGladman/sha1.h"
#include <sstream>
#include <cctype>
#include <cstdint>
#include <random>

namespace Skylicht
{
	namespace Network
	{
		bool CWebSocketUtil::randomBytes(unsigned char* data, std::size_t size)
		{
			try
			{
				std::random_device random;
				for (std::size_t i = 0; i < size; ++i)
					data[i] = static_cast<unsigned char>(random());
			}
			catch (...)
			{
				return false;
			}
			return true;
		}

		std::string CWebSocketUtil::base64(const unsigned char* data, std::size_t size)
		{
			std::string result(4 * ((size + 2) / 3), '\0');
			base64_encode(data, reinterpret_cast<unsigned char*>(&result[0]), size, 0);
			return result;
		}

		// SHA-1 is used only for the RFC 6455 upgrade challenge.
		std::string CWebSocketUtil::acceptKey(const std::string& key)
		{
			std::string input = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
			unsigned char digest[SHA1_DIGEST_SIZE];
			sha1(digest, reinterpret_cast<const unsigned char*>(input.data()),
				 static_cast<unsigned long>(input.size()));
			return base64(digest, sizeof(digest));
		}

		std::string CWebSocketUtil::lower(std::string text)
		{
			for (auto& c : text)
				c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
			return text;
		}
		std::string CWebSocketUtil::trim(const std::string& text)
		{
			auto first = text.find_first_not_of(" \t\r");
			return first == std::string::npos ? "" : text.substr(first, text.find_last_not_of(" \t\r") - first + 1);
		}
		bool CWebSocketUtil::token(const std::string& text, const std::string& expected)
		{
			std::istringstream stream(lower(text));
			std::string part;
			while (std::getline(stream, part, ','))
				if (trim(part) == expected)
					return true;
			return false;
		}

		bool CWebSocketUtil::parseURL(const std::string& url, std::string& host, std::string& authority, std::string& path,
			irr::u16& port)
		{
			if (url.compare(0, 5, "ws://") != 0 || url.find_first_of("\r\n\t #") != std::string::npos)
				return false;
			auto end = url.find_first_of("/?", 5);
			authority = url.substr(5, end == std::string::npos ? end : end - 5);
			if (authority.empty() || authority.find_first_of("@[]") != std::string::npos)
				return false;
			path = end == std::string::npos ? "/" : url[end] == '?' ? "/" + url.substr(end) : url.substr(end);
			port = 80;
			auto colon = authority.find(':');
			host = authority.substr(0, colon);
			if (host.empty())
				return false;
			if (colon != std::string::npos)
			{
				std::string text = authority.substr(colon + 1);
				if (text.empty())
					return false;
				unsigned value = 0;
				for (char digit : text)
				{
					if (digit < '0' || digit > '9')
						return false;
					value = value * 10 + digit - '0';
					if (value > 65535)
						return false;
				}
				if (!value)
					return false;
				port = static_cast<irr::u16>(value);
			}
			return true;
		}

		bool CWebSocketUtil::resolveIPv4(const std::string& host, irr::u16 port, STransportEndpoint& endpoint)
		{
			if (CSocketUtil::parseEndpoint(host.c_str(), port, endpoint))
				return true;
#if defined(_WIN32)
			WSADATA runtime;
			if (WSAStartup(MAKEWORD(2, 2), &runtime) != 0)
				return false;
#endif
			addrinfo hints = {};
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			addrinfo* addresses = nullptr;
			bool resolved = false;
			if (::getaddrinfo(host.c_str(), nullptr, &hints, &addresses) == 0)
			{
				for (auto address = addresses; address && !resolved; address = address->ai_next)
				{
					sockaddr_in native = *reinterpret_cast<const sockaddr_in*>(address->ai_addr);
					native.sin_port = htons(port);
					resolved = CSocketUtil::copyEndpoint(native, endpoint);
				}
				::freeaddrinfo(addresses);
			}
#if defined(_WIN32)
			WSACleanup();
#endif
			return resolved;
		}

		bool CWebSocketUtil::isValidUTF8(const unsigned char* bytes, std::size_t size)
		{
			if (!bytes && size)
				return false;
			uint32_t value = 0, minimum = 0;
			int remaining = 0;
			for (std::size_t i = 0; i < size; ++i)
			{
				auto c = bytes[i];
				if (!remaining)
				{
					if (c < 128)
						continue;
					if (c >= 194 && c <= 223)
					{
						remaining = 1;
						value = c & 31;
						minimum = 128;
					}
					else if (c >= 224 && c <= 239)
					{
						remaining = 2;
						value = c & 15;
						minimum = 2048;
					}
					else if (c >= 240 && c <= 244)
					{
						remaining = 3;
						value = c & 7;
						minimum = 65536;
					}
					else
						return false;
				}
				else
				{
					if ((c & 192) != 128)
						return false;
					value = (value << 6) | (c & 63);
					if (!--remaining &&
						(value < minimum || value > 0x10ffff || (value >= 0xd800 && value <= 0xdfff)))
						return false;
				}
			}
			return remaining == 0;
		}
	}
}
