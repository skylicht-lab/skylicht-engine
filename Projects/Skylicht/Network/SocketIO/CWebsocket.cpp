#include "pch.h"
#include "CWebsocket.h"

#ifndef __EMSCRIPTEN__

#include "Socket/CWebSocketClientTransport.h"
#include "Socket/CWebSocketUtil.h"
#include "Utils/CMemoryStream.h"

namespace Skylicht
{
	namespace Network
	{

		CWebsocket::CWebsocket() : m_messageBytes(0), m_revision(0), m_closed(true), m_connecting(false)
		{
			m_client.OnConnected = [this](CConnection*) {
				m_connecting = false;
				auto callback = OnConnected;
				if (callback)
					callback();
				};
			m_client.OnDisconnected = [this](CConnection*, CBaseSocket::EDisconnectReason) { m_closed = true; };
			m_client.OnReceived = [this](CConnection*, CMemoryStream* packet) {
				// This Socket.IO adapter dispatches Text messages only; binary attachments are unsupported.
				if (packet->isBinary())
					return;

				// Bound the queue when the caller polls without dispatching.
				if (m_messages.size() >= 1024 ||
					packet->getSize() > CWebSocketClientTransport::MaxMessageSize - m_messageBytes)
				{
					m_client.close();
					m_closed = true;
					return;
				}
				m_messages.emplace_back(reinterpret_cast<const char*>(packet->getData()), packet->getSize());
				m_messageBytes += packet->getSize();
				};
		}

		CWebsocket::~CWebsocket()
		{
			m_client.OnConnected = nullptr;
			m_client.OnDisconnected = nullptr;
			m_client.OnReceived = nullptr;
			m_client.close();
		}

		void CWebsocket::failConnect()
		{
			m_closed = true;
			if (!m_connecting)
				return;
			m_connecting = false;
			auto callback = OnConnectFailed;
			if (callback)
				callback();
		}

		bool CWebsocket::connect(const std::string& url, const std::string& origin)
		{
			m_connecting = false;
			m_client.close();
			++m_revision;
			m_messages.clear();
			m_messageBytes = 0;
			m_closed = false;
			m_connecting = true;
			std::string host, authority, path;
			irr::u16 port;
			STransportEndpoint endpoint;
			if (origin.find_first_of("\r\n") != std::string::npos || !CWebSocketUtil::parseURL(url, host, authority, path, port) ||
				!CWebSocketUtil::resolveIPv4(host, port, endpoint))
			{
				failConnect();
				return false;
			}
			auto transportFactory = [path, authority, origin](ETransportMode) -> ITransport*
				{
					return new CWebSocketClientTransport(path, authority, origin);
				};

			if (!m_client.setTransportFactory(transportFactory))
			{
				m_client.close();
				failConnect();
				return false;
			}

			if (!m_client.open(endpoint.Address.c_str(), endpoint.Port))
			{
				m_client.close();
				failConnect();
				return false;
			}
			return true;
		}

		void CWebsocket::send(const std::string& message)
		{
			if (m_closed)
				return;
			if (message.size() <= CWebSocketClientTransport::MaxMessageSize)
			{
				CMemoryStream packet(static_cast<unsigned int>(message.size()));
				packet.setBinary(false); // Socket.IO packets are UTF-8 WebSocket Text messages.
				if (!message.empty())
					packet.writeData(message.data(), static_cast<unsigned int>(message.size()));
				if (m_client.send(packet))
					return;
			}
			m_client.close();
			m_closed = true;
		}

		void CWebsocket::poll(int /*timeout*/)
		{
			// Ignore timeout: CTCPClient progresses non-blocking I/O without waiting.
			if (m_closed)
			{
				if (m_connecting)
					failConnect();
				return;
			}
			m_client.update();
			if (m_closed && m_connecting)
				failConnect();
		}

		void CWebsocket::dispatch(std::function<void(const std::string&)> callable)
		{
			if (!callable)
				return;
			auto revision = m_revision;
			std::size_t count = m_messages.size();
			while (count-- && !m_messages.empty())
			{
				std::string message = std::move(m_messages.front());
				m_messages.pop_front();
				m_messageBytes -= message.size();
				callable(message);
				if (revision != m_revision)
					return;
			}
		}

		bool CWebsocket::isClosed()
		{
			return m_closed;
		}
	} // namespace Network
} // namespace Skylicht

#endif
