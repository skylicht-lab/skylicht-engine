// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#include "stdafx.h"
#include "CPThreadMutex.h"

#ifdef USE_PTHREAD

namespace SkylichtSystem
{

	CPThreadMutex::CPThreadMutex()
	{
		pthread_mutex_init(&m_mutex, 0);
	}

	CPThreadMutex::~CPThreadMutex()
	{
		pthread_mutex_destroy(&m_mutex);
	}

	void CPThreadMutex::lock()
	{
		pthread_mutex_lock(&m_mutex);
	}

	void CPThreadMutex::unlock()
	{
		pthread_mutex_unlock(&m_mutex);
	}

}

#endif