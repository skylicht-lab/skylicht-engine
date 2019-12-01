// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#ifndef _SKYLICHT_SYSTEM_PTHREADMUTEX_H_
#define _SKYLICHT_SYSTEM_PTHREADMUTEX_H_

#include "IMutex.h"
#include "SkylichtSystemConfig.h"

#ifdef USE_PTHREAD

#include <pthread.h>

namespace SkylichtSystem
{
	class CPThreadMutex: public IMutex
	{
	protected:
		pthread_mutex_t	m_mutex;

	public:
		CPThreadMutex();
		virtual ~CPThreadMutex();

		virtual void lock();
		virtual void unlock();
	};
}

#endif

#endif