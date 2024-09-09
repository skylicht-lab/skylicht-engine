#ifndef _SKYLICHTAUDIO_IAUDIODECODERWAV_H_
#define _SKYLICHTAUDIO_IAUDIODECODERWAV_H_

#include "Stream/IStream.h"
#include "Driver/ISoundSource.h"
#include "Driver/ISoundDriver.h"

#include "IWavSubDecoder.h"
#include "IAudioDecoder.h"
#include "WavHeader.h"

namespace Skylicht
{
	namespace Audio
	{
		class CAudioDecoderWav : public IAudioDecoder
		{
		protected:
			IStreamCursor* m_streamCursor;
			SWaveChunk m_waveChunk;
			
			IWavSubDecoder* m_subDecoder;
			
		public:
			CAudioDecoderWav(IStream* stream);
			
			virtual ~CAudioDecoderWav();
			
			virtual EStatus initDecode();
			
			virtual EStatus decode(void* outputBuffer, int bufferSize);
			
			virtual int seek(int bufferSize);
			
			virtual void getTrackParam(STrackParams* track);
			
			virtual float getCurrentTime();
		};
	}
}

#endif
