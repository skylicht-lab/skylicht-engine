/*
!@
MIT License

Copyright (c) 2012 - 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#ifndef _SKYLICHT_AUDIO_CONFIG_
#define _SKYLICHT_AUDIO_CONFIG_

#if !defined(__EMSCRIPTEN__) && !defined(IOS) && !defined(MACOS)
// iOS & MacOS can not change the duration length & buffer size, so we dont use multithread
// webgl emscripten do not support multithread on some browser
#define USE_MULTITHREAD_UPDATE
#endif

#if defined(ANDROID)
// Use OpenSL on Android
#define USE_OPENSL
#elif defined(LINUX) || defined(__EMSCRIPTEN__)
// Use OpenAL on Linux & Emscripten
#define USE_OPENAL
#elif defined(IOS) || defined(MACOS)
// Use AudioUnit on IOS, MAC
#define USE_AUDIO_UNIT
#elif defined(_WIN32)
#include <Windows.h>
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP || WINAPI_FAMILY == WINAPI_FAMILY_PC_APP
// Use XAudio on Store App
#define USE_XAUDIO2
#else
// Use WinMM on Desktop App
#define USE_MMDRIVER
#endif
#endif

#endif
