/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "CCrashHandler.h"

#include "CApplication.h"
#include "Utils/CStringImp.h"
#include "Utils/CPath.h"

#ifdef USE_CRASHHANDLER
#define MAX_STACK_DEPTH 64
#define MAX_UNDECORATEDNAME_LENGTH 256
#endif

namespace Skylicht
{
#ifdef USE_CRASHHANDLER
	HANDLE g_process = nullptr;

	bool resolveAddrName(DWORD64 addr, std::string& functionName)
	{
		if (g_process == nullptr)
			return false;

		SymbolBuffer symbol;
		DWORD64 disp = 0;
		if (SymFromAddr(g_process, (DWORD64)addr, &disp, &symbol) == false)
			return false;

		functionName = std::to_string(addr);
		functionName += ": ";
		functionName += symbol.Name;

		char undecoratedName[MAX_UNDECORATEDNAME_LENGTH];
		memset(undecoratedName, 0, MAX_UNDECORATEDNAME_LENGTH);
		DWORD UNDECORATE_FLAGS = UNDNAME_NO_ACCESS_SPECIFIERS | UNDNAME_NO_MEMBER_TYPE | UNDNAME_NO_MS_KEYWORDS | UNDNAME_NO_LEADING_UNDERSCORES;

		if (UnDecorateSymbolName(symbol.Name, (LPSTR)undecoratedName, MAX_UNDECORATEDNAME_LENGTH, UNDECORATE_FLAGS) == false)
			return true;

		functionName = std::to_string(addr);
		functionName += ": ";
		functionName += undecoratedName;

		IMAGEHLP_LINE line;
		memset(&line, 0, sizeof(IMAGEHLP_LINE));
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE);

		PDWORD dispw = 0;
		if (SymGetLineFromAddr(g_process, (DWORD64)addr, (PDWORD)&dispw, &line) == false)
			return true;

		functionName += "\n    - File: ";
		functionName += line.FileName;
		functionName += "\n    - Line: ";
		functionName += std::to_string(line.LineNumber);

		return true;
	}

	std::string resolveAddr(const std::string& line)
	{
		std::string result;

		try
		{
			size_t pos = 0;
			DWORD64 addr = (DWORD64)std::stoll(line, &pos);
			if (pos == line.length())
			{
				std::string name;
				if (resolveAddrName(addr, name))
					result = name;
				else
					result = line;
			}
		}
		catch (const std::out_of_range& e)
		{
			result = line;
		}
		catch (const std::invalid_argument& e)
		{
			result = line;
		}

		return result;
	}

	void resolveFile(const wchar_t* dumpFile, const wchar_t* resultFile)
	{
		HANDLE hFile = CreateFile(
			dumpFile,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		if (hFile == INVALID_HANDLE_VALUE)
			return;

		DWORD fileSizeHigh = 0;
		DWORD fileSizeLow = GetFileSize(hFile, &fileSizeHigh);
		if (fileSizeLow == INVALID_FILE_SIZE && GetLastError() != NO_ERROR)
		{
			CloseHandle(hFile);
			return;
		}

		if (fileSizeHigh > 0)
		{
			// large file > 4gb
			CloseHandle(hFile);
			return;
		}

		std::string fileContent;
		fileContent.resize(fileSizeLow);

		DWORD bytesRead = 0;
		BOOL readResult = ReadFile(
			hFile,
			&fileContent[0],
			fileSizeLow,
			&bytesRead,
			NULL
		);

		if (!readResult || bytesRead != fileSizeLow)
			return;

		CloseHandle(hFile);

		std::string resolveData;

		std::string currentLine;
		for (char c : fileContent)
		{
			if (c == '\n')
			{
				if (!currentLine.empty())
				{
					resolveData += resolveAddr(currentLine);
					resolveData += "\n";
				}
				else
				{
					resolveData += "\n";
				}
				currentLine.clear();
			}
			else if (c != '\r')
			{
				currentLine += c;
			}
		}

		if (!currentLine.empty())
			resolveData += resolveAddr(currentLine);

		if (resolveData.size() > 0)
		{
			HANDLE hDumpFile = CreateFile(resultFile,
				GENERIC_WRITE,
				FILE_SHARE_WRITE,
				0,
				CREATE_ALWAYS,
				0,
				0);

			DWORD dwBytesWritten;
			WriteFile(hDumpFile, resolveData.c_str(), (DWORD)resolveData.size(), &dwBytesWritten, NULL);

			CloseHandle(hDumpFile);
		}
	}

	static LONG WINAPI dump(EXCEPTION_POINTERS* exp)
	{
		SYSTEMTIME time;
		GetLocalTime(&time);

		std::wstring appName = CStringImp::convertUTF8ToUnicode(getApplication()->getAppName());

		wchar_t szFileName[MAX_PATH];
		swprintf(szFileName, MAX_PATH, L"%s-%04d-%02d%02d-%02d%02d.dmp",
			appName.c_str(),
			time.wYear,
			time.wMonth,
			time.wDay,
			time.wHour,
			time.wMinute);

		void* callstackAddress[MAX_STACK_DEPTH];
		int stackFrame = CaptureStackBackTrace(0, MAX_STACK_DEPTH, callstackAddress, nullptr);

		HANDLE hDumpFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
		std::string buffer;

		buffer = "CRASH HANDLER\n\n";

		for (int i = 0; i < stackFrame; i++)
		{
			buffer += std::to_string((DWORD64)callstackAddress[i]);
			buffer += "\n";
		}

		DWORD dwBytesWritten;
		WriteFile(hDumpFile, buffer.c_str(), (DWORD)buffer.size(), &dwBytesWritten, NULL);

		CloseHandle(hDumpFile);

		wchar_t szResult[MAX_PATH];
		swprintf(szResult, MAX_PATH, L"%s-%04d-%02d%02d-%02d%02d.txt",
			appName.c_str(),
			time.wYear,
			time.wMonth,
			time.wDay,
			time.wHour,
			time.wMinute);

		resolveFile(szFileName, szResult);

		return EXCEPTION_EXECUTE_HANDLER;
	}
#endif

	IMPLEMENT_SINGLETON(CCrashHandler);

	CCrashHandler::CCrashHandler()
	{

	}

	CCrashHandler::~CCrashHandler()
	{
#ifdef USE_CRASHHANDLER
		if (g_process != nullptr)
		{
			SymCleanup(g_process);
			g_process = nullptr;
		}
#endif
	}

	void CCrashHandler::init()
	{
#ifdef USE_CRASHHANDLER
		SetUnhandledExceptionFilter(dump);

		g_process = GetCurrentProcess();

		DWORD SYM_OPTIONS = SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_PUBLICS_ONLY;
		SymSetOptions(SYM_OPTIONS);
		SymInitialize(g_process, NULL, TRUE);
#endif
	}

	void CCrashHandler::resolveDumpFiles()
	{
#ifdef USE_CRASHHANDLER
		WIN32_FIND_DATAA findFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		std::string searchPath = ".\\*.dmp";

		hFind = FindFirstFileA(searchPath.c_str(), &findFileData);

		if (hFind == INVALID_HANDLE_VALUE)
			return;

		std::vector<std::string> dumpFiles;
		do
		{
			if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0)
			{
				if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					dumpFiles.push_back(findFileData.cFileName);
				}
			}
		} while (FindNextFileA(hFind, &findFileData) != 0);

		FindClose(hFind);

		for (auto s : dumpFiles)
		{
			std::string r = CPath::replaceFileExt(s, ".log");

			std::wstring src = CStringImp::convertUTF8ToUnicode(s.c_str());
			std::wstring dst = CStringImp::convertUTF8ToUnicode(r.c_str());

			resolveFile(src.c_str(), dst.c_str());
		}
#endif
	}
}