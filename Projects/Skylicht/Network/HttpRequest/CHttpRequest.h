/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

#include "pch.h"

#ifndef __EMSCRIPTEN__

#include "IHttpRequest.h"

#include "curl/curl.h"
#include "CHttpStream.h"

// 100kb buffer download
#define DOWNLOADBUFFER_SIZE 102400

namespace Skylicht
{
	namespace Network
	{
		class CHttpRequest : public IHttpRequest
		{
		protected:
			// curl handle
			CURL* m_curl;
			CURLM* m_multiHandle;

			void* m_formpost;
			void* m_lastptr;

			int m_needContinue;

			bool m_sendRequest;

			bool m_cancel;
			bool m_isTimeOut;

			unsigned char* m_downloadBuffer;
			unsigned long m_sizeBuffer;

			unsigned long m_requestTime;
			unsigned long m_time;
			unsigned long m_revcTime;
			unsigned long m_currentTime;
			unsigned long m_requestTimeOut;

			unsigned long m_totalBytePerSecond;
			unsigned long m_bytePerSecond;

			std::string m_postField;
			std::string m_sessionFile;

			curl_slist* m_headerlist;

			bool m_checkTimeout;

		public:
			CHttpRequest(IHttpStream* stream);

			virtual ~CHttpRequest();

			static void globalInit();

			static void globalFree();

			static std::string urlEncode(const std::string& s);

			inline void setSessionFile(const char* session)
			{
				m_sessionFile = session;
			}

			void sendRequestByPost();
			void sendRequestByPostJson();
			void sendRequestByGet();
			void sendRequestByPut();
			void sendRequestByDelete();

			virtual void sendRequest();

			virtual bool updateRequest();

			inline bool isSendRequest()
			{
				return m_sendRequest;
			}

			bool checkTimeOut();

			virtual bool isTimeOut()
			{
				return m_isTimeOut;
			}

			inline unsigned long getRequestTime()
			{
				return m_currentTime - m_requestTime;
			}

			void onRevcData(unsigned char* lpData, unsigned long size, unsigned long num);

			void onReadData(unsigned char* lpData, unsigned long size, unsigned long num);

			virtual unsigned long getSpeedDownload()
			{
				return m_bytePerSecond;
			}

			virtual void cancel()
			{
				m_cancel = true;
			}

			virtual bool isCancel()
			{
				return m_cancel;
			}

			inline long getCurrentTimeOut()
			{
				return m_currentTime - m_revcTime;
			}

			inline long getTimeOut()
			{
				return m_requestTimeOut;
			}
		};
	}
}

#endif
