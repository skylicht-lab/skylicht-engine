#ifndef _SKYLICHTAUDIO_IAUDIODECODER_RAW_WAV_H_
#define _SKYLICHTAUDIO_IAUDIODECODER_RAW_WAV_H_

#include "Stream/IStream.h"
#include "Driver/ISoundSource.h"
#include "Driver/ISoundDriver.h"
#include "IAudioDecoder.h"

namespace Skylicht
{
	namespace Audio
	{
		class CAudioDecoderRawWav : public IAudioDecoder
		{
		protected:
			IStreamCursor* m_streamCursor;
			
		public:
			CAudioDecoderRawWav(IStream* stream);
			
			virtual ~CAudioDecoderRawWav();
			
			virtual EStatus initDecode();
			
			virtual EStatus decode(void* outputBuffer, int bufferSize);
			
			virtual int seek(int bufferSize);
			
			virtual void getTrackParam(STrackParams* track);
			
			virtual float getCurrentTime();
			
			virtual void stopStream();
		};
	}
}

#endif
