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

#pragma once

#include "SkylichtAudioConfig.h"

#ifdef USE_OPENAL

#include "CDriverNull.h"

#include "Thread/IThread.h"
#include "Thread/IMutex.h"
using namespace Skylicht::System;

// OpenAL
#ifdef _WIN32
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#else
#include "AL/al.h"
#include "AL/alc.h"
#endif

#define NUM_ALBUFFERS 2

#ifdef _WIN32
#pragma comment(lib,"OpenAL32.lib")
#endif

namespace Skylicht
{
	namespace Audio
	{
		class CDriverOpenAL :
			public CDriverNull,
			public IThreadCallback
		{
		protected:
			ALCdevice* m_device;
			ALCcontext* m_context;
			
			ALuint m_alBufferID[NUM_ALBUFFERS];
			ALuint m_alSourceID;
			
			int m_minBufferSize;
			unsigned char* m_buffer;
			int m_numBuffers;
			int m_bufferSizeOAL;
			int m_currentBuffer;
			
			bool m_isPause;
			
			IMutex *m_mutex;
			
		public:
			CDriverOpenAL();
			virtual ~CDriverOpenAL();
			
			virtual void init();
			virtual void shutdown();
			virtual void suspend();
			virtual void resume();
			virtual void update();
			virtual void updateThread();
			
		public:
			IThread* UpdateThread;
			bool IsThreadRunning;
		};
	}
}

#endif
