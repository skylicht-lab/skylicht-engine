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

#ifndef _SKYLICHTAUDIO_AUDIOEREADER_
#define _SKYLICHTAUDIO_AUDIOEREADER_

#include "Driver/ISoundSource.h"
#include "Driver/ISoundDriver.h"
#include "Decoder/IAudioDecoder.h"
#include "Thread/IMutex.h"

using namespace Skylicht::System;

namespace Skylicht
{
	namespace Audio
	{
		class CAudioReader
		{
		protected:
			bool m_isError;
			int m_initState;
			
			IAudioDecoder* m_decoder;
			ISoundDriver* m_driver;
			
			IMutex* m_mutex;
			IStream* m_stream;
			
			ISoundSource::ESourceState m_state;
			IAudioDecoder::EDecoderType m_decodeType;
			
		public:
			CAudioReader(IStream* stream, IAudioDecoder::EDecoderType type, ISoundDriver* driver);
			
			CAudioReader(const char* file, ISoundDriver* driver);
			
			virtual ~CAudioReader();
			
			void initReader();
			
			EStatus initDecoder();
			
			EStatus readAudio(unsigned char* data, int size);
			
			IAudioDecoder::EDecoderType getDecoderType()
			{
				return m_decodeType;
			}
			
			IAudioDecoder* getDecoder()
			{
				return m_decoder;
			}
			
			IStream* getStream()
			{
				return m_stream;
			}
			
			ISoundSource::ESourceState getState()
			{
				return m_state;
			}
		};
	}
}

#endif
