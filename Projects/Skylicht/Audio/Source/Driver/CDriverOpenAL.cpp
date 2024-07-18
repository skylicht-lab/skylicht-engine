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
#include "CDriverOpenAL.h"
#include "SkylichtAudio.h"

#ifdef USE_OPENAL

namespace Skylicht
{
	namespace Audio
	{
		CDriverOpenAL::CDriverOpenAL()
		{
			m_minBufferSize = 2048;
			m_numBuffers = 2;
			m_alSourceID = -1;
			m_isPause = false;
			
			init();
		}

		CDriverOpenAL::~CDriverOpenAL()
		{
			shutdown();
		}

		void CDriverOpenAL::init()
		{
			printLog("[SkylichtAudio] Init OPENAL");
			
			m_buffer = NULL;
			m_bufferSizeOAL = (static_cast<int>(m_bufferLength * m_preferedRate)) * 4;
			
			// Force buffer size to the next multiple of <m_minBufferSize> samples
			m_bufferSizeOAL += m_minBufferSize - (m_bufferSizeOAL % m_minBufferSize);
			
			if (m_bufferSizeOAL < m_minBufferSize)
				m_bufferSizeOAL = m_minBufferSize;
			
			// recalc length
			// todo: very important!
			m_bufferLength = (m_bufferSizeOAL / (float)m_preferedRate) / 4.0f;
			m_bufferSizeOAL = getBufferSize() * 4;
			
			// alloc buffer
			m_buffer = new unsigned char[m_bufferSizeOAL];
			memset(m_buffer, 0, m_bufferSizeOAL);
			
		#ifdef _WIN32
			//Some hw devices on windows don't work properly, try to get software
			m_device = alcOpenDevice("Generic Software");
			if (!m_device)
				m_device = alcOpenDevice(NULL);
		#else
			m_device = alcOpenDevice(NULL);
		#endif
			
			if (m_device == NULL)
			{
				printLog("[SkylichtAudio] Failed to open OPENAL device");
				return;
			}
			
			m_context = alcCreateContext(m_device, NULL);
			if (m_context == NULL)
			{
				printLog("[SkylichtAudio] Failed to create OPENAL context");
				alcCloseDevice(m_device);
				return;
			}
			
			alcMakeContextCurrent(m_context);
			
			// init open AL buffer
			alGenBuffers(NUM_ALBUFFERS, m_alBufferID);
			alGenSources(1, &m_alSourceID);
			
			// null data
			for (int iLoop = 0; iLoop < NUM_ALBUFFERS; iLoop++)
			{
				alBufferData(m_alBufferID[iLoop], AL_FORMAT_STEREO16, m_buffer, m_bufferSizeOAL, m_preferedRate);
				alSourceQueueBuffers(m_alSourceID, 1, &m_alBufferID[iLoop]);
			}
			
			alSourcePlay(m_alSourceID);
			
			// create mutex
			m_mutex = IMutex::createMutex();
			
			// create thread
			IsThreadRunning = true;
			UpdateThread = IThread::createThread(this);
			
			if (UpdateThread == NULL)
				printLog("[SkylichtAudio] Warning OPENAL will run on no-thread system!");
		}

		void CDriverOpenAL::shutdown()
		{
			// stop thread
			IsThreadRunning = false;
			if (UpdateThread)
			{
				UpdateThread->stop();
				delete UpdateThread;
				UpdateThread = NULL;
			}
			
			if (m_mutex)
			{
				delete m_mutex;
				m_mutex = NULL;
			}
			
			alSourceStop(m_alSourceID);
			alDeleteSources(1, &m_alSourceID);
			alDeleteBuffers(NUM_ALBUFFERS, m_alBufferID);
			
			alcMakeContextCurrent(NULL);
			
			if (m_context)
			{
				alcDestroyContext(m_context);
			}
			
			if (m_device)
			{
				if (!alcCloseDevice(m_device))
				{
					printLog("[SkylichtAudio] Failed to close OPENAL device");
				}
			}
			
			if (m_buffer)
			{
				delete m_buffer;
				m_buffer = NULL;
			}
		}

		void CDriverOpenAL::suspend()
		{
			if (m_isPause == false)
			{
				if (m_mutex)
					m_mutex->lock();
				
				m_isPause = true;
				
				alSourceStop(m_alSourceID);
				alcMakeContextCurrent(NULL);
				alcSuspendContext(m_context);
				
				if (m_mutex)
					m_mutex->unlock();
			}
		}

		void CDriverOpenAL::resume()
		{
			if (m_isPause == true)
			{
				if (m_mutex)
					m_mutex->lock();
				
				alcMakeContextCurrent(m_context);
				alcProcessContext(m_context);
				m_isPause = false;
				
				if (m_mutex)
					m_mutex->unlock();
			}
		}

		void CDriverOpenAL::update()
		{
			// Update OpenAL driver on no thread system (Emscripten)
			if (UpdateThread != NULL)
				return;
			
			if (m_mutex)
				m_mutex->lock();
			
			if (m_isPause == true)
			{
				if (m_mutex)
					m_mutex->unlock();
				return;
			}
			
			// play source if it stop
			ALint val;
			alGetSourcei(m_alSourceID, AL_SOURCE_STATE, &val);
			if (val != AL_PLAYING)
			{
				printLog("Restart source AL: %d - %d", m_alSourceID, val);
				alSourcePlay(m_alSourceID);
			}
			
			ALint iBuffersProcessed = 0;
			alGetSourcei(m_alSourceID, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
			
			// need wait
			if (iBuffersProcessed == 0)
			{
				if (m_mutex)
					m_mutex->unlock();
				return;
			}
			
			while (iBuffersProcessed)
			{
				// todo update all emitter & decode stream
		#ifndef USE_MULTITHREAD_UPDATE
				SkylichtAudio::CAudioEngine::getSoundEngine()->updateEmitter();
		#endif
				
				// Remove the Buffer from the Queue.  (uiBuffer contains the Buffer ID for the unqueued Buffer)
				ALuint uiBuffer = 0;
				alSourceUnqueueBuffers(m_alSourceID, 1, &uiBuffer);
				
				// Get a buffer of samples
				// 2 channel u16 -> samples = size/4
				unsigned short *buffer = reinterpret_cast<unsigned short*>(m_buffer);
				fillBuffer(buffer, m_bufferSizeOAL / 4);
				
				alBufferData(uiBuffer, AL_FORMAT_STEREO16, m_buffer, m_bufferSizeOAL, m_preferedRate);
				alSourceQueueBuffers(m_alSourceID, 1, &uiBuffer);
				
				iBuffersProcessed--;
			}
			
			if (m_mutex)
				m_mutex->unlock();
		}

		void CDriverOpenAL::updateThread()
		{
			printLog("[SkylichtAudio] Start OPENAL update thread");
			
			// Sleep 80% of buffer size
			int sleepTime = static_cast<int> (static_cast<float> (m_bufferSizeOAL / 4) / static_cast<float> (m_preferedRate) * 800.0f);
			
			// Write buffer to device
			while (IsThreadRunning)
			{
				if (m_mutex)
					m_mutex->lock();
				
				if (m_isPause == true)
				{
					if (m_mutex)
						m_mutex->unlock();
					
					IThread::sleep(1);
					continue;
				}
				
				// play source if it stop
				ALint val;
				alGetSourcei(m_alSourceID, AL_SOURCE_STATE, &val);
				if (val != AL_PLAYING)
				{
					printLog("Restart source AL: %d - %d\n", m_alSourceID, val);
					alSourcePlay(m_alSourceID);
				}
				
				ALint iBuffersProcessed = 0;
				alGetSourcei(m_alSourceID, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
				
				if (iBuffersProcessed == 0)
				{
					if (m_mutex)
						m_mutex->unlock();
					
					IThread::sleep(1);
					continue;
				}
				
				while (iBuffersProcessed)
				{
					// todo update all emitter & decode stream
		#ifndef USE_MULTITHREAD_UPDATE
					SkylichtAudio::CAudioEngine::getSoundEngine()->updateEmitter();
		#endif
					
					// Remove the Buffer from the Queue.  (uiBuffer contains the Buffer ID for the unqueued Buffer)
					ALuint uiBuffer = 0;
					alSourceUnqueueBuffers(m_alSourceID, 1, &uiBuffer);
					
					// Get a buffer of samples
					// 2 channel u16 -> samples = size/4
					unsigned short *buffer = reinterpret_cast<unsigned short*>(m_buffer);
					fillBuffer(buffer, m_bufferSizeOAL / 4);
					
					alBufferData(uiBuffer, AL_FORMAT_STEREO16, m_buffer, m_bufferSizeOAL, m_preferedRate);
					alSourceQueueBuffers(m_alSourceID, 1, &uiBuffer);
					
					iBuffersProcessed--;
				}
				
				if (m_mutex)
					m_mutex->unlock();
				
				IThread::sleep(sleepTime);
			}
		}
	}
}

#endif
