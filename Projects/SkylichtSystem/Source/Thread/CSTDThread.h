// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#ifndef _SKYLICHT_SYSTEM_PTHREAD_H_
#define _SKYLICHT_SYSTEM_PTHREAD_H_

#include "IMutex.h"
#include "IThread.h"
#include "SkylichtSystemConfig.h"

#ifdef USE_STDTHREAD

#include <thread>

namespace SkylichtSystem
{
	class CSTDThread: public IThread
	{
	protected:
		std::thread	*m_thread;
		bool		m_run;
	public:
		CSTDThread(IThreadCallback *callback);
		virtual ~CSTDThread();

		virtual void update();

		virtual void stop();

		static void* run(void *param);
	};
}

#endif

#endif