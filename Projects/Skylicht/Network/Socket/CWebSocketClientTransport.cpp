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
#include "CWebSocketClientTransport.h"
#include "CWebSocketUtil.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <sstream>

namespace Skylicht
{
	namespace Network
	{

		CWebSocketClientTransport::CWebSocketClientTransport(const std::string& path, const std::string& host, const std::string& origin)
			: CRawSocketTransport(ETransportMode::Stream), m_path(path), m_host(host), m_origin(origin)
		{
		}

		bool CWebSocketClientTransport::randomBytes(unsigned char* data, std::size_t size)
		{
			return CWebSocketUtil::randomBytes(data, size);
		}

		ETransportStatus CWebSocketClientTransport::protocolError(int code)
		{
			m_lastError.Domain = ETransportErrorDomain::WebSocket;
			m_lastError.Code = code;
			m_state = ETransportState::Failed;
			return ETransportStatus::Error;
		}

		void CWebSocketClientTransport::close()
		{
			m_rx.clear();
			m_tx.clear();
			m_message.clear();
			m_acceptKey.clear();
			m_txOffset = 0;
			m_opcode = EWebSocketOpcode::Continuation;
			m_socketReady = false;
			m_upgradeSent = false;
			m_closing = false;
			CRawSocketTransport::close();
		}

		ETransportStatus CWebSocketClientTransport::connect(const STransportEndpoint& remote)
		{
			if (m_path.empty() || m_path[0] != '/' || m_path.find_first_of("\r\n ") != std::string::npos ||
				m_host.find_first_of("\r\n") != std::string::npos || m_origin.find_first_of("\r\n") != std::string::npos)
				return protocolError();
			m_authority = m_host.empty() ? remote.Address + ":" + std::to_string(remote.Port) : m_host;
			auto status = CRawSocketTransport::connect(remote);
			if (status == ETransportStatus::Done)
				m_state = ETransportState::Handshaking;
			return status == ETransportStatus::Done ? ETransportStatus::Pending : status;
		}

		STransportResult CWebSocketClientTransport::readSocket(void* buffer, std::size_t capacity)
		{
			return receiveBytes(buffer, capacity);
		}

		STransportResult CWebSocketClientTransport::writeSocket(const void* data, std::size_t size)
		{
			return sendBytes(data, size);
		}

		bool CWebSocketClientTransport::beginUpgrade()
		{
			unsigned char nonce[16];
			if (!randomBytes(nonce, 16))
			{
				protocolError();
				return false;
			}
			std::string key = CWebSocketUtil::base64(nonce, 16);
			m_acceptKey = CWebSocketUtil::acceptKey(key);
			std::string request = "GET " + m_path + " HTTP/1.1\r\nHost: " + m_authority +
				"\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " + key +
				"\r\nSec-WebSocket-Version: 13\r\n\r\n";
			if (!m_origin.empty())
				request.insert(request.size() - 2, "Origin: " + m_origin + "\r\n");
			if (request.size() > 16384)
			{
				protocolError();
				return false;
			}
			m_tx.assign(request.begin(), request.end());
			m_upgradeSent = true;
			return true;
		}

		bool CWebSocketClientTransport::validateUpgrade(const std::string& headers)
		{
			std::istringstream stream(headers);
			std::string line, version;
			int status = 0;
			std::getline(stream, line);
			std::istringstream first(line);
			first >> version >> status;
			if (version != "HTTP/1.1" || status != 101)
				return false;
			bool upgrade = false, connection = false, accept = false;
			while (std::getline(stream, line) && line != "\r")
			{
				auto colon = line.find(':');
				if (colon == std::string::npos)
					return false;
				std::string name = CWebSocketUtil::lower(CWebSocketUtil::trim(line.substr(0, colon))), value = CWebSocketUtil::trim(line.substr(colon + 1));
				if (name == "upgrade")
					upgrade = CWebSocketUtil::token(value, "websocket");
				else if (name == "connection")
					connection = CWebSocketUtil::token(value, "upgrade");
				else if (name == "sec-websocket-accept")
				{
					if (accept || value != m_acceptKey)
						return false;
					accept = true;
				}
				else if (name == "sec-websocket-extensions" || name == "sec-websocket-protocol")
					return false;
			}
			return upgrade && connection && accept;
		}

		ETransportStatus CWebSocketClientTransport::flush()
		{
			if (m_txOffset == m_tx.size())
				return ETransportStatus::Done;
			auto result = writeSocket(m_tx.data() + m_txOffset, m_tx.size() - m_txOffset);
			if (result.Status != ETransportStatus::Done)
				return result.Status;
			if (!result.Bytes || result.Bytes > m_tx.size() - m_txOffset)
				return protocolError();
			m_txOffset += result.Bytes;
			if (m_txOffset < m_tx.size())
				return ETransportStatus::Pending;
			m_tx.clear();
			m_txOffset = 0;
			return ETransportStatus::Done;
		}

		ETransportStatus CWebSocketClientTransport::readWire()
		{
			// Receive path: network -> m_rx (raw headers/payload) -> parseFrame() -> m_message
			// (decoded/assembled payload) -> caller's receive buffer -> OnReceived event.
			// Read and append to the end of m_rx, preserving buffered bytes. m_tx is for outgoing data.
			unsigned char buffer[8192];
			auto room = MaxMessageSize + 14 - m_rx.size();
			if (!room)
				return protocolError(1009);
			auto result = readSocket(buffer, (std::min)(room, sizeof(buffer)));
			if (result.Status == ETransportStatus::Done)
			{
				if (!result.Bytes || result.Bytes > (std::min)(room, sizeof(buffer)))
					return protocolError();
				m_rx.insert(m_rx.end(), buffer, buffer + result.Bytes);
			}
			return result.Status;
		}

		ETransportStatus CWebSocketClientTransport::update(std::size_t budget)
		{
			if (m_state == ETransportState::Closed)
				return ETransportStatus::Closed;
			if (m_state == ETransportState::Failed)
				return ETransportStatus::Error;
			if (!budget)
				return ETransportStatus::Pending;
			while (budget--)
			{
				if (m_state == ETransportState::Connecting)
				{
					auto status = CRawSocketTransport::update(1);
					if (status != ETransportStatus::Done)
						return status;
					m_state = ETransportState::Handshaking;
				}
				if (m_state == ETransportState::Handshaking)
				{
					if (!m_socketReady)
					{
						auto status = handshakeSocket();
						if (status != ETransportStatus::Done)
							return status;
						m_socketReady = true;
					}
					if (!m_upgradeSent && !beginUpgrade())
						return ETransportStatus::Error;
					auto status = flush();
					if (status == ETransportStatus::Error || status == ETransportStatus::Closed)
						return status;
					if (!m_tx.empty())
						return ETransportStatus::Pending;
					status = readWire();
					if (status == ETransportStatus::Error || status == ETransportStatus::Closed)
						return status;
					std::string response(m_rx.begin(), m_rx.end());
					auto end = response.find("\r\n\r\n");
					if (end == std::string::npos)
					{
						if (m_rx.size() > 16384)
							return protocolError();
						continue;
					}
					if (end + 4 > 16384 || !validateUpgrade(response.substr(0, end + 4)))
						return protocolError();
					m_rx.erase(m_rx.begin(), m_rx.begin() + end + 4);
					m_state = ETransportState::Ready;
				}
				if (m_state == ETransportState::Ready)
				{
					auto status = flush();
					if (status == ETransportStatus::Error || status == ETransportStatus::Closed)
						return status;
					if (m_closing && m_tx.empty())
					{
						close();
						return ETransportStatus::Closed;
					}
					return m_tx.empty() ? ETransportStatus::Done : ETransportStatus::Pending;
				}
			}
			return ETransportStatus::Pending;
		}

		STransportResult CWebSocketClientTransport::queueFrame(EWebSocketOpcode opcode, const void* data, std::size_t size)
		{
			// RFC 6455 section 5.1: Client -> Server MUST use MASK=1 with a fresh,
			// unpredictable 4-byte masking key per frame (including Ping/Pong/Close).
			// The wire payload is payload[i] XOR mask[i % 4]; the server reverses this XOR.
			// Server -> Client MUST use MASK=0, omit the masking key, and send the payload unchanged.
			// This client encoder therefore always sets MASK and appends the key before the masked payload.
			if ((!data && size) || size > MaxMessageSize)
				return protocolError(1009);
			if (!m_tx.empty())
				return ETransportStatus::WouldBlock;
			unsigned char mask[4];
			if (!randomBytes(mask, 4))
				return protocolError();
			m_tx.push_back(0x80 | static_cast<unsigned char>(opcode));
			m_tx.push_back(0x80 | (size < 126 ? static_cast<unsigned char>(size) : size <= 65535 ? 126 : 127));
			if (size >= 126)
				for (int i = size <= 65535 ? 1 : 7; i >= 0; --i)
					m_tx.push_back(static_cast<unsigned char>(static_cast<uint64_t>(size) >> (i * 8)));
			m_tx.insert(m_tx.end(), mask, mask + 4);
			const auto* payload = static_cast<const unsigned char*>(data);
			for (std::size_t i = 0; i < size; ++i)
				m_tx.push_back(payload[i] ^ mask[i % 4]);
			return { ETransportStatus::Done, size };
		}
		STransportResult CWebSocketClientTransport::send(const void* data, std::size_t size, bool binary)
		{
			if (m_state != ETransportState::Ready)
				return m_state == ETransportState::Failed ? ETransportStatus::Error
				: m_state == ETransportState::Closed ? ETransportStatus::Closed
				: ETransportStatus::Pending;
			if (m_closing)
				return ETransportStatus::Closed;
			if (!binary && data && size <= MaxMessageSize)
			{
				const auto* bytes = static_cast<const unsigned char*>(data);
				if (!CWebSocketUtil::isValidUTF8(bytes, size))
					return protocolError(1007);
			}
			return queueFrame(binary ? EWebSocketOpcode::Binary : EWebSocketOpcode::Text, data, size);
		}

		STransportResult CWebSocketClientTransport::parseFrame(void* buffer, std::size_t capacity)
		{
			// RFC 6455 section 5.2: bytes arrive in network byte order (big-endian).
			//
			//                  Byte 0                         Byte 1
			//       bit 7  6   5   4   3   2   1   0    7   6   5   4   3   2   1   0
			//          +---+---+---+---+---------------+---+---------------------------+
			//          |FIN|RS1|RS2|RS3| Opcode (4 bit) | M | Payload length (7 bit)    |
			//          +---+---+---+---+---------------+---+---------------------------+
			//          | Extended length: absent / 2 bytes / 8 bytes                  |
			//          +--------------------------------------------------------------+
			//          | Masking key: 4 bytes if M=1; absent if M=0                    |
			//          +--------------------------------------------------------------+
			//          | Payload: exactly the decoded payload-length number of bytes  |
			//          +--------------------------------------------------------------+
			//
			// FIN=1: final frame of a message. FIN=0: continuation frame(s) follow.
			// RS1..RS3: reserved; must be zero because no extensions are negotiated.
			// Opcode: Continuation=0x0, Text=0x1, Binary=0x2,
			//         Close=0x8, Ping=0x9, Pong=0xA (see EWebSocketOpcode).
			// Length marker 0..125: payload length is the marker; header = 2 bytes.
			// Length marker 126: read the next 2 bytes as length; header = 4 bytes.
			// Length marker 127: read the next 8 bytes as length; header = 10 bytes.
			// M=1 adds 4 bytes to the header; payload[i] ^= maskingKey[i % 4].
			// RFC 6455 section 5.1: Server -> Client MUST use M=0 with no masking key.
			// This client parser rejects M=1 and reads the server payload without unmasking.
			// Client -> Server MUST use M=1, including control frames; a server parser
			// must reject M=0, read the 4-byte key, and XOR the payload to unmask it.
			// queueFrame() always masks outgoing client frames with a fresh key.
			// Example: sending "Hello":
			// Client -> [Header + Mask key + masked payload] -> Server
			// Server -> [Header + payload "Hello"]          -> Client
			// Control frames require FIN=1 and length <=125. They are handled here;
			// only complete Text/Binary messages are delivered to the application.

			if (m_rx.size() < 2)
				return ETransportStatus::Pending;
			EWebSocketOpcode opcode = static_cast<EWebSocketOpcode>(m_rx[0] & 15);
			unsigned char marker = m_rx[1] & 127;
			bool fin = (m_rx[0] & 128) != 0, control = (m_rx[0] & 8) != 0;
			if ((m_rx[0] & 112) || (m_rx[1] & 128) ||
				(opcode != EWebSocketOpcode::Continuation && opcode != EWebSocketOpcode::Text &&
					opcode != EWebSocketOpcode::Binary && opcode != EWebSocketOpcode::Close &&
					opcode != EWebSocketOpcode::Ping && opcode != EWebSocketOpcode::Pong) ||
				(control && (!fin || marker > 125)))
				return protocolError();
			std::size_t header = marker < 126 ? 2 : marker == 126 ? 4 : 10;
			if (m_rx.size() < header)
				return ETransportStatus::Pending;
			uint64_t size = marker;
			if (marker >= 126)
			{
				size = 0;
				for (std::size_t i = 2; i < header; ++i)
					size = (size << 8) | m_rx[i];
				if ((marker == 126 && size < 126) || (marker == 127 && (size < 65536 || (m_rx[2] & 128))))
					return protocolError();
			}
			if (size > MaxMessageSize || (!control && size > MaxMessageSize - m_message.size()))
				return protocolError(1009);
			if (m_rx.size() - header < size)
				return ETransportStatus::Pending;
			const auto* payload = m_rx.data() + header;
			if (control)
			{
				if (opcode == EWebSocketOpcode::Close)
				{
					if (size == 1)
						return protocolError();
					if (size >= 2)
					{
						unsigned code = (payload[0] << 8) | payload[1];
						if (code < 1000 || code >= 5000 || code == 1004 || code == 1005 || code == 1006 ||
							code == 1015 || (code >= 1016 && code < 3000))
							return protocolError();
						if (!CWebSocketUtil::isValidUTF8(payload + 2, static_cast<std::size_t>(size) - 2))
							return protocolError(1007);
					}
				}
				if (opcode == EWebSocketOpcode::Close || opcode == EWebSocketOpcode::Ping)
				{
					auto result =
						queueFrame(opcode == EWebSocketOpcode::Ping ? EWebSocketOpcode::Pong : EWebSocketOpcode::Close,
							payload, static_cast<std::size_t>(size));
					if (result.Status != ETransportStatus::Done)
						return result.Status;
				}
				// Remove the processed frame header and payload; keep following bytes for the next parse.
				m_rx.erase(m_rx.begin(), m_rx.begin() + header + static_cast<std::size_t>(size));
				if (opcode == EWebSocketOpcode::Close)
					m_closing = true;
				return ETransportStatus::WouldBlock;
			}
			if ((opcode == EWebSocketOpcode::Continuation && m_opcode == EWebSocketOpcode::Continuation) ||
				(opcode != EWebSocketOpcode::Continuation && m_opcode != EWebSocketOpcode::Continuation))
				return protocolError();
			EWebSocketOpcode messageOpcode = opcode == EWebSocketOpcode::Continuation ? m_opcode : opcode;
			bool binary = messageOpcode == EWebSocketOpcode::Binary;
			std::size_t messageSize = m_message.size() + static_cast<std::size_t>(size);
			// Leave the final frame and fragment state untouched if the caller must retry.
			if (fin && capacity < messageSize)
				return { ETransportStatus::Truncated, messageSize, binary };

			m_opcode = messageOpcode;
			m_message.insert(m_message.end(), payload, payload + static_cast<std::size_t>(size));
			// Remove the processed frame header and payload; keep following bytes for the next parse.
			m_rx.erase(m_rx.begin(), m_rx.begin() + header + static_cast<std::size_t>(size));
			if (!fin)
				return ETransportStatus::WouldBlock;
			if (!binary && !CWebSocketUtil::isValidUTF8(m_message.data(), m_message.size()))
				return protocolError(1007);
			// Parsing succeeded: copy the complete decoded message from m_message into the caller's
			// receive buffer. CTCPSocket wraps this buffer in CMemoryStream and delivers OnReceived.
			if (messageSize)
				std::memcpy(buffer, m_message.data(), messageSize);
			m_message.clear();
			m_opcode = EWebSocketOpcode::Continuation;
			return { ETransportStatus::Done, messageSize, binary };
		}

		STransportResult CWebSocketClientTransport::receive(void* buffer, std::size_t capacity)
		{
			if (m_state != ETransportState::Ready)
				return m_state == ETransportState::Failed ? ETransportStatus::Error
				: m_state == ETransportState::Closed ? ETransportStatus::Closed
				: ETransportStatus::Pending;
			if (!buffer || !capacity)
				return setError(CSocketUtil::invalidArgument());
			if (m_closing)
				return ETransportStatus::Pending;
			// Try up to 32 parse/read steps. Exit early when the network has no data
			// (WouldBlock), a message is ready, the connection closes, or an error occurs.
			for (int i = 0; i < 32; ++i)
			{
				auto before = m_rx.size();
				auto result = parseFrame(buffer, capacity);
				if (result.Status != ETransportStatus::Pending && result.Status != ETransportStatus::WouldBlock)
					return result;
				if (m_closing)
					return ETransportStatus::Pending;
				if (m_rx.size() != before)
					continue;
				if (result.Status == ETransportStatus::WouldBlock)
					return result;

				// Read data from the network.
				auto status = readWire();
				if (status != ETransportStatus::Done)
					return status;
			}
			return ETransportStatus::Pending;
		}
	} // namespace Network
} // namespace Skylicht
