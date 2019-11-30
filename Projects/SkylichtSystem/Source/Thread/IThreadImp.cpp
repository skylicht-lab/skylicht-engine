// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#include "stdafx.h"
#include "IThread.h"
#include "SkylichtSystemConfig.h"

#if defined(USE_PTHREAD)
#include "CPThread.h"
#elif defined(USE_STDTHREAD)
#include "CSTDThread.h"
#endif



#ifdef _WIN32
#include <Windows.h>
	
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#include <thread>
#endif

#else
#include <sys/time.h>
#endif

namespace SkylichtSystem
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

		double time = ticks/f;
		return (float)time * 1000.0f;
#elif defined(ANDROID)
		timespec systemtime;
		clock_gettime(CLOCK_MONOTONIC, &systemtime);
		return (float)((systemtime.tv_sec * 1000UL) + (systemtime.tv_nsec / 1000000UL));
#else
		struct timeval	tp;
		double sec, usec;
		gettimeofday(&tp, 0);
		sec = tp.tv_sec;
		usec = tp.tv_usec;
		double time = (sec + usec/1000000);
		return (float)time * 1000.0f;
#endif
	}

	// sleep
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
#else
		struct timespec ts;
		ts.tv_sec = (time_t) (time / 1000);
		ts.tv_nsec = (long) (time % 1000) * 1000000;

		nanosleep(&ts, NULL);
#endif
	}

	// createThread
	// create new thread
	IThread* IThread::createThread(IThreadCallback* callback)
	{
#if defined(USE_PTHREAD)
		return new CPThread(callback);
#elif defined(USE_STDTHREAD)
		return new CSTDThread(callback);
#else
	return NULL;
#endif		
	}
}