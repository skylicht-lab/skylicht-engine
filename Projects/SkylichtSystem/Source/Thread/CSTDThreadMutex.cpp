// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#include "stdafx.h"
#include "CSTDThreadMutex.h"

#ifdef USE_STDTHREAD

namespace SkylichtSystem
{
	CSTDThreadMutex::CSTDThreadMutex()
	{
	}

	CSTDThreadMutex::~CSTDThreadMutex()
	{
	}

	void CSTDThreadMutex::lock()
	{
		m_mutex.lock();
	}

	void CSTDThreadMutex::unlock()
	{
		m_mutex.unlock();
	}
}

#endif