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

#pragma once

#include <cstddef>
#include <string>
#include "irrTypes.h"

namespace Skylicht
{
	namespace Network
	{
		struct STransportEndpoint;

		/// @brief Protocol helpers shared by WebSocket client and server transports.
		class CWebSocketUtil
		{
		public:
			/** @brief Generate random bytes for upgrade nonces and frame masking keys.
			 * @param data Destination buffer large enough for size bytes.
			 * @param size Number of bytes to generate.
			 * @return True on success, false on random-source failure.
			 * @note Uses std::random_device.
			 */
			static bool randomBytes(unsigned char* data, std::size_t size);

			/// @brief Encode bytes as Base64 without line breaks.
			static std::string base64(const unsigned char* data, std::size_t size);

			/// @brief Compute Sec-WebSocket-Accept from the client's Sec-WebSocket-Key (RFC 6455).
			static std::string acceptKey(const std::string& key);

			/// @brief Return a lowercase copy for HTTP header comparisons.
			static std::string lower(std::string text);

			/// @brief Remove leading/trailing spaces, tabs, and carriage returns from HTTP header text.
			static std::string trim(const std::string& text);

			/// @brief Find a token in a comma-separated HTTP header; expected must be lowercase.
			static bool token(const std::string& text, const std::string& expected);

			/** @brief Split a ws:// URL into hostname, HTTP authority, request target, and port.
			 * @note Defaults to port 80 and path '/'; query-only targets gain a leading '/'.
			 * TLS (wss://), user information, IPv6 literals, and fragments are unsupported.
			 * @return True on success; output arguments may be modified on failure.
			 */
			static bool parseURL(const std::string& url, std::string& host, std::string& authority,
				std::string& path, irr::u16& port);

			/** @brief Resolve a numeric IPv4 address or hostname into a TCP endpoint.
			 * @note Hostname lookup uses blocking getaddrinfo(); call before starting transport I/O.
			 * @return True when an IPv4 address is found and copied to endpoint.
			 */
			static bool resolveIPv4(const std::string& host, irr::u16 port, STransportEndpoint& endpoint);

			/** @brief Validate UTF-8 bytes without converting or modifying the payload.
			 * @param bytes Payload; may be nullptr only when size is zero.
			 * @param size Number of bytes, including any embedded null bytes.
			 * @return False for incomplete sequences, overlong encodings, surrogates,
			 * invalid continuation bytes, or code points above U+10FFFF.
			 */
			static bool isValidUTF8(const unsigned char* bytes, std::size_t size);
		};
	}
}
