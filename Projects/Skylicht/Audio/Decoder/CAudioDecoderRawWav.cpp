#include "stdafx.h"
#include "CAudioDecoderRawWav.h"

namespace Skylicht
{
	namespace Audio
	{
		CAudioDecoderRawWav::CAudioDecoderRawWav(IStream* stream)
		:IAudioDecoder(stream)
		{
			m_streamCursor = stream->createCursor();
		}
		
		CAudioDecoderRawWav::~CAudioDecoderRawWav()
		{
			delete m_streamCursor;
		}
		
		EStatus CAudioDecoderRawWav::initDecode()
		{
			return Success;
		}
		
		EStatus CAudioDecoderRawWav::decode(void* outputBuffer, int bufferSize)
		{
			// silent buffer
			memset(outputBuffer, 0, bufferSize);
			
			// need wait data
			// check safe data avaiable will be encode
			if (m_streamCursor->readyReadData(bufferSize) == false)
			{
				// return state wait data
				return WaitData;
			}
			
			// copy data from stream to waveBuffer
			m_streamCursor->read((unsigned char*)outputBuffer, bufferSize);
			
			// trim the readed memory
			m_streamCursor->trim();
			return Success;
		}
		
		int CAudioDecoderRawWav::seek(int bufferSize)
		{
			return 0;
		}
		
		void CAudioDecoderRawWav::getTrackParam(STrackParams* track)
		{
			track->NumChannels = m_stream->getChannels();
			track->SamplingRate = m_stream->getSampleRate();
			track->BitsPerSample = 16; // 16bit
		}
		
		float CAudioDecoderRawWav::getCurrentTime()
		{
			// implement later
			return 0.0f;
		}
		
		void CAudioDecoderRawWav::stopStream()
		{
			m_streamCursor->seek(0, IStreamCursor::OriginStart);
			m_stream->stopStream();
		}
	}
}
