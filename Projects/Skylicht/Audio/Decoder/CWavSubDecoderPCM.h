#ifndef _SKYLICHTAUDIO_WAVESUBDECODER_PCM_
#define _SKYLICHTAUDIO_WAVESUBDECODER_PCM_

#include "IWavSubDecoder.h"

namespace Skylicht
{
	namespace Audio
	{
		class CWavSubDecoderPCM : public IWavSubDecoder
		{
		public:
			CWavSubDecoderPCM(IStreamCursor* streamCursor, SWaveChunk* waveChunks);
			virtual ~CWavSubDecoderPCM();
			
			virtual int decode(void* buffer, int size);
			
			virtual int seek(int size);
			
		protected:
			int getDataSize();
			
		protected:
			
			int m_decodedSamples;
		};
	}
}

#endif
