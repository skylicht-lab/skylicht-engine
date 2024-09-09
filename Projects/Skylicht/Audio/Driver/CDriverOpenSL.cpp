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
#include "CDriverOpenSL.h"
#include "SkylichtAudio.h"

#ifdef USE_OPENSL

#define SLES_CHECKRESULT(X)

namespace Skylicht
{
	namespace Audio
	{
		CDriverOpenSL *g_driver = NULL;
		
		CDriverOpenSL::CDriverOpenSL()
		{
			m_minBufferSize = 2048;
			m_numBuffers = 2;
			m_active = false;
			
			m_buffers = NULL;
			g_driver = this;
			
			init();
		}
		
		CDriverOpenSL::~CDriverOpenSL()
		{
			g_driver = NULL;
			shutdown();
		}
		
		void CDriverOpenSL::init()
		{
			printLog("[SkylichtAudio] Init OpenSL");
			
			m_buffers = NULL;
			m_bufferSizeOSL = (static_cast<unsigned int>(m_bufferLength * m_preferedRate)) * 4;
			
			// Force buffer size to the next multiple of <m_minBufferSize> samples
			m_bufferSizeOSL += m_minBufferSize - (m_bufferSizeOSL % m_minBufferSize);
			
			if (m_bufferSizeOSL < m_minBufferSize)
				m_bufferSizeOSL = m_minBufferSize;
			
			// recalc length
			// todo: very important!
			m_bufferLength = (m_bufferSizeOSL / (float)m_preferedRate) / 4.0f;
			m_bufferSizeOSL = getBufferSize() * 4;
			
			// create engine
			SLresult result = slCreateEngine(&m_engineObject, 0, NULL, 0, NULL, NULL);
			SLuint32 state;
			SLES_CHECKRESULT(result);
			
			if (m_engineObject != NULL)
			{
				// realize the engine
				result = (*m_engineObject)->Realize(m_engineObject, SL_BOOLEAN_FALSE);
				SLES_CHECKRESULT(result);
				
				// get the engine interface, which is needed in order to create other objects
				result = (*m_engineObject)->GetInterface(m_engineObject, SL_IID_ENGINE, &m_engine);
				SLES_CHECKRESULT(result);
				
				result = (*m_engineObject)->GetState(m_engineObject, &state);
				SLES_CHECKRESULT(result);
				
				if (m_engine != NULL)
				{
					// create output mix, with environmental reverb specified as a non-required interface
					//const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
					//const SLboolean req[1] = {SL_BOOLEAN_FALSE};
					//result = (*m_engine)->CreateOutputMix(m_engine, &m_output, 1, ids, req);
					result = (*m_engine)->CreateOutputMix(m_engine, &m_output, 0, 0, 0);
					SLES_CHECKRESULT(result);
					
					if (m_output != NULL)
					{
						// realize the output mix
						result = (*m_output)->Realize(m_output, SL_BOOLEAN_FALSE);
						SLES_CHECKRESULT(result);
						
						result = (*m_output)->GetState(m_output, &state);
						SLES_CHECKRESULT(result);
					}
					else
					{
						printLog("OpenSLES: Failed to create output mixe");
						return;
					}
				}
				else
				{
					printLog("OpenSLES: Failed to get engine interface");
					return;
				}
			}
			else
			{
				printLog("OpenSLES: Failed to create object engine");
				return;
			}
			
			// Create audio player
			// configure audio source
			SLDataLocator_BufferQueue loc_bufq =
			{
				SL_DATALOCATOR_BUFFERQUEUE,
				m_numBuffers
			};
			
			SLDataFormat_PCM format_pcm =
			{
				SL_DATAFORMAT_PCM,
				2, // 2 channels
				static_cast<SLuint32>(m_preferedRate * 1000),	// samplesPerSec (milliHz)
				SL_PCMSAMPLEFORMAT_FIXED_16,
				SL_PCMSAMPLEFORMAT_FIXED_16, // bit size
				SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
				SL_BYTEORDER_LITTLEENDIAN
			};
			
			SLDataSource audioSrc = { &loc_bufq, &format_pcm };
			
			// configure audio sink
			SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, m_output };
			SLDataSink audioSnk = { &loc_outmix, NULL };
			
			// create audio player
			const SLInterfaceID ids[3] = { SL_IID_BUFFERQUEUE };
			const SLboolean req[3] = { SL_BOOLEAN_TRUE };
			
			result = (*m_engine)->CreateAudioPlayer(m_engine, &m_playerObject, &audioSrc, &audioSnk, 1, ids, req);
			SLES_CHECKRESULT(result);
			
			result = (*m_playerObject)->Realize(m_playerObject, SL_BOOLEAN_FALSE);
			SLES_CHECKRESULT(result);
			
			result = (*m_playerObject)->GetInterface(m_playerObject, SL_IID_PLAY, &m_play);
			SLES_CHECKRESULT(result);
			
			result = (*m_playerObject)->GetInterface(m_playerObject, SL_IID_BUFFERQUEUE, &m_bufferQueue);
			SLES_CHECKRESULT(result);
			
			
			// alloc buffer
			m_buffers = new unsigned char*[m_numBuffers];
			
			// init bufer
			for (int i = 0; i < m_numBuffers; i++)
			{
				m_buffers[i] = new unsigned char[m_bufferSizeOSL];
				memset(m_buffers[i], 0, m_bufferSizeOSL);
			}
			m_currentBuffer = 0;
			
			// register callback
			result = (*m_bufferQueue)->RegisterCallback(m_bufferQueue, callbackOSL, NULL);
			SLES_CHECKRESULT(result);
			
			
			// fill buffer queue;
			doCallbackOSL();
			
			//start player
			result = (*m_play)->SetPlayState(m_play, SL_PLAYSTATE_PLAYING);
			SLES_CHECKRESULT(result);
			
			m_active = true;
			
			//result = (*m_playerObject)->GetInterface(m_object, SL_IID_VOLUME, &m_volume);
			//SLES_CHECKRESULT(result);
		}
		
		void CDriverOpenSL::shutdown()
		{
			SScopeMutex sm(m_mutex);
			
			// stop player
			SLresult result = (*m_play)->SetPlayState(m_play, SL_PLAYSTATE_STOPPED);
			SLES_CHECKRESULT(result);
			
			// destroy player
			if (m_playerObject != NULL)
			{
				(*m_playerObject)->Destroy(m_playerObject);
				m_playerObject = NULL;
				m_play = NULL;
				m_bufferQueue = NULL;
			}
			
			// destroy output mix object, and invalidate all associated interfaces
			if (m_output != NULL) {
				(*m_output)->Destroy(m_output);
				m_output = NULL;
				m_output = NULL;
			}
			
			// destroy engine object, and invalidate all associated interfaces
			if (m_engineObject != NULL) {
				(*m_engineObject)->Destroy(m_engineObject);
				m_engineObject = NULL;
				m_engine = NULL;
			}
			
			// destroy buffer
			if (m_buffers != NULL)
			{
				for (int i = 0; i < m_numBuffers; i++)
					delete m_buffers[i];
				
				delete m_buffers;
				m_buffers = NULL;
			}
		}
		
		void CDriverOpenSL::suspend()
		{
			if (m_active)
			{
				SScopeMutex sm(m_mutex);
				SLresult result = (*m_play)->SetPlayState(m_play, SL_PLAYSTATE_PAUSED);
				SLES_CHECKRESULT(result);
			}
		}
		
		void CDriverOpenSL::resume()
		{
			if (m_active)
			{
				SScopeMutex sm(m_mutex);
				SLresult result = (*m_play)->SetPlayState(m_play, SL_PLAYSTATE_PLAYING);
				SLES_CHECKRESULT(result);
			}
		}
		
		void CDriverOpenSL::doCallbackOSL()
		{
			if (m_buffers == NULL)
				return;
			
			// todo update all emitter & decode stream
	#ifndef USE_MULTITHREAD_UPDATE
			SkylichtAudio::CAudioEngine::getSoundEngine()->updateEmitter();
	#endif
			
			SLBufferQueueState state;
			SLresult result = (*m_bufferQueue)->GetState(m_bufferQueue, &state);
			SLES_CHECKRESULT(result);
			
			// mix sound engine
			unsigned short *buffer = reinterpret_cast<unsigned short*>(m_buffers[m_currentBuffer]);
			fillBuffer(buffer, m_bufferSizeOSL / 4);
			
			// flush buffer to device
			result = (*m_bufferQueue)->Enqueue(m_bufferQueue, buffer, m_bufferSizeOSL);
			SLES_CHECKRESULT(result);
			
			// change buffer
			m_currentBuffer = (m_currentBuffer + 1) % m_numBuffers;
		}
		
		// callback OpenSLES
		void CDriverOpenSL::callbackOSL(SLBufferQueueItf bufferQueue, void *caller)
		{
			if (g_driver != NULL)
			{
				g_driver->doCallbackOSL();
			}
		}
	}
}

#endif
