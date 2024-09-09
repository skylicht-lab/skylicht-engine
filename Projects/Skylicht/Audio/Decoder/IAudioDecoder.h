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

#ifndef _SKYLICHTAUDIO_IAUDIODECODER_H_
#define _SKYLICHTAUDIO_IAUDIODECODER_H_

#include "Driver/ISoundSource.h"
#include "Stream/IStream.h"

#include "AudioDebugLog.h"

namespace Skylicht
{
	namespace Audio
	{
		enum EStatus
		{
			Failed = 0,
			Success,
			WaitData,
			EndStream,
		};
		
		class IAudioDecoder
		{
		public:
			enum EDecoderType
			{
				Wav,
				Mp3,
				RawWav,	// use for micro, video audio
				Num
			};
			
		protected:
			bool m_loop;
			
			IStream* m_stream;
			
		public:
			IAudioDecoder(IStream* stream)
			{
				m_loop = false;
				m_stream = stream;
			}
			
			virtual ~IAudioDecoder()
			{
			}
			
			virtual EStatus initDecode() = 0;
			
			virtual EStatus decode(void* outputBuffer, int bufferSize) = 0;
			
			virtual float getCurrentTime() = 0;
			
			virtual int seek(int bufferSize) = 0;
			
			virtual void setLoop(bool loop)
			{
				m_loop = loop;
			}
			
			virtual bool isLoop()
			{
				return m_loop;
			}
			
			virtual void getTrackParam(STrackParams* track) = 0;
			
			virtual void stopStream()
			{
				
			}
		};
	}
}

#endif
