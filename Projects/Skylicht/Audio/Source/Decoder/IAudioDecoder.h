#ifndef _SKYLICHTAUDIO_IAUDIODECODER_H_
#define _SKYLICHTAUDIO_IAUDIODECODER_H_

#include "Driver/ISoundSource.h"
#include "Stream/IStream.h"

#include "AudioDebugLog.h"

namespace SkylichtAudio
{

	class IAudioDecoder
	{
	public:
		enum EDecoderType
		{
			Wav = 0,
			Mp3,
			Num
		};
	protected:
		bool m_loop;
		IStream *m_stream;

	public:
		IAudioDecoder(IStream *stream)
		{
			m_loop = false;
			m_stream = stream;
		}

		virtual ~IAudioDecoder()
		{
		}

		virtual bool initDecode() = 0;
		virtual int decode(void* outputBuffer, int bufferSize) = 0;
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

	};

}

#endif