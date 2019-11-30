// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#ifndef _SKYLICHT_SYSTEM_PTHREAD_H_
#define _SKYLICHT_SYSTEM_PTHREAD_H_

#include "IMutex.h"
#include "IThread.h"
#include "SkylichtSystemConfig.h"

#ifdef USE_PTHREAD

#include <pthread.h>

namespace SkylichtSystem
{
	class CPThread: public IThread
	{
	protected:
		pthread_t	m_pthread;

		bool		m_run;
	public:
		CPThread(IThreadCallback *callback);
		virtual ~CPThread();

		virtual void update();

		virtual void stop();

		static void* run(void *param);
	};
}

#endif

#endif