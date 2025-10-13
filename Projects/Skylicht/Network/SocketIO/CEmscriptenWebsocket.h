#pragma once
#include "IWebsocket.h"

#ifdef __EMSCRIPTEN__

#include <vector>
#include <string>
#include <functional>
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>

namespace Skylicht
{
	namespace Network
	{
		class CEmscriptenWebsocket : public IWebsocket
		{
		public:
			CEmscriptenWebsocket();
			virtual ~CEmscriptenWebsocket();

			// IWebsocket
			virtual bool connect(const std::string& url, const std::string& origin);
			virtual void send(const std::string& message);
			virtual void poll(int timeout = 0);
			virtual void dispatch(std::function<void(const std::string&)> callable);
			virtual bool isClosed();

		private:
			static EM_BOOL onOpen(int, const EmscriptenWebSocketOpenEvent* e, void* userData);
			static EM_BOOL onError(int, const EmscriptenWebSocketErrorEvent* e, void* userData);
			static EM_BOOL onClose(int, const EmscriptenWebSocketCloseEvent* e, void* userData);
			static EM_BOOL onMessage(int, const EmscriptenWebSocketMessageEvent* e, void* userData);

		private:
			EMSCRIPTEN_WEBSOCKET_T m_ws;
			bool m_open;
			bool m_closed;
			std::vector<std::string> m_messages;
		};
	}
}

#endif // __EMSCRIPTEN__