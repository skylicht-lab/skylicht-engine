#pragma once

#if defined(_WIN32) || defined(CYGWIN) || defined(MINGW)
#include <Windows.h>

#ifdef WINDOWS_STORE
#include <wrl.h>
#include <wrl/client.h>
#include <agile.h>
#include <concrt.h>
#endif

#endif

#include "AppInclude.h"
