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
#include "CSoundSource.h"

#include "SkylichtAudio.h"

namespace Skylicht
{
	namespace Audio
	{
		CSoundSource::CSoundSource(float length)
		{
			m_bitPerSample = 16;		// 16 bit
			m_bufferDuration = length;	// s
			m_driverBuffer = 0;
			m_state = ISoundSource::StateInitial;
			m_mutex = IMutex::createMutex();
			
			m_distanceGain = 1.0f;
			m_leftGain = 1.0f;
			m_rightGain = 1.0f;
			m_gain = 1.0f;
			m_pitch = 1.0f;
			m_rollOff = 10.0f;		// 10m
			m_is3DSound = false;
		}
		
		CSoundSource::~CSoundSource()
		{
			for (int i = 0, n = (int)m_buffers.size(); i < n; i++)
			{
				if (m_buffers[i].Data != NULL)
				{
					delete m_buffers[i].Data;
					m_buffers[i].Data = NULL;
				}
			}
			
			delete m_mutex;
		}
		
		void CSoundSource::init(const STrackParams& trackParam, const SSourceParam& driverParam)
		{
			SScopeMutex scopeLock(m_mutex);
			m_trackParams = trackParam;
			
			m_numDriverBuffer = driverParam.NumBuffer;
			m_driverBufferSize = driverParam.BufferSize;
			m_driverSamplingRate = driverParam.SamplingRate;
			
			m_bitPerSample = trackParam.BitsPerSample;
			m_numChannels = trackParam.NumChannels;
			
			m_bufferSize = (int)(m_bufferDuration * m_trackParams.SamplingRate);
			m_bufferSize = m_bufferSize * m_numChannels * m_bitPerSample / 8;
			
			// init driver buffer
			for (int i = 0; i < m_numDriverBuffer; i++)
			{
				m_buffers.push_back(SDriverBuffer());
				SDriverBuffer& buffer = m_buffers.back();
				
				buffer.Data = NULL;
				buffer.Free = true;
				buffer.TotalSize = m_bufferSize;
				buffer.UsedSize = 0;
			}
			
			m_state = ISoundSource::StateStopped;
		}
		
		void CSoundSource::changeDuration(float duration)
		{
			// recalc buffer size
			m_bufferDuration = duration;
			
			m_bufferSize = (int)(m_bufferDuration * m_trackParams.SamplingRate);
			m_bufferSize = m_bufferSize * m_numChannels * m_bitPerSample / 8;
		}
		
		int CSoundSource::getBufferSize()
		{
			return m_bufferSize;
		}
		
		bool CSoundSource::needData()
		{
			SScopeMutex scopeLock(m_mutex);
			return m_buffers[m_driverBuffer].Free;
		}
		
		void CSoundSource::play()
		{
			SScopeMutex scopeLock(m_mutex);
			if (m_state == ISoundSource::StateInitial)
				return;
			m_state = ISoundSource::StatePlaying;
		}
		
		void CSoundSource::stop()
		{
			SScopeMutex scopeLock(m_mutex);
			if (m_state == ISoundSource::StateInitial)
				return;
			m_state = ISoundSource::StateStopped;
		}
		
		void CSoundSource::pause()
		{
			SScopeMutex scopeLock(m_mutex);
			if (m_state == ISoundSource::StateInitial)
				return;
			m_state = ISoundSource::StatePause;
		}
		
		void CSoundSource::reset()
		{
			SScopeMutex scopeLock(m_mutex);
			if (m_state == ISoundSource::StateInitial)
				return;
			m_state = ISoundSource::StateStopped;
		}
		
		ISoundSource::ESourceState CSoundSource::getState()
		{
			return m_state;
		}
		
		void CSoundSource::setState(ESourceState state)
		{
			m_state = state;
		}
		
		long CSoundSource::getByteOffset()
		{
			return 0;
		}
		
		void CSoundSource::setByteOffset(int offset)
		{
		}
		
		void CSoundSource::uploadData(void* soundData, unsigned int bufferSize)
		{
			SScopeMutex scopeLock(m_mutex);
			
			if (m_buffers[m_driverBuffer].Data == NULL)
			{
				// alloc new
				m_buffers[m_driverBuffer].Data = new unsigned char[bufferSize];
				memset(m_buffers[m_driverBuffer].Data, 0, bufferSize);
			}
			else
			{
				if (m_buffers[m_driverBuffer].TotalSize < bufferSize)
				{
					delete m_buffers[m_driverBuffer].Data;
					
					// re-alloc new size
					m_buffers[m_driverBuffer].Data = new unsigned char[bufferSize];
					memset(m_buffers[m_driverBuffer].Data, 0, bufferSize);
				}
			}
			
			// copy data
			memcpy(m_buffers[m_driverBuffer].Data, soundData, bufferSize);
			
			m_buffers[m_driverBuffer].UsedSize = bufferSize;
			m_buffers[m_driverBuffer].TotalSize = bufferSize;
			m_buffers[m_driverBuffer].Free = false;
		}
		
		void CSoundSource::lockThread()
		{
			m_mutex->lock();
		}
		
		void CSoundSource::unlockThread()
		{
			m_mutex->unlock();
		}
		
		void CSoundSource::fillBuffer(int* buffer, int nbSample, float gain)
		{
			SScopeMutex scopeLock(m_mutex);
			
			short* sourceBuffer = (short*)m_buffers[m_driverBuffer].Data;
			
			if (sourceBuffer == NULL || m_trackParams.BitsPerSample != 16)
				return;
			
			m_distanceGain = 1.0f;
			m_leftGain = 1.0f;
			m_rightGain = 1.0f;
			
			if (m_is3DSound)
			{
				// calc left, right, distance gain
				update3D();
			}
			
			float rateRatio = m_trackParams.SamplingRate / (float)m_driverSamplingRate;
			
			float pitch = rateRatio;
			float currentSample = 0;
			
			int sample = 0;
			
			float leftGain = m_leftGain * m_gain * m_distanceGain * gain;
			float rightGain = m_rightGain * m_gain * m_distanceGain * gain;
			
			int sampleValue = 0;
			float fract = 0.0f;
			
			// do not need mix
			if (m_gain > 0.0f && m_distanceGain > 0.0f && m_pitch >= SKYLICHTAUDIO_MIN_PITCH && m_pitch <= SKYLICHTAUDIO_MAX_PITCH)
			{
				if (m_trackParams.NumChannels == 2)
				{
					// Mix STEREO
					for (int i = 0; i < nbSample; ++i)
					{
						sample = (int)currentSample;
						fract = currentSample - sample;
						
						// left
						sampleValue = (int)(sourceBuffer[sample * 2] * (1.0f - fract) + sourceBuffer[sample * 2 + 2] * fract);
						*buffer += (int)(sampleValue * leftGain);
						++buffer;
						
						// right
						sampleValue = (int)(sourceBuffer[sample * 2 + 1] * (1.0f - fract) + sourceBuffer[sample * 2 + 3] * fract);
						*buffer += (int)(sampleValue * rightGain);
						++buffer;
						
						currentSample = i * pitch;
					}
				}
				else
				{
					// Mix MONO
					for (int i = 0; i < nbSample; i++)
					{
						sample = (int)currentSample;
						fract = currentSample - sample;
						
						sampleValue = (int)(sourceBuffer[sample] * (1.0f - fract) + sourceBuffer[sample + 1] * fract);
						
						// left
						*buffer += (int)(sampleValue * leftGain);
						++buffer;
						
						// right
						*buffer += (int)(sampleValue * rightGain);
						++buffer;
						
						currentSample = i * pitch;
					}
				}
			}
			
			// begin to upload data
			m_buffers[m_driverBuffer].Free = true;
			
			// swap buffer
			m_driverBuffer++;
			m_driverBuffer %= m_numDriverBuffer;
		}
		
		void CSoundSource::setGain(float gain)
		{
			m_gain = gain;
		}
		
		void CSoundSource::setPitch(float pitch)
		{
			m_pitch = pitch;
		}
		
		void CSoundSource::setPosition(const SVector3& pos)
		{
			m_position = pos;
		}
		
		void CSoundSource::setRollOff(float rollOff)
		{
			m_rollOff = rollOff;
		}
		
		void CSoundSource::set3DSound(bool b)
		{
			m_is3DSound = b;
		}
		
		float CSoundSource::getGain()
		{
			return m_gain;
		}
		
		float CSoundSource::getPitch()
		{
			return m_pitch;
		}
		
		void CSoundSource::update(float dt)
		{
		}
		
		float CSoundSource::getBufferLength()
		{
			return m_bufferDuration;
		}
		
		int CSoundSource::getSampleRate()
		{
			return m_trackParams.SamplingRate;
		}
		
		void CSoundSource::update3D()
		{
			// calc distance gain
			const SListener& listener = CAudioEngine::getSoundEngine()->getListener();
			
			m_distanceGain = calcDistanceGain(listener);
			calcLeftRightGain(listener, m_leftGain, m_rightGain);
		}
		
		float CSoundSource::calcDistanceGain(const SListener& listener)
		{
			SVector3 dis;
			dis.X = m_position.X - listener.Position.X;
			dis.Y = m_position.Y - listener.Position.Y;
			dis.Z = m_position.Z - listener.Position.Z;
			
			float distance = sqrtf(dis.X * dis.X + dis.Y * dis.Y + dis.Z * dis.Z);
			
			if (distance == 0.0f)
				distance = 0.1f;
			if (m_rollOff == 0.0f)
				m_rollOff = 0.1f;
			
			float gain = 1.0f - distance / m_rollOff;
			
			if (gain < 0.0f)
				gain = 0.0f;
			if (gain > 1.0f)
				gain = 1.0f;
			
			return gain;
		}
		
		void CSoundSource::calcLeftRightGain(const SListener& listener, float& left, float& right)
		{
			float soundX = m_position.X - listener.Position.X;
			float soundY = m_position.Y - listener.Position.Y;
			float soundZ = m_position.Z - listener.Position.Z;
			
			// Calculate the norm of the source (relative) position vector.
			float  soundNorm = sqrtf(soundX * soundX + soundY * soundY + soundZ * soundZ);
			
			// Calculate vector product between 'at' and 'up' vectors
			float  rightX = listener.LookAt.Y * listener.Up.Z -
			listener.LookAt.Z * listener.Up.Y;
			float  rightY = listener.LookAt.Z * listener.Up.X -
			listener.LookAt.X * listener.Up.Z;
			float  rightZ = listener.LookAt.X * listener.Up.Y -
			listener.LookAt.Y * listener.Up.X;
			
			// Calculate the norm of the ('at' x 'up') vector product.
			float rightNormal = sqrtf(rightX * rightX + rightY * rightY + rightZ * rightZ);
			
			float centerRatio = 0.0f;
			
			if (soundNorm > 0.0f && rightNormal > 0.0f)
			{
				// Normalize source (relative) position
				soundX /= soundNorm;
				soundY /= soundNorm;
				soundZ /= soundNorm;
				
				// Normalize ('at' x 'up') vector product.
				rightX /= rightNormal;
				rightY /= rightNormal;
				rightZ /= rightNormal;
				
				// dot = |a|*|b|*cos(x)
				// dot = cos(x) with |a| = |b| = 1.0
				// cos(x) = right
				
				// and cos(x) = right/h	(in triangle)
				// => right = cos(x) with h = 1.0
				//
				//            sound
				//             (*)     look
				//              **     (*)
				//              * *     *
				//              *  * h  *
				//              *   *   *
				//              *    *  *
				//              *   x * *
				// (*)*********(*)*k***(*)Pos
				// right  centerRatio
				//
				//
				// center ratio has value from -1 to 1
				centerRatio = soundX * rightX + soundY * rightY + soundZ * rightZ;
			}
			
			float power = 1.0f;
			
			left = sqrtf(((power - 0.5f) * centerRatio) + 0.5f);
			right = sqrtf(1.0f - left * left);
		}
	}
}
