#include "pch.h"
#include "CWebsocket.h"

#ifndef __EMSCRIPTEN__

namespace Skylicht
{
	namespace Network
	{
		CWebsocket::CWebsocket() :
			m_ws(NULL)
		{

		}

		CWebsocket::~CWebsocket()
		{
			if (m_ws)
			{
				m_ws->close();
				delete m_ws;
				m_ws = NULL;
			}
		}

		bool CWebsocket::connect(const std::string& url, const std::string& origin)
		{
			m_ws = easywsclient::WebSocket::from_url(url.c_str(), origin.c_str());
			return m_ws != NULL;
		}

		void CWebsocket::send(const std::string& message)
		{
			if (m_ws)
				m_ws->send(message);
		}

		void CWebsocket::poll(int timeout)
		{
			if (m_ws)
				m_ws->poll(timeout);
		}

		void CWebsocket::dispatch(std::function<void(const std::string&)> callable)
		{
			if (m_ws)
				m_ws->dispatch(callable);
		}

		bool CWebsocket::isClosed()
		{
			if (m_ws)
				return m_ws->getReadyState() == easywsclient::WebSocket::CLOSED;
			return true;
		}
	}
}

#endif