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
#include "IHttpStream.h"

namespace Skylicht
{
	namespace Network
	{
		class IHttpRequest
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
			
		protected:
			std::string m_url;
			
			int m_httpCode;
			int m_requestID;
			
			ERequestType m_requestType;
			IHttpStream* m_dataStream;
			
			std::vector<SForm> m_post;
			void* m_userData;
			
			std::vector<std::string> m_headers;
		public:
			IHttpRequest(IHttpStream* stream);
			
			virtual ~IHttpRequest();
			
			void addFormRequest(const char* name, const char* value);
			
			void addFormFileRequest(const char* name, const char* value);
			
			virtual void sendRequest() = 0;
			
			inline void setURL(const char* url)
			{
				m_url = url;
			}
			
			inline const std::string& getURL()
			{
				return m_url;
			}
			
			inline void setSendRequestType(ERequestType type)
			{
				m_requestType = type;
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
		};
	}
}
