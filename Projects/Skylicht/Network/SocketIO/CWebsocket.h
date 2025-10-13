#include "IWebsocket.h"

#ifndef __EMSCRIPTEN__

#include <easywsclient/easywsclient.hpp>

namespace Skylicht
{
	namespace Network
	{
		class CWebsocket : public IWebsocket
		{
		protected:
			easywsclient::WebSocket* m_ws;

		public:
			CWebsocket();

			virtual ~CWebsocket();

			virtual bool connect(const std::string& url, const std::string& origin);

			virtual void send(const std::string& message);

			virtual void poll(int timeout = 0);

			virtual void dispatch(std::function<void(const std::string&)> callable);

			virtual bool isClosed();
		};
	}
}

#endif