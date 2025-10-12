#include "pch.h"

#ifdef __EMSCRIPTEN__
#include "CEmscriptenHttpRequest.h"

#include <cstdlib>
#include <algorithm>

namespace Skylicht
{
	namespace Network
	{
		CEmscriptenHttpRequest::CEmscriptenHttpRequest(IHttpStream* stream) :
			IHttpRequest(stream),
			m_fetch(nullptr),
			m_isCancel(false),
			m_requestTimeOut(15000),
			m_totalBytePerSecond(0),
			m_bytePerSecond(0),
			m_requestTime(0),
			m_time(0),
			m_revcTime(0),
			m_currentTime(0)
		{

		}

		CEmscriptenHttpRequest::~CEmscriptenHttpRequest()
		{
			// If a fetch is in-flight, close it
			if (m_fetch)
			{
				emscripten_fetch_close(m_fetch);
				m_fetch = nullptr;
			}
		}

		// Main entry — dispatch based on type
		void CEmscriptenHttpRequest::sendRequest()
		{
			m_isCancel = false;
			m_httpCode = -1;
			m_downloading = 0;
			m_total = 0;

			m_requestTimeOut = 15000; // 15s, same default
			m_totalBytePerSecond = 0;
			m_bytePerSecond = 0;
			m_requestTime = m_time = m_revcTime = m_currentTime = os::Timer::getTime();

			switch (m_requestType)
			{
			case Get:       startFetch("GET", /*json*/false); break;
			case Post:      startFetch("POST", /*json*/false); break;
			case PostJson:  startFetch("POST", /*json*/true);  break;
			case Put:       startFetch("PUT",  /*json*/true);  break;
			case Delete:    startFetch("DELETE",/*json*/false); break;
			}
		}

		bool CEmscriptenHttpRequest::updateRequest()
		{
			// Return true if request is still in progress
			if (m_fetch == nullptr)
				return true;

			// tick current time
			m_currentTime = os::Timer::getTime();

			// update speed every ~1s
			if (m_currentTime - m_time > 1000)
			{
				m_bytePerSecond = m_totalBytePerSecond;
				m_totalBytePerSecond = 0;
				m_time = m_currentTime;
			}

			if (getCurrentTimeOut() > getTimeOut())
			{
				return true;
			}

			return false;
		}

		// Optional: allow manual cancel similar to CHttpRequest::cancel()
		void CEmscriptenHttpRequest::cancel()
		{
			if (m_fetch)
			{
				emscripten_fetch_close(m_fetch);
				m_fetch = nullptr;
				m_isCancel = true;
			}
		}

		// Build body for POST/PUT
		std::string CEmscriptenHttpRequest::buildBody(bool json)
		{
			if (json)
			{
				// Serialize { "k":"v" } from m_post (skip entries marked as File)
				std::string out = "{";
				bool first = true;
				for (const auto& f : m_post)
				{
					if (f.File) continue; // not supported here
					if (!first) out += ","; else first = false;
					out += "\"" + escapeJson(f.Name) + "\":\"" + escapeJson(f.Value) + "\"";
				}
				out += "}";
				return out;
			}
			else
			{
				// x-www-form-urlencoded for simple form fields (non-file)
				std::string out;
				for (size_t i = 0; i < m_post.size(); ++i)
				{
					const auto& f = m_post[i];
					if (f.File) continue; // TODO: multipart/form-data
					if (!out.empty()) out += '&';
					out += urlEncode(f.Name);
					out += '=';
					out += urlEncode(f.Value);
				}
				return out;
			}
		}

		std::string CEmscriptenHttpRequest::escapeJson(const std::string& s)
		{
			std::string o; o.reserve(s.size());
			for (char c : s)
			{
				switch (c)
				{
				case '\\': o += "\\\\"; break;
				case '"':  o += "\\\""; break;
				case '\n': o += "\\n"; break;
				case '\r': o += "\\r"; break;
				case '\t': o += "\\t"; break;
				default:    o += c; break;
				}
			}
			return o;
		}

		std::string CEmscriptenHttpRequest::urlEncode(const std::string& s)
		{
			// Minimal percent-encoder for web builds (covers space and a-zA-Z0-9-_.* ~)
			static auto isUnreserved = [](unsigned char ch) {
				return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' || ch == '.' || ch == '*' || ch == '~';
				};
			std::string out; out.reserve(s.size() * 3);
			for (unsigned char ch : s)
			{
				if (isUnreserved(ch)) out += (char)ch;
				else if (ch == ' ') out += '+'; // classic form encoding
				else {
					char buf[4];
					snprintf(buf, sizeof(buf), "%02X", ch);
					out += '%'; out += buf;
				}
			}
			return out;
		}

		void CEmscriptenHttpRequest::startFetch(const char* method, bool json)
		{
			// Prepare attributes
			emscripten_fetch_attr_t attr;
			emscripten_fetch_attr_init(&attr);
			strcpy(attr.requestMethod, method);
			attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
			attr.onsuccess = &CEmscriptenHttpRequest::OnSuccess;
			attr.onerror = &CEmscriptenHttpRequest::OnError;
			attr.onprogress = &CEmscriptenHttpRequest::OnProgress;
			attr.userData = this;

			// Build headers array (pairs terminated with 0)
			m_headerKV.clear();
			m_headerPtrs.clear();

			// If sending a body, set proper content-type default unless user supplied one
			std::string body;
			if (!strcmp(method, "POST") || !strcmp(method, "PUT"))
			{
				body = buildBody(json);
				if (!body.empty())
				{
					attr.requestData = body.c_str();
					attr.requestDataSize = (int)body.size();
				}

				bool hasCT = false;
				for (const auto& h : m_headers)
				{
					if (h.size() >= 13 && strncasecmp(h.c_str(), "Content-Type", 12) == 0)
					{
						hasCT = true; break;
					}
				}
				if (!hasCT)
				{
					m_headerKV.emplace_back("Content-Type");
					m_headerKV.emplace_back(json ? "application/json" : "application/x-www-form-urlencoded");
				}
			}

			// Copy user headers (split at the first ':')
			for (const auto& h : m_headers)
			{
				auto pos = h.find(':');
				if (pos == std::string::npos) continue;
				std::string k = h.substr(0, pos);
				std::string v = h.substr(pos + 1);
				// trim leading spaces
				v.erase(v.begin(), std::find_if(v.begin(), v.end(), [](unsigned char c) {return !std::isspace(c); }));
				m_headerKV.push_back(k);
				m_headerKV.push_back(v);
			}

			// Build pointer list
			for (auto& s : m_headerKV) m_headerPtrs.push_back(s.c_str());
			m_headerPtrs.push_back(nullptr);
			attr.requestHeaders = m_headerPtrs.data();

			// Fire!
			m_fetch = emscripten_fetch(&attr, m_url.c_str());
		}

		void CEmscriptenHttpRequest::OnSuccess(emscripten_fetch_t* fetch)
		{
			CEmscriptenHttpRequest* self = static_cast<CEmscriptenHttpRequest*>(fetch->userData);
			self->m_httpCode = fetch->status;
			if (self->m_dataStream && fetch->numBytes > 0 && fetch->data)
			{
				self->m_dataStream->write((void*)fetch->data, (unsigned int)fetch->numBytes);
				self->m_dataStream->endStream();
			}
			// Done with the request
			emscripten_fetch_close(fetch);
			self->m_fetch = nullptr;
		}

		void CEmscriptenHttpRequest::OnError(emscripten_fetch_t* fetch)
		{
			CEmscriptenHttpRequest* self = static_cast<CEmscriptenHttpRequest*>(fetch->userData);
			self->m_httpCode = -1;// fetch->status; // often 0 for network errors in the browser
			emscripten_fetch_close(fetch);
			self->m_fetch = nullptr;
		}

		void CEmscriptenHttpRequest::OnProgress(emscripten_fetch_t* fetch)
		{
			CEmscriptenHttpRequest* self = static_cast<CEmscriptenHttpRequest*>(fetch->userData);

			if (fetch->dataOffset >= 0)
			{
				unsigned long nowBytes = (unsigned long)fetch->dataOffset;
				unsigned long prev = self->m_downloading;

				if (nowBytes > prev)
					self->m_totalBytePerSecond += (nowBytes - prev);
				self->m_downloading = nowBytes;
			}

			if (fetch->totalBytes >= 0)
				self->m_total = (unsigned long)fetch->totalBytes;

			self->m_revcTime = self->m_currentTime = os::Timer::getTime();
		}
	}
}

#endif // __EMSCRIPTEN__
