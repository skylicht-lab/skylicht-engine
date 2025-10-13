#include "pch.h"
#include "CEmscriptenWebsocket.h"

#ifdef __EMSCRIPTEN__

namespace Skylicht
{
	namespace Network
	{
		CEmscriptenWebsocket::CEmscriptenWebsocket() :
			m_ws(0),
			m_open(false),
			m_closed(true)
		{
			// emscripten_websocket_init(); // idempotent
		}

		CEmscriptenWebsocket::~CEmscriptenWebsocket()
		{
			if (m_ws)
			{
				// 1000 = Normal Closure
				emscripten_websocket_close(m_ws, 1000, "bye");
				emscripten_websocket_delete(m_ws);
				m_ws = 0;
			}
		}

		bool CEmscriptenWebsocket::connect(const std::string& url, const std::string& /*origin*/)
		{
			if (!emscripten_websocket_is_supported())
				return false;

			os::Printer::log("[Websocket] connecting to ");
			os::Printer::log(url.c_str());

			EmscriptenWebSocketCreateAttributes attr;
			emscripten_websocket_init_create_attributes(&attr);
			attr.url = url.c_str();
			attr.protocols = nullptr;
			attr.createOnMainThread = EM_TRUE;

			m_ws = emscripten_websocket_new(&attr);
			if (!m_ws)
				return false;

			m_open = false;
			m_closed = false;
			m_messages.clear();

			emscripten_websocket_set_onopen_callback(m_ws, this, &CEmscriptenWebsocket::onOpen);
			emscripten_websocket_set_onerror_callback(m_ws, this, &CEmscriptenWebsocket::onError);
			emscripten_websocket_set_onclose_callback(m_ws, this, &CEmscriptenWebsocket::onClose);
			emscripten_websocket_set_onmessage_callback(m_ws, this, &CEmscriptenWebsocket::onMessage);

			return true;
		}

		void CEmscriptenWebsocket::send(const std::string& message)
		{
			if (m_ws && m_open && !m_closed)
			{
				os::Printer::log("[Websocket] send: ");
				os::Printer::log(message.c_str());
				emscripten_websocket_send_utf8_text(m_ws, message.c_str());
			}
		}

		void CEmscriptenWebsocket::poll(int timeout)
		{

		}

		void CEmscriptenWebsocket::dispatch(std::function<void(const std::string&)> callable)
		{
			if (!callable)
				return;

			while (!m_messages.empty())
			{
				std::string msg = std::move(m_messages.back());

				m_messages.pop_back();
				callable(msg);
			}
		}

		bool CEmscriptenWebsocket::isClosed()
		{
			return m_closed;
		}

		// ==== Callbacks (static) ====

		EM_BOOL CEmscriptenWebsocket::onOpen(int, const EmscriptenWebSocketOpenEvent*, void* userData)
		{
			os::Printer::log("[Websocket] connected");
			auto* self = reinterpret_cast<CEmscriptenWebsocket*>(userData);
			self->m_open = true;
			self->m_closed = false;
			if (self->OnConnected != nullptr)
				self->OnConnected();
			return EM_TRUE;
		}

		EM_BOOL CEmscriptenWebsocket::onError(int, const EmscriptenWebSocketErrorEvent*, void* userData)
		{
			os::Printer::log("[Websocket] error");
			auto* self = reinterpret_cast<CEmscriptenWebsocket*>(userData);
			self->m_closed = true;
			self->m_open = false;
			if (self->OnConnectFailed != nullptr)
				self->OnConnectFailed();
			return EM_TRUE;
		}

		EM_BOOL CEmscriptenWebsocket::onClose(int, const EmscriptenWebSocketCloseEvent*, void* userData)
		{
			os::Printer::log("[Websocket] closed");
			auto* self = reinterpret_cast<CEmscriptenWebsocket*>(userData);
			self->m_closed = true;
			self->m_open = false;
			return EM_TRUE;
		}

		EM_BOOL CEmscriptenWebsocket::onMessage(int, const EmscriptenWebSocketMessageEvent* e, void* userData)
		{
			auto* self = reinterpret_cast<CEmscriptenWebsocket*>(userData);
			if (!self)
				return EM_TRUE;

			if (e->numBytes > 0 && e->data)
			{
				const char* data = reinterpret_cast<const char*>(e->data);
				self->m_messages.emplace_back(std::string(data));
			}
			return EM_TRUE;
		}
	}
}

#endif // __EMSCRIPTEN__