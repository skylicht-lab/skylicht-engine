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

#include "stdafx.h"
#include "CDriverXAudio2.h"
#include "SkylichtAudio.h"

#ifdef USE_XAUDIO2

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=nullptr; } }
#endif

namespace Skylicht
{
	namespace Audio
	{
		CDriverXAudio2::CDriverXAudio2()
		{
			m_v5MinLatency = 10240; // In bytes (= 5 * 512 samples). For versions 5 and earlier
			m_v6MinLatency = 14336; // In bytes (= 7 * 512 samples). For versions 6 and later
			m_minBufferSize = 2048;
			
			m_output.CurrentBuffer = 0;
			m_output.NumBuffers = 2;
			
			init();
		}
		
		CDriverXAudio2::~CDriverXAudio2()
		{
			shutdown();
		}
		
		void CDriverXAudio2::init()
		{
			printLog("[SkylichtAudio] Init CDriverXAudio2");
			
			// Calc buffer size (byte)
			// 2 channel u16 -> 4 bytes
			m_output.BufferSize = (static_cast<int>(m_bufferLength * m_preferedRate)) * 4;
			
			// Force buffer size to the next multiple of <m_minBufferSize> samples
			m_output.BufferSize += m_minBufferSize - (m_output.BufferSize % m_minBufferSize);
			
			// Set a lower limit to buffer size
			if (m_output.BufferSize < m_minBufferSize)
				m_output.BufferSize = m_minBufferSize;
			
			// Total buffer size must be higher or equal to 7 x 512 samples
			if (m_output.NumBuffers * m_output.BufferSize < m_v6MinLatency)
			{
				int nbExtraBytesNeeded = m_v6MinLatency - (m_output.NumBuffers * m_output.BufferSize);
				int nbExtraBuffersFloor = nbExtraBytesNeeded / m_output.BufferSize;
				m_output.NumBuffers += nbExtraBuffersFloor + ((nbExtraBytesNeeded % m_output.BufferSize) > 0);
			}
			
			// Create buffers
			m_output.Buffers = new unsigned char*[m_output.NumBuffers];
			
			// Init bufer
			for (int i = 0; i < m_output.NumBuffers; i++)
			{
				m_output.Buffers[i] = new unsigned char[m_output.BufferSize];
				memset(m_output.Buffers[i], 0, m_output.BufferSize);
			}
			
			// todo: very important!
			// recalc length
			m_bufferLength = (m_output.BufferSize / (float)m_preferedRate) / 4.0f;
			m_output.BufferSize = getBufferSize() * 4;
			
			CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			
			m_xaudio = nullptr;
			
			UINT32 flags = 0;
	#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
			flags |= XAUDIO2_DEBUG_ENGINE;
	#endif
			
			HRESULT hr = XAudio2Create(&m_xaudio, flags);
			if (FAILED(hr))
			{
				printLog("CDriverXAudio2 Failed to init XAudio2 engine: %#X\n", hr);
				CoUninitialize();
				return;
			}
			
	#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
			// To see the trace output, you need to view ETW logs for this application:
			//    Go to Control Panel, Administrative Tools, Event Viewer.
			//    View->Show Analytic and Debug Logs.
			//    Applications and Services Logs / Microsoft / Windows / XAudio2.
			//    Right click on Microsoft Windows XAudio2 debug logging, Properties, then Enable Logging, and hit OK
			XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
			debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
			debug.BreakMask = XAUDIO2_LOG_ERRORS;
			m_xaudio->SetDebugConfiguration(&debug, 0);
	#endif
			
			//
			// Create a mastering voice
			//
			m_masteringVoice = nullptr;
			
			if (FAILED(hr = m_xaudio->CreateMasteringVoice(&m_masteringVoice)))
			{
				printLog("CDriverXAudio2 Failed creating mastering voice: %#X\n", hr);
				SAFE_RELEASE(m_xaudio);
				CoUninitialize();
			}
			
			// setup wave format
			WAVEFORMATEX	waveFormat;
			waveFormat.wFormatTag = WAVE_FORMAT_PCM;
			waveFormat.wBitsPerSample = 16;
			waveFormat.nChannels = 2;
			waveFormat.nSamplesPerSec = m_preferedRate;
			waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8);
			waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
			
			if (FAILED(hr = m_xaudio->CreateSourceVoice(&m_sourceVoice, &waveFormat, 0, 1.0f, &m_voiceContext)))
			{
				printLog("Error %#X creating source voice", hr);
				return;
			}
			
			// start voice source
			m_sourceVoice->Start(0, 0);
			
			
			// start thread
			m_output.UpdateThread = IThread::createThread(this);
			m_output.IsThreadRunning = true;
		}
		
		void CDriverXAudio2::shutdown()
		{
			if (m_output.IsThreadRunning)
			{
				// Terminate update thread
				m_output.IsThreadRunning = false;
				
				if (m_output.UpdateThread)
				{
					m_output.UpdateThread->stop();
					delete m_output.UpdateThread;
					m_output.UpdateThread = NULL;
				}
				
				//
				// Cleanup XAudio2
				//
				
				// All XAudio2 interfaces are released when the engine is destroyed, but being tidy
				m_sourceVoice->Stop(0);
				m_sourceVoice->DestroyVoice();
				
				m_masteringVoice->DestroyVoice();
				SAFE_RELEASE(m_xaudio);
				CoUninitialize();
				
				// delete buffer
				if (m_output.Buffers)
				{
					for (int i = 0; i < m_output.NumBuffers; i++)
					{
						delete m_output.Buffers[i];
					}
					
					delete m_output.Buffers;
					m_output.Buffers = 0;
				}
			}
		}
		
		void CDriverXAudio2::suspend()
		{
			// todo nothing
		}
		
		void CDriverXAudio2::resume()
		{
			// todo nothing
		}
		
		void CDriverXAudio2::updateThread()
		{
			// Sleep 80% of buffer size
			int sleepTime = static_cast<DWORD> (static_cast<float> (m_output.BufferSize / 4) / static_cast<float> (m_preferedRate) * 800.0f);
			
			// Write buffer to device
			while (m_output.IsThreadRunning)
			{
				XAUDIO2_VOICE_STATE state;
				m_sourceVoice->GetState(&state);
				if (state.BuffersQueued > 1)
				{
					// wait playing
					// WaitForSingleObject( m_voiceContext.hBufferEndEvent, INFINITE );
					IThread::sleep(1);
					continue;
				}
				
				// todo update all emitter & decode stream
	#ifndef USE_MULTITHREAD_UPDATE
				SkylichtAudio::CAudioEngine::getSoundEngine()->updateEmitter();
	#endif
				
				// Get a buffer of samples
				// 2 channel u16 -> samples = size/4
				fillBuffer(reinterpret_cast<unsigned short*>(m_output.Buffers[m_output.CurrentBuffer]), m_output.BufferSize / 4);
				
				//
				// At this point we have a buffer full of audio and enough room to submit it, so
				// let's submit it and get another read request going.
				//
				XAUDIO2_BUFFER buf = { 0 };
				buf.AudioBytes = m_output.BufferSize;
				buf.pAudioData = m_output.Buffers[m_output.CurrentBuffer];
				
				// update sound data
				m_sourceVoice->SubmitSourceBuffer(&buf);
				m_sourceVoice->Start();
				
				m_output.CurrentBuffer = (m_output.CurrentBuffer + 1) % m_output.NumBuffers;
				
				//Sleep(sleepTime);
				IThread::sleep(sleepTime);
			}
		}
	}
}

#endif
