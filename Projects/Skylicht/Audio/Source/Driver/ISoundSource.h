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

#ifndef _ISOUND_SOURCE_H_
#define _ISOUND_SOURCE_H_

namespace Skylicht
{
	namespace Audio
	{
		struct SSourceParam
		{
		public:
			int SamplingRate;
			int NumBuffer;
			int BufferSize;
			int BitPerSampler;
		};
		
		
		struct STrackParams
		{
			int NumChannels;
			int SamplingRate;
			int BitsPerSample;
			int NumSamples;
			
			STrackParams() :NumChannels(0), SamplingRate(0), BitsPerSample(0), NumSamples(0)
			{
			}
			
			void reset(void)
			{
				NumChannels = 0;
				SamplingRate = 0;
				BitsPerSample = 0;
				NumSamples = 0;
			}
		};
		
		
		struct SDriverBuffer
		{
			unsigned char* Data;
			unsigned int UsedSize; // In bytes.
			unsigned int TotalSize; // In bytes.
			bool Free;
		};
		
		struct SVector3
		{
			float X;
			float Y;
			float Z;
			
			SVector3()
			{
				X = 0;
				Y = 0;
				Z = 0;
			}
		};
		
		struct SListener
		{
			SVector3 Position;
			SVector3 Up;
			SVector3 LookAt;
		};
		
		class ISoundSource
		{
		public:
			
			enum ESourceState
			{
				StateError = -1,
				StateInitial,
				StatePlaying,
				StatePause,
				StateStopped,
				StatePauseWaitData,
			};
			
		public:
			virtual ~ISoundSource()
			{
			}
			
			virtual void init(const STrackParams& trackParam, const SSourceParam& driverParam) = 0;
			virtual bool needData() = 0;
			
			virtual void play() = 0;
			virtual void stop() = 0;
			virtual void pause() = 0;
			virtual void reset() = 0;
			
			virtual ESourceState getState() = 0;
			virtual void setState(ESourceState state) = 0;
			
			virtual long getByteOffset() = 0;
			virtual void setByteOffset(int offset) = 0;
			
			virtual int getBufferSize() = 0;
			virtual void uploadData(void* soundData, unsigned int bufferSize) = 0;
			
			virtual void lockThread() = 0;
			virtual void unlockThread() = 0;
			
			virtual void setGain(float gain) = 0;
			virtual void setPitch(float pitch) = 0;
			virtual void setPosition(const SVector3& pos) = 0;
			virtual void setRollOff(float rollOff) = 0;
			virtual void set3DSound(bool b) = 0;
			
			virtual float getGain() = 0;
			virtual float getPitch() = 0;
			
			virtual void update(float dt) = 0;
			
			virtual float getBufferLength() = 0;
			virtual int getSampleRate() = 0;
		};
	}
}

#endif
