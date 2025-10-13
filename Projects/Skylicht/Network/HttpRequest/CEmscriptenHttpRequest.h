// CEmscriptenHttpRequest.h / .cpp
// Backend: emscripten/fetch.h
// Drop-in replacement implementing Skylicht::Network::IHttpRequest for Web builds
// Notes:
//  - Supports GET, POST (x-www-form-urlencoded), POST JSON, PUT JSON, DELETE
//  - Streams response data into provided IHttpStream (same as CHttpRequest)
//  - Sets HTTP status code via getResponseCode()
//  - Header support using emscripten_fetch "requestHeaders" (pairs: key, value, ..., 0)
//  - File uploads via multipart are not implemented in this minimal version (see TODO)
//  - Requires building with Emscripten and -sFETCH=1

#pragma once

#ifdef __EMSCRIPTEN__

#include <emscripten/fetch.h>
#include "IHttpRequest.h"

namespace Skylicht
{
	namespace Network
	{
		class CEmscriptenHttpRequest : public IHttpRequest
		{
		public:
			CEmscriptenHttpRequest(IHttpStream* stream);

			~CEmscriptenHttpRequest();

			virtual void sendRequest();

			virtual bool updateRequest();

			virtual void cancel();

			virtual bool isCancel()
			{
				return m_isCancel;
			}

			virtual unsigned long getSpeedDownload()
			{
				return m_bytePerSecond;
			}

			virtual bool isTimeOut()
			{
				return m_isTimeOut;
			}

		private:

			std::string buildBody(bool json);

			static std::string escapeJson(const std::string& s);

			static std::string urlEncode(const std::string& s);

			void startFetch(const char* method, bool json);

			static void OnSuccess(emscripten_fetch_t* fetch);

			static void OnError(emscripten_fetch_t* fetch);

			static void OnProgress(emscripten_fetch_t* fetch);

		protected:

			inline long getCurrentTimeOut()
			{
				return m_currentTime - m_revcTime;
			}

			inline long getTimeOut()
			{
				return m_requestTimeOut;
			}

		private:
			bool m_isCancel;
			bool m_isTimeOut;

			unsigned long m_requestTime;
			unsigned long m_time;
			unsigned long m_revcTime;
			unsigned long m_currentTime;
			unsigned long m_requestTimeOut;

			unsigned long m_totalBytePerSecond;
			unsigned long m_bytePerSecond;

			emscripten_fetch_t* m_fetch;
			std::vector<std::string> m_headerKV;      // key, value, key, value ...
			std::vector<const char*> m_headerPtrs;    // pointers for attr.requestHeaders
		};
	}
}

#endif // __EMSCRIPTEN__
