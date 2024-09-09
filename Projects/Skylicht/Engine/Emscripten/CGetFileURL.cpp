/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
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

#include "CGetFileURL.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/fetch.h>
#endif

namespace Skylicht
{
	void wget_load(unsigned int s, void* userData, const char *path)
	{
		CGetFileURL *file = reinterpret_cast<CGetFileURL*>(userData);
		file->onLoad(s, path);
	}

	void wget_status(unsigned int s, void* userData, int status)
	{
		CGetFileURL *file = reinterpret_cast<CGetFileURL*>(userData);
		file->onStatus(s, status);
	}

	void wget_process(unsigned int s, void* userData, int percent)
	{
		CGetFileURL *file = reinterpret_cast<CGetFileURL*>(userData);
		file->onProcess(s, percent);
	}

	CGetFileURL::CGetFileURL(const char *url, const char *fileName) :
		m_url(url),
		m_fileName(fileName),
		m_state(None),
		m_errorCode(0),
		m_percent(0)
	{

	}

	CGetFileURL::~CGetFileURL()
	{

	}

	void CGetFileURL::download(EMethod method)
	{
#ifdef __EMSCRIPTEN__
		const char *getMethod = "GET";
		if (method == CGetFileURL::Post)
			getMethod = "POST";

		m_state = CGetFileURL::Downloading;

		emscripten_async_wget2(
			m_url.c_str(),
			m_fileName.c_str(),
			getMethod,
			m_params.c_str(),
			reinterpret_cast<void*>(this),
			wget_load,
			wget_status,
			wget_process
		);
#endif
	}

	void CGetFileURL::onLoad(unsigned int size, const char *path)
	{
		char log[512];
		sprintf(log, "%s - load: %d", m_fileName.c_str(), size);
		os::Printer::log(log);
		
		m_state = CGetFileURL::Finish;
	}

	void CGetFileURL::onStatus(unsigned int size, int status)
	{
		char log[512];
		sprintf(log, "%s - status: %d - %d", m_fileName.c_str(), status, size);
		os::Printer::log(log);

		if (status != 0)
			m_state = CGetFileURL::Error;

		m_errorCode = status;
	}

	void CGetFileURL::onProcess(unsigned int size, int percent)
	{
		m_percent = percent;

		char log[512];
		sprintf(log, "%s - process: %d - %d", m_fileName.c_str(), percent, size);
		os::Printer::log(log);
	}
}