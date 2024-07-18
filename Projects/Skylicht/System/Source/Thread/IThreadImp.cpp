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

#include "stdafx.h"
#include "IThread.h"

#if defined(USE_PTHREAD)
#include "CPThread.h"
#elif defined(USE_STDTHREAD)
#include "CSTDThread.h"
#elif defined(USE_WINTHREAD)
#include "CWinThread.h"
#endif


#if defined(_WIN32)
#include <Windows.h>
#ifdef WINDOWS_STORE
#include <thread>
#endif
#elif defined(EMSCRIPTEN)
#include <sys/time.h>
#else		
#include <sys/time.h>
#endif

namespace Skylicht
{
	namespace System
	{
		float IThread::getTime()
		{
	#if defined(_WIN32)
			LARGE_INTEGER freq, start;
			double ticks, f;
			
			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&start);
			f = (double)freq.QuadPart;
			ticks = (double)start.QuadPart;
			
			double time = ticks / f;
			return (float)time * 1000.0f;
	#elif defined(ANDROID)
			timespec systemtime;
			clock_gettime(CLOCK_MONOTONIC, &systemtime);
			return (float)((systemtime.tv_sec * 1000UL) + (systemtime.tv_nsec / 1000000UL));
	#else
			struct timeval tp;
			double sec, usec;
			gettimeofday(&tp, 0);
			sec = tp.tv_sec;
			usec = tp.tv_usec;
			double time = (sec + usec / 1000000);
			return (float)time * 1000.0f;
	#endif
		}
		
		void IThread::sleep(unsigned int time)
		{
	#ifdef _WIN32
			//#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			// Window phone
			// std::this_thread::sleep_for(std::chrono::milliseconds(time));
			//#else
			// Win32
			Sleep(time);
			//#endif
	#elif defined(EMSCRIPTEN)
			
	#else
			struct timespec ts;
			ts.tv_sec = (time_t)(time / 1000);
			ts.tv_nsec = (long)(time % 1000) * 1000000;
			
			nanosleep(&ts, NULL);
	#endif
		}
		
		IThread* IThread::createThread(IThreadCallback* callback)
		{
	#if defined(USE_PTHREAD)
			printf("[IThread] Create pthread\n");
			return new CPThread(callback);
	#elif defined(USE_STDTHREAD)
			printf("[IThread] Create std::thread\n");
			return new CSTDThread(callback);
	#elif defined(USE_WINTHREAD)
			printf("[IThread] Create winthread\n");
			return new CWinThread(callback);
	#else
			printf("[IThread] Warning: create null thread\n");
			return NULL;
	#endif
		}
	}
}
