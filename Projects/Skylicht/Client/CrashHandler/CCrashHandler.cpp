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

#ifdef USE_CRASHHANDLER
#define MAX_STACK_DEPTH 64
#define MAX_UNDECORATEDNAME_LENGTH 256
#endif

namespace Skylicht
{
#ifdef USE_CRASHHANDLER
	HANDLE g_process = nullptr;

	bool resolve(DWORD64 addr, std::string& functionName)
	{
		if (g_process == nullptr)
			return false;

		SymbolBuffer symbol;
		DWORD64 disp = 0;
		if (SymFromAddr(g_process, (DWORD64)addr, &disp, &symbol) == false)
			return false;

		IMAGEHLP_LINE line;
		memset(&line, 0, sizeof(IMAGEHLP_LINE));
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE);

		PDWORD dispw = 0;
		if (SymGetLineFromAddr(g_process, (DWORD64)addr, (PDWORD)&dispw, &line) == false)
			return false;

		char undecoratedName[MAX_UNDECORATEDNAME_LENGTH];
		memset(undecoratedName, 0, MAX_UNDECORATEDNAME_LENGTH);
		DWORD UNDECORATE_FLAGS = UNDNAME_NO_ACCESS_SPECIFIERS | UNDNAME_NO_MEMBER_TYPE | UNDNAME_NO_MS_KEYWORDS | UNDNAME_NO_LEADING_UNDERSCORES;

		if (UnDecorateSymbolName(symbol.Name, (LPSTR)undecoratedName, MAX_UNDECORATEDNAME_LENGTH, UNDECORATE_FLAGS) == false)
		{
			return false;
		}

		functionName = undecoratedName;
		functionName += "\n    - File: ";
		functionName += line.FileName;
		functionName += "\n    - Line: ";
		functionName += std::to_string(line.LineNumber);

		return true;
	}

	static LONG WINAPI dump(EXCEPTION_POINTERS* exp)
	{
		SYSTEMTIME time;
		GetLocalTime(&time);

		wchar_t szFileName[MAX_PATH];
		swprintf(szFileName, MAX_PATH, L"%s-%04d-%02d%02d-%02d%02d.dmp",
			L"SkylichtApp",
			time.wYear,
			time.wMonth,
			time.wDay,
			time.wHour,
			time.wMinute);

		void* callstackAddress[MAX_STACK_DEPTH];
		int stackFrame = CaptureStackBackTrace(0, MAX_STACK_DEPTH, callstackAddress, nullptr);

		HANDLE hDumpFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
		std::string buffer, funcName;

		buffer = "CRASH HANDLER\n\n";

		for (int i = 0; i < stackFrame; i++)
		{
			if (resolve((DWORD64)callstackAddress[i], funcName))
			{
				buffer += funcName;
				buffer += "\n";
			}
			else
			{
				buffer += std::to_string((DWORD64)callstackAddress[i]);
				buffer += "\n";
			}
		}

		DWORD dwBytesWritten;
		WriteFile(hDumpFile, buffer.c_str(), (DWORD)buffer.size(), &dwBytesWritten, NULL);

		CloseHandle(hDumpFile);

		return EXCEPTION_EXECUTE_HANDLER;
	}
#endif

	IMPLEMENT_SINGLETON(CCrashHandler);

	CCrashHandler::CCrashHandler()
	{

	}

	CCrashHandler::~CCrashHandler()
	{
		if (g_process != nullptr)
		{
			SymCleanup(g_process);
			g_process = nullptr;
		}
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
}