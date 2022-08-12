#pragma once

#include "HttpRequest/CHttpRequest.h"
#include "easywsclient.hpp"

namespace Skylicht
{
	class CSocketIO
	{
	public:
		enum EConnectState
		{
			None,
			RequestSocketIO,
			UpdateSocket,
			Closed
		};

	protected:
		std::string m_url;
		std::string m_wsURL;
		std::string m_wsOriginURL;

		CHttpRequest* m_httpRequest;

		std::queue<std::string> m_sendData;
		float m_sendTimeout;

		EConnectState m_state;

		char* m_bufferData;

		easywsclient::WebSocket* m_ws;

		bool m_connected;

	public:

	public:
		CSocketIO(const char* url);

		virtual ~CSocketIO();

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

		void sendMessage(const char* message);

		void emit(const char* type, bool ack, int askID = 1);
		void emit(const char* type, const char* param, const char* value, bool ack, int askID = 1);
		void emit(const char* type, std::map<std::string, std::string>& params, bool ack, int askID = 1);

		bool isConnected();

		void onConnected();
		void onDisconnected();
		void onMessage(const std::string& msg);
		void onMessageAsk(const std::string& msg, int id);

		void updateRequest();

		void close();

		inline easywsclient::WebSocket* getWebSocket()
		{
			return m_ws;
		}
	};

};