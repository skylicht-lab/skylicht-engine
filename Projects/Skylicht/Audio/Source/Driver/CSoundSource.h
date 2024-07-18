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

#ifndef _SOUND_SOURCE_H_
#define _SOUND_SOURCE_H_

#include "stdafx.h"
#include "Thread/IMutex.h"
#include "ISoundSource.h"

using namespace Skylicht::System;

namespace SkylichtAudio
{
	class CSoundSource : public ISoundSource
	{
	public:
		CSoundSource(float length);
		virtual ~CSoundSource();

		virtual void init(const STrackParams& trackParam, const SSourceParam& driverParam);
		virtual bool needData();

		virtual void changeDuration(float duration);

		virtual void play();
		virtual void stop();
		virtual void pause();
		virtual void reset();

		virtual ESourceState getState();
		virtual void setState(ESourceState state);

		virtual long getByteOffset();
		virtual void setByteOffset(int offset);

		virtual int getBufferSize();

		virtual void uploadData(void* soundData, unsigned int bufferSize);

		virtual void lockThread();
		virtual void unlockThread();

		virtual void fillBuffer(int* buffer, int nbSample, float gain = 1.0f);

		virtual void setGain(float gain);
		virtual void setPitch(float pitch);
		virtual void setPosition(const SVector3& pos);
		virtual void setRollOff(float rollOff);
		virtual void set3DSound(bool b);

		virtual float getGain();
		virtual float getPitch();

		virtual void update(float dt);

		virtual float getBufferLength();
		virtual int getSampleRate();

	protected:
		void update3D();
		float calcDistanceGain(const SListener& listener);
		void calcLeftRightGain(const SListener& listener, float& left, float& right);
	protected:
		STrackParams m_trackParams;
		int m_numDriverBuffer;
		int m_driverBuffer;
		int m_driverBufferSize;
		int m_driverSamplingRate;

		float m_bufferDuration;
		int m_bufferSize;
		int m_bitPerSample;
		int m_numChannels;

		std::vector<SDriverBuffer> m_buffers;
		IMutex* m_mutex;

		float m_distanceGain;
		float m_leftGain;
		float m_rightGain;
		float m_gain;
		float m_pitch;

		bool m_is3DSound;
		SVector3 m_position;
		float m_rollOff;

		ISoundSource::ESourceState m_state;
	};
}

#endif
