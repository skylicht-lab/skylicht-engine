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

#include "pch.h"
#include "Crypto/md5.h"
#include "CMD5.h"
#include "CHttpRequest.h"

#ifndef __EMSCRIPTEN__

namespace Skylicht
{
	size_t httpWriteFunc(void* data, size_t n, size_t sizeData, void* obj)
	{
		// inc data revc
		CHttpRequest* pHttp = (CHttpRequest*)obj;
		pHttp->onRevcData((unsigned char*)data, (unsigned long)sizeData, (unsigned long)n);
		return sizeData * n;
	}

	size_t httpReadFunc(void* data, size_t n, size_t sizeData, void* obj)
	{
		CHttpRequest* pHttp = (CHttpRequest*)obj;
		pHttp->onReadData((unsigned char*)data, (unsigned long)sizeData, (unsigned long)n);
		return sizeData * n;
	}

	int httpProgressFunc(void* obj,
		double t, /* dltotal */
		double d, /* dlnow */
		double ultotal,
		double ulnow)
	{
		CHttpRequest* pHttp = (CHttpRequest*)obj;
		pHttp->updateStatusDownload((unsigned long)d, (unsigned long)t);

		// cancel
		if (pHttp->isCancel() == true)
			return -1;

		return 0;
	}


	void CHttpRequest::globalInit()
	{
		curl_global_init(CURL_GLOBAL_ALL);
	}

	void CHttpRequest::globalFree()
	{
		curl_global_cleanup();
	}

	std::string CHttpRequest::urlEncode(const std::string& s)
	{
		CURL* url = curl_easy_init();

		char* output = curl_easy_escape(url, s.c_str(), (int)s.size());
		std::string ret = output;

		curl_free(output);
		curl_easy_cleanup(url);

		return ret;
	}

	CHttpRequest::CHttpRequest(IHttpStream* stream)
	{
		m_curl = curl_easy_init();

		// http request function
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, httpWriteFunc);
		curl_easy_setopt(m_curl, CURLOPT_READFUNCTION, httpReadFunc);
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);
		curl_easy_setopt(m_curl, CURLOPT_READDATA, this);
		curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(m_curl, CURLOPT_PROGRESSFUNCTION, httpProgressFunc);
		curl_easy_setopt(m_curl, CURLOPT_PROGRESSDATA, this);
		curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1);

#ifdef WIN32
		//curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1);
#endif

		// https
		curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, 0);

		// firefox agent
		curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");

		m_dataStream = stream;

		m_multiHandle = curl_multi_init();
		curl_multi_add_handle(m_multiHandle, m_curl);

		m_cancel = false;
		m_sendRequest = false;
		m_requestType = Get;
		m_isTimeOut = false;
		m_checkTimeout = true;

		m_downloadBuffer = new unsigned char[DOWNLOADBUFFER_SIZE];
		m_sizeBuffer = 0;
		m_requestID = -1;

		m_formpost = NULL;
		m_lastptr = NULL;

		m_userData = NULL;

		m_headerlist = NULL;

		// timeout is 15s
		m_requestTimeOut = 15000;

		m_downloading = 0;
		m_total = 0;

		m_md5Context = new MD5_CTX();

		memset(m_hashString, 0, 32);
	}

	CHttpRequest::~CHttpRequest()
	{
		if (m_downloadBuffer)
			delete m_downloadBuffer;

		if (m_formpost)
		{
			curl_formfree((curl_httppost*)m_formpost);
			m_formpost = NULL;
			m_lastptr = NULL;
		}

		if (m_dataStream)
			delete m_dataStream;

		if (m_headerlist)
			curl_slist_free_all(m_headerlist);

		if (m_multiHandle != NULL)
		{
			curl_multi_remove_handle(m_multiHandle, m_curl);
			curl_multi_cleanup(m_multiHandle);
		}

		curl_easy_cleanup(m_curl);

		delete m_md5Context;
	}

	void CHttpRequest::addFormRequest(const char* name, const char* value)
	{
		m_post.push_back(SForm());
		SForm& form = m_post.back();

		form.Name = name;
		form.Value = value;
		form.File = false;
	}

	void CHttpRequest::addFormFileRequest(const char* name, const char* value)
	{
		m_post.push_back(SForm());
		SForm& form = m_post.back();

		form.Name = name;
		form.Value = value;
		form.File = true;
	}

	void CHttpRequest::sendRequest()
	{
		if (m_requestType == Post)
			sendRequestByPost();
		else if (m_requestType == PostJson)
			sendRequestByPostJson();
		else if (m_requestType == Put)
			sendRequestByPut();
		else if (m_requestType == Delete)
			sendRequestByDelete();
		else
			sendRequestByGet();
	}

	void CHttpRequest::sendRequestByPut()
	{
		m_lastptr = NULL;

		bool first = true;

		char format[32 * 1024];

		std::vector<SForm>::iterator i = m_post.begin(), end = m_post.end();
		while (i != end)
		{
			SForm& pForm = (*i);

			if (first)
			{
				m_postField = "{";
			}
			else
			{
				m_postField += ",";
			}

			sprintf(format, "\"%s\":\"%s\"", pForm.Name.c_str(), pForm.Value.c_str());
			m_postField += format;

			first = false;
			i++;
		}

		m_postField += "}";

		m_sendRequest = true;

		m_headerlist = curl_slist_append(m_headerlist, "Accept: application/json");
		m_headerlist = curl_slist_append(m_headerlist, "Content-Type: application/json");
		m_headerlist = curl_slist_append(m_headerlist, "charsets: utf-8");

		// append header
		for (int i = 0, n = (int)m_headers.size(); i < n; i++)
		{
			m_headerlist = curl_slist_append(m_headerlist, m_headers[i].c_str());
		}

		// set url
		curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());
		curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headerlist);

		if (m_postField.empty() == false)
		{
			curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, m_postField.c_str());
		}

		if (m_sessionFile.empty() == false)
		{
			curl_easy_setopt(m_curl, CURLOPT_COOKIEFILE, m_sessionFile.c_str());
			curl_easy_setopt(m_curl, CURLOPT_COOKIEJAR, m_sessionFile.c_str());
		}

		curl_multi_remove_handle(m_multiHandle, m_curl);
		curl_multi_add_handle(m_multiHandle, m_curl);
		curl_multi_perform(m_multiHandle, &m_needContinue);

		m_httpCode = -1;
		m_downloading = 0;
		m_sizeBuffer = 0;

		m_bytePerSecond = 0;
		m_totalBytePerSecond = 0;

		m_requestTime = os::Timer::getTime();
		m_time = m_requestTime;
		m_revcTime = m_requestTime;
		m_currentTime = m_requestTime;

		// calc hash data revc
		memset(m_hashString, 0, HASHSTRING_SIZE);
		md5_init((MD5_CTX*)m_md5Context);
	}

	void CHttpRequest::sendRequestByDelete()
	{
		// resume download
		m_cancel = false;
		m_sendRequest = true;

		curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());
		curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "DELETE");

		// apply session
		if (m_sessionFile.empty() == false)
		{
			curl_easy_setopt(m_curl, CURLOPT_COOKIEFILE, m_sessionFile.c_str());
			curl_easy_setopt(m_curl, CURLOPT_COOKIEJAR, m_sessionFile.c_str());
		}

		// append header
		for (int i = 0, n = (int)m_headers.size(); i < n; i++)
		{
			m_headerlist = curl_slist_append(m_headerlist, m_headers[i].c_str());
			curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headerlist);
		}

		curl_multi_remove_handle(m_multiHandle, m_curl);
		curl_multi_add_handle(m_multiHandle, m_curl);
		curl_multi_perform(m_multiHandle, &m_needContinue);

		m_httpCode = -1;
		m_downloading = 0;
		m_sizeBuffer = 0;

		m_bytePerSecond = 0;
		m_totalBytePerSecond = 0;

		m_requestTime = os::Timer::getTime();
		m_time = m_requestTime;
		m_revcTime = m_requestTime;
		m_currentTime = m_requestTime;

		// calc hash data revc
		memset(m_hashString, 0, HASHSTRING_SIZE);
		md5_init((MD5_CTX*)m_md5Context);
	}

	void CHttpRequest::sendRequestByPost()
	{
		// clean form post
		if (m_formpost)
			curl_formfree((curl_httppost*)m_formpost);

		m_formpost = NULL;
		m_lastptr = NULL;

		std::vector<SForm>::iterator i = m_post.begin(), end = m_post.end();
		while (i != end)
		{
			SForm& pForm = (*i);

			if (pForm.File == false)
			{
				// form
				curl_formadd
				(
					(curl_httppost**)&m_formpost, (curl_httppost**)&m_lastptr,
					CURLFORM_COPYNAME, pForm.Name.c_str(),
					CURLFORM_COPYCONTENTS, pForm.Value.c_str(),
					CURLFORM_END
				);

				if (m_postField.empty())
					m_postField = "{";
				else
					m_postField += ",";

				m_postField += "\"";
				m_postField += pForm.Name;
				m_postField += "\":\"";
				m_postField += pForm.Value;
				m_postField += "\"";
			}
			else
			{
				curl_formadd
				(
					(curl_httppost**)&m_formpost, (curl_httppost**)&m_lastptr,
					CURLFORM_COPYNAME, pForm.Name.c_str(),
					CURLFORM_FILE, pForm.Value.c_str(),
					CURLFORM_END
				);
		
				m_checkTimeout = false;
			}

			i++;
		}

		if (m_postField.empty() == false)
			m_postField += "}";

		m_sendRequest = true;

		// set url
		curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());

		// apply session
		if (m_sessionFile.empty() == false)
		{
			curl_easy_setopt(m_curl, CURLOPT_COOKIEFILE, m_sessionFile.c_str());
			curl_easy_setopt(m_curl, CURLOPT_COOKIEJAR, m_sessionFile.c_str());
		}

		// add form to request
		if (m_formpost)
			curl_easy_setopt(m_curl, CURLOPT_HTTPPOST, m_formpost);

		// append header
		for (int i = 0, n = (int)m_headers.size(); i < n; i++)
		{
			m_headerlist = curl_slist_append(m_headerlist, m_headers[i].c_str());
			curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headerlist);
		}

		curl_multi_remove_handle(m_multiHandle, m_curl);
		curl_multi_add_handle(m_multiHandle, m_curl);
		curl_multi_perform(m_multiHandle, &m_needContinue);

		m_httpCode = -1;
		m_downloading = 0;
		m_sizeBuffer = 0;

		m_bytePerSecond = 0;
		m_totalBytePerSecond = 0;

		m_requestTime = os::Timer::getTime();
		m_time = m_requestTime;
		m_revcTime = m_requestTime;
		m_currentTime = m_requestTime;

		// calc hash data revc
		memset(m_hashString, 0, HASHSTRING_SIZE);
		md5_init((MD5_CTX*)m_md5Context);
	}

	void CHttpRequest::sendRequestByPostJson()
	{
		m_lastptr = NULL;

		bool first = true;

		char format[32 * 1024];

		std::vector<SForm>::iterator i = m_post.begin(), end = m_post.end();
		while (i != end)
		{
			SForm& pForm = (*i);

			if (pForm.File == false)
			{
				if (first)
				{
					m_postField = "{";
				}
				else
				{
					m_postField += ",";
				}

				sprintf(format, "\"%s\":\"%s\"", pForm.Name.c_str(), pForm.Value.c_str());
				m_postField += format;

				first = false;
			}

			i++;
		}

		m_postField += "}";

		m_sendRequest = true;

		m_headerlist = curl_slist_append(m_headerlist, "Accept: application/json");
		m_headerlist = curl_slist_append(m_headerlist, "Content-Type: application/json");
		m_headerlist = curl_slist_append(m_headerlist, "charsets: utf-8");

		// append header
		for (int i = 0, n = (int)m_headers.size(); i < n; i++)
		{
			m_headerlist = curl_slist_append(m_headerlist, m_headers[i].c_str());
		}

		// set url
		curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());
		curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headerlist);

		if (m_postField.empty() == false)
		{
			curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, m_postField.c_str());
		}

		// apply session
		if (m_sessionFile.empty() == false)
		{
			curl_easy_setopt(m_curl, CURLOPT_COOKIEFILE, m_sessionFile.c_str());
			curl_easy_setopt(m_curl, CURLOPT_COOKIEJAR, m_sessionFile.c_str());
		}

		curl_multi_remove_handle(m_multiHandle, m_curl);
		curl_multi_add_handle(m_multiHandle, m_curl);
		curl_multi_perform(m_multiHandle, &m_needContinue);

		m_httpCode = -1;
		m_downloading = 0;
		m_sizeBuffer = 0;

		m_bytePerSecond = 0;
		m_totalBytePerSecond = 0;

		m_requestTime = os::Timer::getTime();
		m_time = m_requestTime;
		m_revcTime = m_requestTime;
		m_currentTime = m_requestTime;

		// calc hash data revc
		memset(m_hashString, 0, HASHSTRING_SIZE);
		md5_init((MD5_CTX*)m_md5Context);
	}

	void CHttpRequest::sendRequestByGet()
	{
		// resume download
		m_cancel = false;
		m_sendRequest = true;

		curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());

		// append header
		for (int i = 0, n = (int)m_headers.size(); i < n; i++)
		{
			m_headerlist = curl_slist_append(m_headerlist, m_headers[i].c_str());
			curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headerlist);
		}

		// apply session
		if (m_sessionFile.empty() == false)
		{
			curl_easy_setopt(m_curl, CURLOPT_COOKIEFILE, m_sessionFile.c_str());
			curl_easy_setopt(m_curl, CURLOPT_COOKIEJAR, m_sessionFile.c_str());
		}

		curl_multi_remove_handle(m_multiHandle, m_curl);
		curl_multi_add_handle(m_multiHandle, m_curl);
		curl_multi_perform(m_multiHandle, &m_needContinue);

		m_httpCode = -1;
		m_downloading = 0;
		m_sizeBuffer = 0;

		m_bytePerSecond = 0;
		m_totalBytePerSecond = 0;

		m_requestTime = os::Timer::getTime();
		m_time = m_requestTime;
		m_revcTime = m_requestTime;
		m_currentTime = m_requestTime;

		// calc hash data revc
		memset(m_hashString, 0, HASHSTRING_SIZE);
		md5_init((MD5_CTX*)m_md5Context);
	}

	bool CHttpRequest::checkTimeOut()
	{
		if (getCurrentTimeOut() > getTimeOut())
		{
			m_hashString[0] = 0;
			return false;
		}
		return true;
	}

	bool CHttpRequest::updateRequest()
	{
		if (m_needContinue == 0)
		{
			return true;
		}

		// get current time
		m_currentTime = os::Timer::getTime();

		int maxfd = -1;
		fd_set fdread;
		fd_set fdwrite;
		fd_set fdexcep;
		//long curl_timeo = -1;
		struct timeval timeout;
		int rc = 0;

		//char log[1024];
		//sprintf(log, "Update request: %s", m_url.c_str());
		//os::Printer::log(log);

		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdexcep);

		timeout.tv_sec = 0;
		timeout.tv_usec = 0;

		curl_multi_fdset(m_multiHandle, &fdread, &fdwrite, &fdexcep, &maxfd);
		CURLMcode result = curl_multi_perform(m_multiHandle, &m_needContinue);

		if (m_needContinue == 1)
			rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);

		if (rc < 0)
		{
			m_bytePerSecond = 0;
			m_totalBytePerSecond = 0;

			// quit request
			if (checkTimeOut() == false && m_checkTimeout == true)
			{
				m_isTimeOut = true;
				return true;
			}

			if (m_needContinue == 0)
			{
				os::Printer::log("CHttpRequest close connection\n");
			}

			// todo wait dns thread
			return false;
		}

		// calc speed download	
		if (m_currentTime - m_time > 1000)
		{
			m_bytePerSecond = m_totalBytePerSecond;
			m_totalBytePerSecond = 0;
			m_time = m_currentTime;
		}

		if (m_needContinue == 1)
		{
			// quit request
			if (checkTimeOut() == false && m_checkTimeout == true)
			{
				os::Printer::log("Http request time out");
				m_isTimeOut = true;
				return true;
			}

			return false;
		}

		if (result == CURLM_OK)
		{
			int ret = -1;
			curl_easy_getinfo(m_curl, CURLINFO_HTTP_CODE, &ret);
			m_httpCode = ret;
		}
		else
		{
			m_httpCode = -1;
		}

		// add the end buffer...
		if (m_sizeBuffer > 0)
		{
			m_downloadBuffer[m_sizeBuffer] = 0;

			// update hash
			md5_update((MD5_CTX*)m_md5Context, m_downloadBuffer, m_sizeBuffer);

			// write file
			if (m_dataStream)
			{
				m_dataStream->write((void*)m_downloadBuffer, (unsigned int)m_sizeBuffer);
				m_dataStream->endStream();
			}

			m_sizeBuffer = 0;
		}

		// calc hash
		memset(m_hashString, 0, HASHSTRING_SIZE);
		unsigned char digest[16]; // 16*8 = 128bit
		md5_final((MD5_CTX*)m_md5Context, digest);

		// get hash string
		for (int i = 0; i < 16; ++i)
		{
			unsigned char nChar = digest[i];
			char& hex1 = m_hashString[i << 1];
			char& hex2 = m_hashString[(i << 1) + 1];
			hex1 = (nChar & 0xF0) >> 4;
			hex2 = (nChar & 0x0F);

			if (0 <= hex1 && hex1 <= 9) hex1 += '0';    //0 => 48 inascii
			if (10 <= hex1 && hex1 <= 15) hex1 += 'A' - 10; //A => 65 inascii
			if (0 <= hex2 && hex2 <= 9) hex2 += '0';
			if (10 <= hex2 && hex2 <= 15) hex2 += 'A' - 10;
		}
		m_hashString[32] = 0;

		for (int i = 0; i < HASHSTRING_SIZE; i++)
			m_hashString[i] = tolower(m_hashString[i]);

		return true;
	}

	void CHttpRequest::onRevcData(unsigned char* lpData, unsigned long size, unsigned long num)
	{
		// last time revc data
		m_revcTime = os::Timer::getTime();
		m_currentTime = os::Timer::getTime();

		if (m_sizeBuffer + size > DOWNLOADBUFFER_SIZE)
		{
			// update hash
			md5_update((MD5_CTX*)m_md5Context, m_downloadBuffer, m_sizeBuffer);

			// flush
			if (m_dataStream)
				m_dataStream->write((void*)m_downloadBuffer, (unsigned int)m_sizeBuffer);

			m_sizeBuffer = 0;
		}

		m_totalBytePerSecond = m_totalBytePerSecond + size;

		// push to buffer
		memcpy(m_downloadBuffer + m_sizeBuffer, lpData, size);
		m_sizeBuffer += size;
	}

	void CHttpRequest::onReadData(unsigned char* lpData, unsigned long size, unsigned long num)
	{
		// implement if upload data
	}

}

#endif
