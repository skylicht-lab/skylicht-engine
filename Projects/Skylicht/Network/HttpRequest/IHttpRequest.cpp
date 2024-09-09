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
#include "IHttpRequest.h"

namespace Skylicht
{
	namespace Network
	{
		IHttpRequest::IHttpRequest(IHttpStream* stream):
		m_dataStream(stream),
		m_userData(NULL),
		m_requestType(IHttpRequest::Get),
		m_httpCode(-1),
		m_requestID(-1)
		{
			
		}
		
		IHttpRequest::~IHttpRequest()
		{
			if (m_dataStream)
				delete m_dataStream;
		}
		
		void IHttpRequest::addFormRequest(const char* name, const char* value)
		{
			m_post.push_back(SForm());
			SForm& form = m_post.back();
			
			form.Name = name;
			form.Value = value;
			form.File = false;
		}
		
		void IHttpRequest::addFormFileRequest(const char* name, const char* value)
		{
			m_post.push_back(SForm());
			SForm& form = m_post.back();
			
			form.Name = name;
			form.Value = value;
			form.File = true;
		}
	}
}
