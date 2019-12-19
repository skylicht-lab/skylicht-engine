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
		m_errorCode(0)
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
		char log[512];
		sprintf(log, "%s - process: %d - %d", m_fileName.c_str(), percent, size);
		os::Printer::log(log);
	}
}