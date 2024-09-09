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

#ifdef USE_XAUDIO2

#include <windows.h>
#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")

#include "CDriverNull.h"

#include "Thread/IThread.h"
#include "Thread/IMutex.h"
using namespace Skylicht::System;

namespace Skylicht
{
	namespace Audio
	{
		//--------------------------------------------------------------------------------------
		// Callback structure
		//--------------------------------------------------------------------------------------
		struct StreamingVoiceContext : public IXAudio2VoiceCallback
		{
			STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32) override
			{
			}
			STDMETHOD_(void, OnVoiceProcessingPassEnd)() override
			{
			}
			STDMETHOD_(void, OnStreamEnd)() override
			{
			}
			STDMETHOD_(void, OnBufferStart)(void*) override
			{
			}
			STDMETHOD_(void, OnBufferEnd)(void*) override
			{
			}
			STDMETHOD_(void, OnLoopEnd)(void*) override
			{
			}
			STDMETHOD_(void, OnVoiceError)(void*, HRESULT) override
			{
			}
			
			HANDLE hBufferEndEvent;
			
			StreamingVoiceContext()
			{
			}
			
			virtual ~StreamingVoiceContext()
			{
			}
		};
		
		
		class CDriverXAudio2 :
			public CDriverNull,
			public IThreadCallback
		{
		protected:
			IXAudio2* m_xaudio;
			IXAudio2MasteringVoice* m_masteringVoice;
			
			StreamingVoiceContext m_voiceContext;
			IXAudio2SourceVoice* m_sourceVoice;
			
		public:
			CDriverXAudio2();
			virtual ~CDriverXAudio2();
			
			virtual void init();
			virtual void shutdown();
			virtual void suspend();
			virtual void resume();
			
			virtual void updateThread();
		protected:
			int m_minBufferSize;
			int m_v5MinLatency;
			int m_v6MinLatency;
			
			// Buffer parameters
			struct SBufferParam
			{
				int NumBuffers;
				int BufferSize;
				int CurrentBuffer;
				unsigned char** Buffers;
				
				// MMSytem parameters
				// WAVEFORMATEX Waveformat;
				// WAVEHDR *WaveBlocks;		// Objects containing buffer address provided (by reference) to mmsystem.
				
				IThread* UpdateThread;
				bool IsThreadRunning;
			};
			
			SBufferParam m_output;
			SBufferParam m_input;
		};
	}
}
#endif
