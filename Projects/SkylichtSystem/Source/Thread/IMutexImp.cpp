// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#include "stdafx.h"
#include "IMutex.h"
#include "SkylichtSystemConfig.h"

#if defined(USE_PTHREAD)
#include "CPThreadMutex.h"
#elif defined(USE_STDTHREAD)
#include "CSTDThreadMutex.h"
#endif

namespace SkylichtSystem
{
	IMutex* IMutex::createMutex()
	{
#if defined(USE_PTHREAD)
		return new CPThreadMutex();
#elif defined(USE_STDTHREAD)
		return new CSTDThreadMutex();
#else
		return NULL;
#endif
	}
}