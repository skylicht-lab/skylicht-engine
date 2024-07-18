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

#include "curl/curl.h"
#include "CHttpStream.h"

// 100kb buffer download
#define DOWNLOADBUFFER_SIZE 102400
#define HASHSTRING_SIZE	35

namespace Skylicht
{
	class CHttpRequest
	{
	public:
		enum ERequestType
		{
			Get = 0,
			Post,
			PostJson,
			Put,
			Delete
		};
	protected:
		// curl handle
		CURL* m_curl;
		CURLM* m_multiHandle;

		void* m_formpost;
		void* m_lastptr;

		// total size download
		unsigned long m_downloading;
		unsigned long m_total;
		int m_needContinue;

		int m_httpCode;

		std::string m_url;
		bool m_sendRequest;

		ERequestType m_requestType;

		IHttpStream* m_dataStream;
		int m_requestID;

		bool m_cancel;
		bool m_isTimeOut;

		unsigned char* m_downloadBuffer;
		unsigned long m_sizeBuffer;

		void* m_md5Context;
		char m_hashString[HASHSTRING_SIZE];

		unsigned long m_requestTime;
		unsigned long m_time;
		unsigned long m_revcTime;
		unsigned long m_currentTime;
		unsigned long m_requestTimeOut;

		unsigned long m_totalBytePerSecond;
		unsigned long m_bytePerSecond;

		std::string m_postField;

		std::string m_sessionFile;

		struct SForm
		{
			std::string	Name;
			std::string Value;
			bool File;

			SForm()
			{
				File = false;
			}
		};

		std::vector<SForm> m_post;

		curl_slist* m_headerlist;

		void* m_userData;

		std::vector<std::string> m_headers;

		bool m_checkTimeout;

	public:
		CHttpRequest(IHttpStream* stream);
		virtual ~CHttpRequest();

		static void globalInit();
		
		static void globalFree();

		static std::string urlEncode(const std::string& s);

		inline void setURL(const char* url)
		{
			m_url = url;
		}

		inline const std::string& getURL()
		{
			return m_url;
		}

		inline void setSessionFile(const char* session)
		{
			m_sessionFile = session;
		}

		void addFormRequest(const char* name, const char* value);
		void addFormFileRequest(const char* name, const char* value);

		void sendRequestByPost();
		void sendRequestByPostJson();
		void sendRequestByGet();
		void sendRequestByPut();
		void sendRequestByDelete();

		void sendRequest();

		inline bool isSendRequest()
		{
			return m_sendRequest;
		}

		inline void setSendRequestType(ERequestType type)
		{
			m_requestType = type;
		}

		bool updateRequest();

		bool checkTimeOut();

		inline bool isTimeOut()
		{
			return m_isTimeOut;
		}

		inline unsigned long getRequestTime()
		{
			return m_currentTime - m_requestTime;
		}

		inline void setUserData(void* data)
		{
			m_userData = data;
		}

		inline void* getUserData()
		{
			return m_userData;
		}

		inline void clearHeader()
		{
			m_headers.clear();
		}

		inline void addHeader(const char* header)
		{
			m_headers.push_back(header);
		}

		void onRevcData(unsigned char* lpData, unsigned long size, unsigned long num);

		void onReadData(unsigned char* lpData, unsigned long size, unsigned long num);

		inline unsigned long getByteDownload()
		{
			return m_downloading;
		}

		inline unsigned long getSpeedDownload()
		{
			return m_bytePerSecond;
		}

		inline void updateStatusDownload(unsigned long downLoad, unsigned long total)
		{
			m_downloading = downLoad;
			m_total = total;
		}

		inline int getResponseCode()
		{
			return m_httpCode;
		}

		inline IHttpStream* getStream()
		{
			return m_dataStream;
		}

		inline void setRequestID(int id)
		{
			m_requestID = id;
		}

		inline int getRequestID()
		{
			return m_requestID;
		}

		inline void cancel()
		{
			m_cancel = true;
		}

		inline bool isCancel()
		{
			return m_cancel;
		}

		inline const char* getCurrentHashString()
		{
			return m_hashString;
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

#endif
