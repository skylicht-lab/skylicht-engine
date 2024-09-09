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
#include "CDriverNull.h"
#include "Engine/CAudioEmitter.h"

namespace Skylicht
{
	namespace Audio
	{
		CDriverNull::CDriverNull()
		{
			m_mixBuffer = NULL;
			m_numMixSampler = 0;
			
			m_bufferLength = 0.0625f;
			m_preferedRate = 44100; // hz (num sampler per second)
			
			m_masterGain = 1.0f;
			
			m_mutex = IMutex::createMutex();
		}
		
		CDriverNull::~CDriverNull()
		{
			delete m_mutex;
			
			if (m_mixBuffer)
				delete m_mixBuffer;
			
			shutdown();
		}
		
		void CDriverNull::init()
		{
		}
		
		void CDriverNull::shutdown()
		{
		}
		
		void CDriverNull::suspend()
		{
		}
		
		void CDriverNull::resume()
		{
		}
		
		void CDriverNull::update()
		{
		}
		
		void CDriverNull::lockThread()
		{
			m_mutex->lock();
		}
		
		void CDriverNull::unlockThread()
		{
			m_mutex->unlock();
		}
		
		void CDriverNull::changeDuration(float duration)
		{
			m_bufferLength = duration;
			
			// loop all source
			std::vector<CSoundSource*>::iterator i = m_sources.begin(), end = m_sources.end();
			while (i != end)
			{
				(*i)->changeDuration(duration);
				++i;
			}
		}
		
		void CDriverNull::fillBuffer(unsigned short* outBuffer, int numSample)
		{
			SScopeMutex lockScope(m_mutex);
			
			int totalSamples = numSample * 2;	// left & right
			int safeBufferSize = totalSamples + totalSamples / 2;
			
			// alloc mix buffer
			if (m_numMixSampler < numSample)
			{
				m_numMixSampler = numSample;
				
				// free current mix buffer
				if (m_mixBuffer != NULL)
					delete m_mixBuffer;
				
				m_mixBuffer = new int[safeBufferSize];
				m_numMixSampler = numSample;
			}
			
			// silent audio
			memset(m_mixBuffer, 0, sizeof(int)*safeBufferSize);
			
			// mix audio
			std::vector<CSoundSource*>::iterator iSource = m_sources.begin(), sourceEnd = m_sources.end();
			while (iSource != sourceEnd)
			{
				CSoundSource *source = (*iSource);
				
				// mix playing source
				if (source->getState() == ISoundSource::StatePlaying)
				{
					source->fillBuffer(m_mixBuffer, numSample, m_masterGain);
				}
				
				++iSource;
			}
			
			// clamp audio (convert fixed to short)
			int *mixingBufferCursor = m_mixBuffer;
			short *outBufferCursor = (short*)outBuffer;
			
			for (int i = 0; i < totalSamples; i++)
			{
				if ((unsigned int)(*mixingBufferCursor + 32768) > 65535)
					*outBufferCursor = (short)(*mixingBufferCursor < 0 ? -32768 : 32767);
				else
					*outBufferCursor = (short)(*mixingBufferCursor);
				
				outBufferCursor++;
				mixingBufferCursor++;
			}
		}
		
		ISoundSource* CDriverNull::createSource()
		{
			SScopeMutex lockScope(m_mutex);
			
			CSoundSource *source = new CSoundSource(m_bufferLength);
			m_sources.push_back(source);
			return source;
		}
		
		void CDriverNull::destroyDriverSource(ISoundSource* driverSource)
		{
			SScopeMutex lockScope(m_mutex);
			
			std::vector<CSoundSource*>::iterator i = m_sources.begin(), end = m_sources.end();
			while (i != end)
			{
				if ((*i) == driverSource)
				{
					delete driverSource;
					m_sources.erase(i);
					return;
				}
				++i;
			}
		}
		
		void CDriverNull::destroyAllSource()
		{
			SScopeMutex lockScope(m_mutex);
			
			std::vector<CSoundSource*>::iterator i = m_sources.begin(), end = m_sources.end();
			while (i != end)
			{
				delete (*i);
				++i;
			}
			m_sources.clear();
		}
		
		void CDriverNull::getSourceParam(SSourceParam *source)
		{
			source->NumBuffer = 2;
			source->BufferSize = (int)(m_bufferLength * m_preferedRate);
			source->SamplingRate = m_preferedRate;
			source->BitPerSampler = 16;
		}
		
		int CDriverNull::getBufferSize()
		{
			int bufferSize = (int)(m_bufferLength * m_preferedRate);
			return bufferSize;
		}
	}
}
