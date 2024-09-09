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

#ifndef _SKYLICHTAUDIO_ISTREAM_H_
#define _SKYLICHTAUDIO_ISTREAM_H_

#include "SkylichtAudioAPI.h"

namespace Skylicht
{
	namespace Audio
	{
		class IStreamCursor
		{
		public:
			enum EOrigin
			{
				OriginStart,
				OriginCurrent,
				OriginEnd
			};
		public:
			virtual ~IStreamCursor()
			{
			}
			
			virtual int seek(int pos, EOrigin origin) = 0;
			virtual int tell() = 0;
			virtual int read(unsigned char* buff, int len) = 0;
			virtual bool endOfStream() = 0;
			virtual int size() = 0;
			virtual bool readyReadData(int len) = 0;
			virtual void trim() {}
		};
		
		
		class IStream
		{
		protected:
			int	m_referenceCount;
			
			// the sampleRate & channel for stream recoder
			int m_sampleRate;
			int m_channels;
			
		public:
			IStream()
			{
				m_referenceCount = 1;
				m_channels = 2;
				m_sampleRate = 0;
			}
			
			virtual ~IStream()
			{
			}
			
			virtual IStreamCursor* createCursor() = 0;
			
			virtual void grab()
			{
				m_referenceCount++;
			}
			
			virtual bool drop()
			{
				m_referenceCount--;
				if (m_referenceCount <= 0)
				{
					delete this;
					return true;
				}
				return false;
			}
			
			int getSampleRate()
			{
				return m_sampleRate;
			}
			
			int getChannels()
			{
				return m_channels;
			}
			
			void setStreamAudio(int sampleRate, int channels)
			{
				m_sampleRate = sampleRate;
				m_channels = channels;
			}
			
			virtual void uploadData(unsigned char* buffer, int size)
			{
				
			}
			
			virtual void stopStream()
			{
				
			}
		};
	}
}

#endif
