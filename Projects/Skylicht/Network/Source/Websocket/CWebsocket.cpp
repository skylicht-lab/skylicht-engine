#include "pch.h"
#include "CWebsocket.h"

#include "Utils/CStringImp.h"
#include "Utils/CDateTimeUtils.h"

int g_wsID = 0;
Skylicht::CWebsocket* g_currentWS = NULL;

void handle_ws_message(const std::string& message)
{
	if (g_currentWS != NULL)
		g_currentWS->onWebSocketMessage(message);
}

namespace Skylicht
{
	CWebsocket::CWebsocket(const char* url, IWebsocketCallback* callback)
	{
		m_httpRequest = NULL;
		m_websocket = NULL;
		m_url = url;
		m_callback = callback;

		m_sendTimeout = 0.0f;

		m_isConnecting = false;

		m_bufferData = new char[2 * 1024 * 1024];
	}

	CWebsocket::~CWebsocket()
	{
		if (m_websocket)
		{
			delete m_websocket;
			m_websocket = NULL;
		}

		delete m_bufferData;
	}

	void CWebsocket::update()
	{
		if (m_isConnecting)
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
						}
					}
				}
				else if (responseCode == 400)
				{
					os::Printer::log("[Websocket] Error 400");
				}
				else
				{
					os::Printer::log("[Websocket] Can't connect WS");
				}

				m_isConnecting = false;
				delete m_httpRequest;
				m_httpRequest = NULL;
			}
		}
		else
		{
			if (m_websocket != NULL && m_websocket->getReadyState() != easywsclient::WebSocket::CLOSED)
			{
				g_currentWS = this;

				// send data
				m_sendTimeout = m_sendTimeout - getTimeStep();

				if (m_sendTimeout < 0.0f)
				{
					if (m_sendData.size() > 0)
					{
						std::string& message = m_sendData.front();

						m_websocket->send(message);

						m_sendData.pop();
						m_sendTimeout = 50.0f;
					}
					else
						m_sendTimeout = 0.0f;
				}

				// check handle message
				m_websocket->poll(1);
				m_websocket->dispatch(handle_ws_message);

				g_currentWS = NULL;
			}
		}
	}

	void CWebsocket::forceSend()
	{
		if (m_websocket != NULL && m_websocket->getReadyState() != easywsclient::WebSocket::CLOSED)
		{
			g_currentWS = this;

			// send data
			if (m_sendData.size() > 0)
			{
				std::string& message = m_sendData.front();

				m_websocket->send(message);

				m_sendData.pop();
				m_sendTimeout = 500.0f;
			}
			else
				m_sendTimeout = 0.0f;

			// check handle message
			m_websocket->poll(0);
			m_websocket->dispatch(handle_ws_message);

			g_currentWS = NULL;
		}
	}

	void CWebsocket::init()
	{
		if (m_url.empty() == true)
			os::Printer::log("[Websocket] Can not init WS with empty url\n");

		// run http services	
		std::string httpURL = std::string("http://") + m_url + std::string("/socket.io/?transport=polling&EIO=4");

		m_httpRequest = new CHttpRequest(new CHttpStream());
		m_httpRequest->setURL(httpURL.c_str());
		m_httpRequest->sendRequestByGet();

		m_isConnecting = true;
	}

	bool CWebsocket::startServices()
	{
		m_websocket = easywsclient::WebSocket::from_url(m_wsURL, m_wsOriginURL);

		if (m_websocket == NULL)
		{
			return false;
		}

		return true;
	}

	bool CWebsocket::isConnected()
	{
		if (m_websocket == NULL || m_websocket->getReadyState() == easywsclient::WebSocket::CLOSED)
			return false;

		return true;
	}

	void CWebsocket::sendMessage(const char* message)
	{
		if (m_websocket)
			m_sendData.push(message);
	}

	void CWebsocket::emit(const char* type)
	{
		sprintf(m_bufferData, "5:::{\"name\":\"%s\",\"args\":[]}", type);

		sendMessage(m_bufferData);
	}

	void CWebsocket::emit(const char* type, const char* params, const char* value)
	{
		sprintf(m_bufferData, "5:::{\"name\":\"%s\",\"args\":[{\"%s\":\"%s\"}]}", type, params, value);

		sendMessage(m_bufferData);
	}

	void CWebsocket::emit(const char* type, std::map<std::string, std::string>& params)
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


		sprintf(m_bufferData, "5:::{\"name\":\"%s\",\"args\":[{%s}]}", type, textParams.c_str());

		sendMessage(m_bufferData);
	}

	void CWebsocket::onWebSocketMessage(const std::string& data)
	{
		printf("onWebSocketMessage: %s\n", data.c_str());

		if (data == "2::")
		{
			// heartbeat
			m_websocket->send("2::");
			return;
		}

		// delivery callback
		if (m_callback)
			m_callback->onWebSocketMessage(data);
	}
}