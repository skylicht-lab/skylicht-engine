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

#ifdef USE_OPENSL

#include "CDriverNull.h"

#include "Thread/IThread.h"
#include "Thread/IMutex.h"
using namespace Skylicht::System;

// opensl library
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Platform.h>

namespace Skylicht
{
	namespace Audio
	{
		class CDriverOpenSL : public CDriverNull
		{
		public:
			CDriverOpenSL();
			virtual ~CDriverOpenSL();
			
			virtual void init();
			virtual void shutdown();
			virtual void suspend();
			virtual void resume();
			
			// doCallbackOSL
			// fill buffer
			void doCallbackOSL();
			
			// callback OpenSLES
			static void callbackOSL(SLBufferQueueItf bufferQueue, void *caller);
			
		protected:
			unsigned int m_minBufferSize;
			
			//Engine
			SLObjectItf	m_engineObject;
			SLEngineItf m_engine;
			SLObjectItf m_output;
			
			//Player
			SLObjectItf	m_playerObject;
			SLPlayItf m_play;
			SLBufferQueueItf m_bufferQueue;
			
			unsigned char**	m_buffers;
			SLuint32 m_numBuffers;
			unsigned int m_bufferSizeOSL;
			unsigned int m_currentBuffer;
			
			bool	m_active;
		};
	}
}

#endif
