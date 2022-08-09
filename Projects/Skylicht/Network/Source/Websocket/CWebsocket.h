#ifndef _WEBSOCKET_H_
#define _WEBSOCKET_H_

#include "easywsclient.hpp"
#include "HttpRequest/CHttpRequest.h"

namespace Skylicht
{
	class IWebsocketCallback
	{
	public:
		virtual void onWebSocketMessage(const std::string& data)
		{
		}
	};

	class CWebsocket
	{
	protected:
		std::string m_url;
		std::string m_wsURL;
		std::string m_wsOriginURL;

		CHttpRequest* m_httpRequest;
		easywsclient::WebSocket* m_websocket;

		IWebsocketCallback* m_callback;

		std::queue<std::string> m_sendData;
		float m_sendTimeout;

		bool m_isConnecting;

		char* m_bufferData;

	public:
		CWebsocket(const char* url, IWebsocketCallback* callback);

		virtual ~CWebsocket();

		void update();

		void forceSend();

		void init();

		bool startServices();

		void setWSURL(const char* ws)
		{
			m_wsURL = ws;
		}

		void setOriginURL(const char* ws)
		{
			m_wsOriginURL = ws;
		}

		void setCallback(IWebsocketCallback* cb)
		{
			m_callback = cb;
		}

		void sendMessage(const char* message);

		void emit(const char* type);
		void emit(const char* type, const char* params, const char* value);
		void emit(const char* type, std::map<std::string, std::string>& params);

		virtual void onWebSocketMessage(const std::string& data);

		bool isConnected();

		bool isConnecting()
		{
			return m_isConnecting;
		}
	};

};

#endif