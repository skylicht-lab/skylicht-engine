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

#pragma once

#include "Utils/CSingleton.h"

#ifdef USE_CRASHHANDLER
#include <Windows.h>
#include <Dbghelp.h>

#pragma comment(lib, "Dbghelp.lib")
#endif

namespace Skylicht
{
	class CCrashHandler
	{
	public:
		DECLARE_SINGLETON(CCrashHandler)

		CCrashHandler();

		virtual ~CCrashHandler();

		void init();
	};

#ifdef USE_CRASHHANDLER
	class SymbolBuffer : public SYMBOL_INFO
	{
	public:
		SymbolBuffer()
		{
			MaxNameLen = MaxStringLength;
			SizeOfStruct = sizeof(SYMBOL_INFO);
		}

		~SymbolBuffer()
		{

		}

	private:
		static constexpr size_t MaxStringLength = 512;

	private:

		wchar_t buffer[MaxStringLength];
#endif

	};
}