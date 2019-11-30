// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

#ifndef _SKYLICHT_SYSTEM_CONFIG_
#define _SKYLICHT_SYSTEM_CONFIG_

#if defined(ANDROID)
	#define USE_PTHREAD
#elif defined(EMSCRIPTEN)
	#define USE_PTHREAD
#elif defined(IOS)
	#define USE_PTHREAD
#elif defined(CYGWIN) || defined(MINGW)
	#define USE_PTHREAD
#elif defined(_WIN32)
	#include <Windows.h>
	#define USE_STDTHREAD
#elif defined(LINUX)
	#define USE_PTHREAD
#endif

#endif
