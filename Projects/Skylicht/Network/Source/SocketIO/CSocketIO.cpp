#include "pch.h"
#include "CSocketIO.h"

#include "Utils/CStringImp.h"
#include "Utils/CDateTimeUtils.h"

int g_wsID = 0;
Skylicht::CSocketIO* g_currentIO = NULL;

void handle_ws_message(const std::string& message)
{

}

namespace Skylicht
{
	CSocketIO::CSocketIO(const char* url)
	{
		m_httpRequest = NULL;
		m_url = url;
		m_sendTimeout = 0.0f;

		m_state = None;

		m_bufferData = new char[2 * 1024 * 1024];
	}

	CSocketIO::~CSocketIO()
	{
		delete m_bufferData;
	}

	void CSocketIO::updateRequest1()
	{
		bool finish = m_httpRequest->updateRequest();
		if (finish)
		{
			int responseCode = m_httpRequest->getResponseCode();
			IHttpStream* stream = m_httpRequest->getStream();

			const char* responseData = NULL;
			int responseSize = 0;

			if (stream)
			{
				responseData = (const char*)stream->getData();
				responseSize = (int)stream->getDataSize();
			}

			char log[512];
			sprintf(log, "[Websocket] response: %s", responseData);
			os::Printer::log(log);

			std::string sessionID;

			if (responseCode == 200)
			{
				const char* sid = "\"sid\":\"";
				int pos = CStringImp::find<const char>(responseData, sid);
				if (pos > 0)
				{
					pos = pos + (int)strlen(sid);

					int posEnd = CStringImp::find<const char>(responseData + pos, "\"");
					if (posEnd > 0)
					{
						char id[64];
						CStringImp::mid<char, const char>(id, responseData, pos, pos + posEnd);

						std::string wsURL = "ws://" + m_url + "/socket.io/?transport=polling&EIO=4&sid=" + id;
						std::string httpURL = "http://" + m_url;

						setWSURL(wsURL.c_str());
						setOriginURL(httpURL.c_str());
						startServices();

						m_state = UpdateSocket;
					}
				}
			}
			else if (responseCode == 400)
			{
				os::Printer::log("[Websocket] Error 400");
				m_state = None;
			}
			else
			{
				os::Printer::log("[Websocket] Can't connect WS");
				m_state = None;
			}

			delete m_httpRequest;
			m_httpRequest = NULL;
		}
	}

	void CSocketIO::update()
	{
		if (m_state == Request1)
		{
			updateRequest1();
		}
		else if (m_state == UpdateSocket)
		{

		}
	}

	void CSocketIO::forceSend()
	{

	}

	void CSocketIO::init()
	{
		if (m_url.empty() == true)
			os::Printer::log("[Websocket] Can not init WS with empty url\n");

		// run http services	
		std::string httpURL = std::string("http://") + m_url + std::string("/socket.io/?transport=polling&EIO=4");

		m_httpRequest = new CHttpRequest(new CHttpStream());
		m_httpRequest->setURL(httpURL.c_str());
		m_httpRequest->sendRequestByGet();

		m_state = Request1;
	}

	bool CSocketIO::startServices()
	{
		return true;
	}

	bool CSocketIO::isConnected()
	{
		return true;
	}

	void CSocketIO::sendMessage(const char* message)
	{

	}

	void CSocketIO::emit(const char* type)
	{

	}

	void CSocketIO::emit(const char* type, const char* params, const char* value)
	{

	}

	void CSocketIO::emit(const char* type, std::map<std::string, std::string>& params)
	{
		std::string textParams;

		int n = 0;

		std::map<std::string, std::string>::iterator i = params.begin(), end = params.end();
		while (i != end)
		{
			const std::string& p = (*i).first;
			const std::string& v = (*i).second;

			sprintf(m_bufferData, "\"%s\":\"%s\"", p.c_str(), v.c_str());

			if (n > 0)
				textParams += ",";

			textParams += m_bufferData;

			++i;
			n++;
		}

	}
}