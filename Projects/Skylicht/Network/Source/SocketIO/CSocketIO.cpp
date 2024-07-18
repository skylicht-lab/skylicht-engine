#include "pch.h"

#ifndef __EMSCRIPTEN__

#include "CSocketIO.h"
#include "HttpRequest/CHttpRequest.h"

#include "Utils/CStringImp.h"
#include "Utils/CDateTimeUtils.h"

int g_wsID = 0;

Skylicht::CSocketIO* g_currentIO = NULL;

typedef enum {
	IO_OPEN = '0', ///< Sent from the server when a new transport is opened (recheck)
	IO_CLOSE = '1', ///< Request the close of this transport but does not shutdown the connection itself.
	IO_PING = '2', ///< Sent by the client. Server should answer with a pong packet containing the same data
	IO_PONG = '3', ///< Sent by the server to respond to ping packets.
	IO_MESSAGE = '4', ///< actual message, client and server should call their callbacks with the data
	IO_UPGRADE = '5', ///< Before engine.io switches a transport, it tests, if server and client can communicate over this transport. If this test succeed, the client sends an upgrade packets which requests the server to flush its cache on the old transport and switch to the new transport.
	IO_NOOP = '6', ///< A noop packet. Used primarily to force a poll cycle when an incoming websocket connection is received.
} EIOEngineType;

typedef enum {
	IO_MSG_CONNECT = '0',
	IO_MSG_DISCONNECT = '1',
	IO_MSG_EVENT = '2',
	IO_MSG_ACK = '3',
	IO_MSG_ERROR = '4',
	IO_MSG_BINARY_EVENT = '5',
	IO_MSG_BINARY_ACK = '6',
} EIOMessageType;

void handle_ws_message(const std::string& message)
{
	if (g_currentIO == NULL)
		return;

	// std::string log = "[Websocket] raw message: ";
	// log += message;
	// os::Printer::log(log.c_str());

	if (message.length() > 1)
	{
		if (message == "3probe")
		{
			// https://github.com/socketio/socket.io-protocol
			// handshake socket.io 4 protocol

			// now send message connect to socket io
			g_currentIO->getWebSocket()->send("40");
		}
		else if (message[0] == IO_PING)
		{
			// receive ping (2)
			// send pong (3) to keep connection
			g_currentIO->getWebSocket()->send("3");	// IO_PONG
		}
		else if (message.length() > 2)
		{
			if (message[0] == IO_MESSAGE)
			{
				if (message[1] == IO_MSG_CONNECT)
				{
					// receive socket id
					g_currentIO->onConnected();

					// parse sid in 40{sid:"xxxxx"};
					char id[512];
					int pos = Skylicht::CStringImp::find<const char>(message.c_str(), ":\"");
					if (pos > 0)
					{
						if (Skylicht::CStringImp::getBlock(id, message.c_str(), "\"", pos + 1) > 0)
						{
							g_currentIO->setSocketID(id);
						}
					}
				}
				else if (message[1] == IO_MSG_DISCONNECT)
				{
					// receive disconnected
					g_currentIO->onDisconnected();
				}
				else if (message[1] == IO_MSG_EVENT)
				{
					// receive event
					std::string data = message.c_str() + 2;
					g_currentIO->onMessage(data);
				}
				else if (message[1] == IO_MSG_ACK)
				{
					// receive ask
					std::string data = message.c_str() + 2;
					char id[16];

					int pos = Skylicht::CStringImp::find<const char>(data.c_str(), '[');
					if (pos > 0)
					{
						Skylicht::CStringImp::mid(id, data.c_str(), 0, pos);

						data = data.c_str() + pos;
						g_currentIO->onMessageAsk(data, atoi(id));
					}
				}
			}
		}
	}
}

namespace Skylicht
{
	CSocketIO::CSocketIO(const char* url)
	{
		m_httpRequest = NULL;
		m_url = url;
		m_sendTimeout = 0.0f;
		m_ws = NULL;

		m_state = None;
		m_connected = false;

		m_bufferData = new char[2 * 1024 * 1024];
	}

	CSocketIO::~CSocketIO()
	{
		delete m_bufferData;
	}

	void CSocketIO::updateRequest()
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

						std::string wsURL = "ws://" + m_url + "/socket.io/?transport=websocket&EIO=4&sid=" + id;
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

				if (OnConnectFailed != nullptr)
					OnConnectFailed();
			}
			else
			{
				os::Printer::log("[Websocket] Can't connect WS");
				m_state = None;

				if (OnConnectFailed != nullptr)
					OnConnectFailed();
			}

			delete m_httpRequest;
			m_httpRequest = NULL;
		}
	}

	void CSocketIO::onConnected()
	{
		m_connected = true;
		if (OnConnected != nullptr)
			OnConnected();
	}

	void CSocketIO::onDisconnected()
	{
		m_connected = false;
		if (OnDisconnected != nullptr)
			OnDisconnected();
	}

	void CSocketIO::onMessage(const std::string& msg)
	{
		std::string log = "[Websocket] message: ";
		log += msg;
		os::Printer::log(log.c_str());

		if (OnMessage != nullptr)
			OnMessage(msg);
	}

	void CSocketIO::onMessageAsk(const std::string& msg, int id)
	{
		std::string log = "[Websocket] message: ";
		log += std::to_string(id);
		log += ":";
		log += msg;
		os::Printer::log(log.c_str());

		if (OnMessageAsk != nullptr)
			OnMessageAsk(msg, id);
	}

	void CSocketIO::update()
	{
		g_currentIO = this;

		if (m_state == RequestSocketIO)
		{
			updateRequest();
		}
		else if (m_state == UpdateSocket)
		{
			m_ws->poll(1);
			m_ws->dispatch(handle_ws_message);

			if (m_ws->getReadyState() == easywsclient::WebSocket::CLOSED)
			{
				os::Printer::log("[Websocket] closed");
				m_state = Closed;
				if (m_connected)
				{
					if (OnDisconnected != nullptr)
						OnDisconnected();
					m_connected = false;
				}
			}
		}

		g_currentIO = NULL;
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

		m_state = RequestSocketIO;
	}

	bool CSocketIO::startServices()
	{
		m_ws = easywsclient::WebSocket::from_url(m_wsURL.c_str(), m_wsOriginURL.c_str());
		if (m_ws)
		{
			m_ws->send("2probe");
			m_ws->send("5");
		}
		else
		{
			if (OnConnectFailed != nullptr)
				OnConnectFailed();
		}

		return true;
	}

	bool CSocketIO::isConnected()
	{
		return m_connected;
	}

	void CSocketIO::sendMessage(const char* message)
	{
		if (m_ws && m_connected)
			m_ws->send(message);
	}

	void CSocketIO::emit(const char* type, bool ack, int askID)
	{
		if (ack)
			sprintf(m_bufferData, "42/,%d[\"%s\"]", askID, type);
		else
			sprintf(m_bufferData, "42[\"%s\"]", type);
		sendMessage(m_bufferData);
	}

	void CSocketIO::emit(const char* type, const char* param, const char* value, bool ack, int askID)
	{
		if (ack)
			sprintf(m_bufferData, "42/,%d[\"%s\",{\"%s\":%s}]", askID, type, param, value);
		else
			sprintf(m_bufferData, "42[\"%s\",{\"%s\":%s}]", type, param, value);

		sendMessage(m_bufferData);
	}

	void CSocketIO::emit(const char* type, const char* param, const std::string& value, bool ack, int askID)
	{
		emit(type, param, value.c_str(), ack, askID);
	}

	void CSocketIO::emit(const char* type, std::map<std::string, std::string>& params, bool ack, int askID)
	{
		std::string textParams;

		int n = 0;

		std::map<std::string, std::string>::iterator i = params.begin(), end = params.end();
		while (i != end)
		{
			const std::string& p = (*i).first;
			const std::string& v = (*i).second;

			sprintf(m_bufferData, "\"%s\":%s", p.c_str(), v.c_str());

			if (n > 0)
				textParams += ",";

			textParams += m_bufferData;

			++i;
			n++;
		}

		if (ack)
			sprintf(m_bufferData, "42/,%d[\"%s\",{%s}]", askID, type, textParams.c_str());
		else
			sprintf(m_bufferData, "42[\"%s\",{%s}]", type, textParams.c_str());

		sendMessage(m_bufferData);
	}

	std::string CSocketIO::toStringParam(const char* s)
	{
		std::string value = "\"";
		value += s;
		value += "\"";

		return value;
	}

	std::string CSocketIO::toStringParam(const std::string& s)
	{
		std::string value = "\"";
		value += s;
		value += "\"";

		return value;
	}
}

#endif
