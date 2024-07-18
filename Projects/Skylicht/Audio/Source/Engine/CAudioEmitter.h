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

#ifndef _SKYLICHTAUDIO_AUDIOEMITTER_
#define _SKYLICHTAUDIO_AUDIOEMITTER_

#include "Driver/ISoundSource.h"
#include "Driver/ISoundDriver.h"

#include "Decoder/IAudioDecoder.h"
#include "Thread/IMutex.h"

#define SKYLICHTAUDIO_MIN_PITCH	0.25f
#define SKYLICHTAUDIO_MAX_PITCH	4.0f

#define SKYLICHTAUDIO_MIN_GAIN	0.0f
#define SKYLICHTAUDIO_MAX_GAIN	4.0f

using namespace Skylicht::System;

namespace Skylicht
{
	namespace Audio
	{
		class CAudioEmitter
		{
		protected:
			std::string m_name;
			std::string m_fileName;
			bool m_cache;
			
			IAudioDecoder* m_decoder;
			
			IMutex* m_mutex;
			IStream* m_stream;
			ISoundSource* m_source;
			ISoundDriver* m_driver;
			
			ISoundSource::ESourceState m_state;
			IAudioDecoder::EDecoderType m_decodeType;
			
			unsigned char** m_buffer;
			unsigned char* m_decodeBuffer;
			
			float m_bufferLengthTime;
			int m_currentBuffer;
			int m_numBuffer;
			int m_bufferSize;
			
			bool m_init;
			int m_initState;
			
			float m_gain;
			bool m_loop;
			float m_pitch;
			bool m_forcePlay;
			
			bool m_is3DSound;
			
			SVector3 m_position;
			float m_rollOff;
			
			float m_fadeGain;
			float m_fadeout;
			float m_stopWithFade;
			float m_playWithFade;
			
			float m_currentTime;
		public:
			
			static IAudioDecoder::EDecoderType getDecode(const char* fileName);
			
		protected:
			
			EStatus initEmitter();
			
		public:
			CAudioEmitter(IStream* stream, IAudioDecoder::EDecoderType type, ISoundDriver* driver);
			
			CAudioEmitter(const char* file, bool cache, ISoundDriver* driver);
			
			virtual ~CAudioEmitter();
			
			void setName(const char* name)
			{
				m_name = name;
			}
			
			const char* getName()
			{
				return m_name.c_str();
			}
			
			IAudioDecoder::EDecoderType getDecoderType()
			{
				return m_decodeType;
			}
			
			IStream* getStream()
			{
				return m_stream;
			}
			
			ISoundSource::ESourceState	getState()
			{
				return m_state;
			}
			
			void initDefaultValue();
			
			bool init();
			
			virtual void update();
			
			virtual void play(bool fromBegin = true);
			virtual void stop();
			virtual void pause();
			virtual void reset();
			virtual void stopStream();
			
			virtual void seek(float time);
			
			void stopWithFade(float time);
			
			void playWithFade(float gain, float time);
			
			virtual void setLoop(bool loop);
			
			virtual bool isLoop()
			{
				return m_loop;
			}
			
			virtual void setPitch(float p);
			
			virtual float getPitch()
			{
				return m_pitch;
			}
			
			virtual void setGain(float g);
			
			virtual float getGain()
			{
				return m_gain;
			}
			
			bool isPlaying();
			
			void setPosition(float x, float y, float z);
			
			void setRollOff(float rollOff);
			
			float getCurrentTime()
			{
				return m_currentTime;
			}
		};
	}
}

#endif
