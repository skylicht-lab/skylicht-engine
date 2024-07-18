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
#include "CDriverMMSystem.h"
#include "SkylichtAudio.h"

#ifdef USE_MMDRIVER

namespace Skylicht
{
	namespace Audio
	{
		CDriverMMSystem::CDriverMMSystem()
		{
			m_v5MinLatency = 10240;		// In bytes (= 5 * 512 samples). For versions 5 and earlier
			m_v6MinLatency = 14336;		// In bytes (= 7 * 512 samples). For versions 6 and later
			m_minBufferSize = 2048;
			
			m_output.CurrentBuffer = 0;
			m_output.NumBuffers = 2;
			
			init();
		}
		
		CDriverMMSystem::~CDriverMMSystem()
		{
			shutdown();
		}
		
		void CDriverMMSystem::init()
		{
			printLog("[SkylichtAudio] init driver WinMM");
			
			// Calc buffer size (byte)
			// 2 channel u16 -> 4 bytes
			m_output.BufferSize = (static_cast<int>(m_bufferLength * m_preferedRate)) * 4;
			
			// Force buffer size to the next multiple of <m_minBufferSize> samples
			m_output.BufferSize += m_minBufferSize - (m_output.BufferSize % m_minBufferSize);
			
			// Set a lower limit to buffer size
			if (m_output.BufferSize < m_minBufferSize)
				m_output.BufferSize = m_minBufferSize;
			
			// Get major version of mmsystem
			DWORD dwMajorVersion = 0;
			
			// todo old version
			// dwMajorVersion = (DWORD)(LOBYTE(LOWORD(GetVersion())));
			
			OSVERSIONINFOEX osvi;
			DWORDLONG dwlConditionMask = 0;
			int op = VER_GREATER_EQUAL;
			
			// Initialize the OSVERSIONINFOEX structure.
			ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			osvi.dwMajorVersion = 5;
			osvi.dwMinorVersion = 1;
			osvi.wServicePackMajor = 2;
			osvi.wServicePackMinor = 0;
			
			// Initialize the condition mask.
			VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
			VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
			VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, op);
			VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMINOR, op);
			
			// Perform the test.
			VerifyVersionInfo(
							  &osvi,
							  VER_MAJORVERSION | VER_MINORVERSION |
							  VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
							  dwlConditionMask);
			
			dwMajorVersion = osvi.dwMajorVersion;
			
			
			if (dwMajorVersion >= 6)
			{
				// Total buffer size must be higher or equal to 7 x 512 samples
				if (m_output.NumBuffers * m_output.BufferSize < m_v6MinLatency)
				{
					int nbExtraBytesNeeded = m_v6MinLatency - (m_output.NumBuffers * m_output.BufferSize);
					int nbExtraBuffersFloor = nbExtraBytesNeeded / m_output.BufferSize;
					m_output.NumBuffers += nbExtraBuffersFloor + ((nbExtraBytesNeeded % m_output.BufferSize) > 0);
				}
			}
			else // dwMajorVersion <= 5
			{
				// Total buffer size must be higher or equal to 5 x 512 samples
				if (m_output.NumBuffers * m_output.BufferSize < m_v5MinLatency)
				{
					int nbExtraBytesNeeded = m_v5MinLatency - (m_output.NumBuffers * m_output.BufferSize);
					int nbExtraBuffersFloor = nbExtraBytesNeeded / m_output.BufferSize;
					m_output.NumBuffers += nbExtraBuffersFloor + ((nbExtraBytesNeeded % m_output.BufferSize) > 0);
				}
			}
			
			// Create buffers
			m_output.Buffers = new unsigned char*[m_output.NumBuffers];
			
			// Init bufer
			for (int i = 0; i < m_output.NumBuffers; i++)
			{
				m_output.Buffers[i] = new unsigned char[m_output.BufferSize];
				memset(m_output.Buffers[i], 0, m_output.BufferSize);
			}
			
			// recalc length
			m_bufferLength = (m_output.BufferSize / (float)m_preferedRate) / 4.0f;
			m_output.BufferSize = getBufferSize() * 4;
			
			m_output.WaveBlocks = static_cast<WAVEHDR*> (new WAVEHDR[m_output.NumBuffers]);
			
			// Set up the WAVEFORMATEX structure.
			memset(&m_output.Waveformat, 0, sizeof(m_output.Waveformat));
			m_output.Waveformat.wFormatTag = WAVE_FORMAT_PCM;
			m_output.Waveformat.wBitsPerSample = 16;
			m_output.Waveformat.nChannels = 2;
			m_output.Waveformat.nSamplesPerSec = m_preferedRate;
			m_output.Waveformat.nBlockAlign = m_output.Waveformat.nChannels * (m_output.Waveformat.wBitsPerSample / 8);
			m_output.Waveformat.nAvgBytesPerSec = m_output.Waveformat.nSamplesPerSec * m_output.Waveformat.nBlockAlign;
			
			m_output.UpdateThread = NULL;
			
			if (waveOutOpen(&m_waveOut, WAVE_MAPPER, &m_output.Waveformat, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR)
			{
				m_output.BufferSize = 0;
				
				for (int i = 0; i < m_output.NumBuffers; i++)
				{
					delete m_output.Buffers[i];
				}
				
				delete m_output.Buffers;
				m_output.Buffers = 0;
				
				delete m_output.WaveBlocks;
				m_output.WaveBlocks = 0;
				
				printLog("[SkylichtAudio] Can not create MMSystem waveOutOpen");
				return;
			}
			
			// Initialize buffers with silence
			for (int i = 0; i < m_output.NumBuffers; i++)
			{
				memset(&m_output.WaveBlocks[i], 0, sizeof(WAVEHDR));
				m_output.WaveBlocks[i].dwBufferLength = m_output.BufferSize;
				m_output.WaveBlocks[i].lpData = (LPSTR)m_output.Buffers[i];
				waveOutPrepareHeader(m_waveOut, &m_output.WaveBlocks[i], sizeof(WAVEHDR));
				waveOutWrite(m_waveOut, &m_output.WaveBlocks[i], sizeof(WAVEHDR));
			}
			
			// start thread
			m_output.UpdateThread = IThread::createThread(this);
			m_output.IsThreadRunning = true;
		}
		
		void CDriverMMSystem::shutdown()
		{
			printLog("[SkylichtAudio] shutdown driver WinMM");
			
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
				
				waveOutReset(m_waveOut);
				
				// Close mmsystem output device
				waveOutClose(m_waveOut);
				
				// Free allocated memory
				if (m_output.WaveBlocks)
				{
					delete m_output.WaveBlocks;
					m_output.WaveBlocks = 0;
				}
				
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
		
		void CDriverMMSystem::suspend()
		{
			// todo nothing
		}
		
		void CDriverMMSystem::resume()
		{
			// todo nothing
		}
		
		void CDriverMMSystem::updateThread()
		{
			// Sleep 80% of buffer size
			int sleepTime = static_cast<DWORD> (static_cast<float> (m_output.BufferSize / 4) / static_cast<float> (m_preferedRate) * 800.0f);
			
			// Write buffer to device
			while ((m_output.WaveBlocks[m_output.CurrentBuffer].dwFlags & WHDR_DONE) && m_output.IsThreadRunning)
			{
				// todo update all emitter & decode stream
	#ifndef USE_MULTITHREAD_UPDATE
				SkylichtAudio::CAudioEngine::getSoundEngine()->updateEmitter();
	#endif
				
				if (m_output.WaveBlocks[m_output.CurrentBuffer].dwFlags & WHDR_PREPARED)
					waveOutUnprepareHeader(m_waveOut, &m_output.WaveBlocks[m_output.CurrentBuffer], sizeof(WAVEHDR));
				
				// Get a buffer of samples
				// 2 channel u16 -> samples = size/4
				fillBuffer(reinterpret_cast<unsigned short*>(m_output.Buffers[m_output.CurrentBuffer]), m_output.BufferSize / 4);
				
				// write audio
				m_output.WaveBlocks[m_output.CurrentBuffer].dwBufferLength = m_output.BufferSize;
				m_output.WaveBlocks[m_output.CurrentBuffer].lpData = (LPSTR)m_output.Buffers[m_output.CurrentBuffer];
				
				waveOutPrepareHeader(m_waveOut, &m_output.WaveBlocks[m_output.CurrentBuffer], sizeof(WAVEHDR));
				waveOutWrite(m_waveOut, &m_output.WaveBlocks[m_output.CurrentBuffer], sizeof(WAVEHDR));
				
				m_output.CurrentBuffer = (m_output.CurrentBuffer + 1) % m_output.NumBuffers;
				
				Sleep(sleepTime);
			}
		}
	}
}

#endif
