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

#ifndef _SKYLICHTAUDIO_IAUDIODECODERMP3_H_
#define _SKYLICHTAUDIO_IAUDIODECODERMP3_H_

#include "Stream/IStream.h"
#include "Driver/ISoundSource.h"
#include "Driver/ISoundDriver.h"

#include "IAudioDecoder.h"

// library mp3 mpg123
#include "mpg123.h"


namespace Skylicht
{
	namespace Audio
	{
		class CAudioDecoderMp3 : public IAudioDecoder
		{
		protected:
			IStreamCursor* m_streamCursor;
			
			bool m_floatSample;
			int m_bitPerSample;
			
			mpg123_handle* m_mp3Handle;
			int m_initState;
			
		public:
			CAudioDecoderMp3(IStream* stream);
			virtual ~CAudioDecoderMp3();
			
			virtual EStatus initDecode();
			virtual EStatus decode(void* outputBuffer, int bufferSize);
			virtual float getCurrentTime();
			virtual int seek(int bufferSize);
			
			virtual void getTrackParam(STrackParams* track);
			
		protected:
			
			void resizeData(int size);
			
			void convertFloat32ToShort(short* dest, float* src, int size);
			
		protected:
			STrackParams m_trackParams;
			
			int m_dataInBuffer;
			int m_dataInBufferConsumed;
			
			int m_bufferSize;
			
			unsigned char* m_sampleBuffer;
			unsigned char* m_sampleDecodeBuffer;
		};
	}
}

#endif
