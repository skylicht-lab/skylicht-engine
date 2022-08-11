#pragma once

#include "HttpRequest/CHttpRequest.h"

namespace Skylicht
{
	class CSocketIO
	{
	public:
		enum EConnectState
		{
			None,
			Request1,
			UpdateSocket,
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

		void emit(const char* type);
		void emit(const char* type, const char* params, const char* value);
		void emit(const char* type, std::map<std::string, std::string>& params);

		bool isConnected();

		void updateRequest1();
	};

};