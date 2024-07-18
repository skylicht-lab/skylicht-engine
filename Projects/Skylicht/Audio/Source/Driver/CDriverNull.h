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

#ifndef _CDRIVER_NULL_
#define _CDRIVER_NULL_

#include "stdafx.h"
#include "ISoundDriver.h"
#include "CSoundSource.h"

#include "Thread/IMutex.h"

using namespace Skylicht::System;

namespace SkylichtAudio
{
	class CDriverNull : public ISoundDriver
	{
	protected:
		std::vector<CSoundSource*> m_sources;

		int* m_mixBuffer;
		int m_numMixSampler;

		unsigned char* m_buffer;
		float m_bufferLength;
		int m_preferedRate;

		IMutex* m_mutex;

		float m_masterGain;

	public:
		CDriverNull();

		virtual ~CDriverNull();

		void setMasterGain(float g)
		{
			m_masterGain = g;
		}

		float getMasterGain()
		{
			return m_masterGain;
		}

		virtual void init();

		virtual void shutdown();

		virtual void suspend();

		virtual void resume();

		virtual void update();

		virtual void lockThread();

		virtual void unlockThread();

		virtual void fillBuffer(unsigned short* outBuffer, int numSample);

		virtual ISoundSource* createSource();

		virtual void destroyDriverSource(ISoundSource* driverSource);

		virtual void destroyAllSource();

		virtual void getSourceParam(SSourceParam* source);

		virtual int getBufferSize();

		virtual void changeDuration(float duration);
	};
}

#endif
