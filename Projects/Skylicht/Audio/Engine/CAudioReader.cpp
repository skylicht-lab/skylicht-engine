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

#include "stdafx.h"
#include "CAudioReader.h"
#include "CAudioEmitter.h"

#include "Decoder/CAudioDecoderWav.h"
#include "Decoder/CAudioDecoderMp3.h"
#include "Decoder/CAudioDecoderRawWav.h"
#include "Engine/CAudioEngine.h"

namespace Skylicht
{
	namespace Audio
	{
		CAudioReader::CAudioReader(IStream* stream, IAudioDecoder::EDecoderType type, ISoundDriver* driver)
		{
			m_initState = 0;
			m_state = ISoundSource::StateInitial;
			m_isError = false;
			m_decoder = NULL;
			m_decodeType = type;
			m_driver = driver;
			m_mutex = IMutex::createMutex();
			
			m_stream = stream;
			m_stream->grab();
			
			initReader();
		}
		
		CAudioReader::CAudioReader(const char* file, ISoundDriver* driver)
		{
			m_initState = 0;
			m_state = ISoundSource::StateInitial;
			m_isError = false;
			m_decoder = NULL;
			m_decodeType = CAudioEmitter::getDecode(file);
			m_driver = driver;
			m_stream = CAudioEngine::getSoundEngine()->createStreamFromFileAndCache(file, false);
			m_mutex = IMutex::createMutex();
			
			initReader();
		}
		
		CAudioReader::~CAudioReader()
		{
			if (m_decoder != NULL)
				delete m_decoder;
			
			m_stream->drop();
			delete m_mutex;
		}
		
		void CAudioReader::initReader()
		{
			if (m_stream == NULL)
				return;
			
			switch (m_decodeType)
			{
				case IAudioDecoder::Wav:
					m_decoder = new CAudioDecoderWav(m_stream);
					break;
				case IAudioDecoder::Mp3:
					m_decoder = new CAudioDecoderMp3(m_stream);
					break;
				case IAudioDecoder::RawWav:
					m_decoder = new CAudioDecoderRawWav(m_stream);
					break;
				default:
					m_decoder = NULL;
					break;
			}
		}
		
		EStatus CAudioReader::initDecoder()
		{
			SScopeMutex scopelock(m_mutex);
			
			EStatus status = m_decoder->initDecode();
			if (status == WaitData)
				return status;
			else if (status == Failed)
			{
				m_state = ISoundSource::StateError;
				return Failed;
			}
			else if (status == Success)
			{
				m_initState++;
				m_state = ISoundSource::StatePlaying;
				return Success;
			}
			
			return status;
		}
		
		EStatus CAudioReader::readAudio(unsigned char* data, int size)
		{
			SScopeMutex scopelock(m_mutex);
			
			if (data != NULL && size > 0)
				memset(data, 0, size);
			
			if (m_state == ISoundSource::StateError ||
				m_state == ISoundSource::StateStopped)
				return Failed;
			
			EStatus status = Success;
			if (m_initState == 0)
			{
				status = m_decoder->initDecode();
				if (status == WaitData)
					return status;
				else if (status == Failed)
				{
					m_state = ISoundSource::StateError;
					return Failed;
				}
				else if (status == Success)
				{
					m_initState++;
					m_state = ISoundSource::StatePlaying;
					return Success;
				}
			}
			else if (data != NULL && size > 0)
			{
				status = m_decoder->decode(data, size);
				if (status == WaitData)
				{
					m_state = ISoundSource::StatePauseWaitData;
				}
				else if (status == Failed || status == EndStream)
				{
					// todo post event stop
					m_state = ISoundSource::StateStopped;
				}
				else if (status == Success)
				{
					// todo nothing
					m_state = ISoundSource::StatePlaying;
				}
			}
			
			return status;
		}
	}
}
