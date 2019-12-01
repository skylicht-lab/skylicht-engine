// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#ifndef _SKYLICHT_SYSTEM_STDTHREADMUTEX_H_
#define _SKYLICHT_SYSTEM_STDTHREADMUTEX_H_

#include "IMutex.h"
#include <mutex>
#include "SkylichtSystemConfig.h"

#ifdef USE_STDTHREAD

namespace SkylichtSystem
{
	class CSTDThreadMutex: public IMutex
	{
	protected:
		std::mutex m_mutex;

	public:
		CSTDThreadMutex();
		virtual ~CSTDThreadMutex();

		virtual void lock();
		virtual void unlock();
	};
}

#endif

#endif