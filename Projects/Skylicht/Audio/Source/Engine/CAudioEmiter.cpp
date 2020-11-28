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
#include "CAudioEmitter.h"
#include "Decoder/CAudioDecoderWav.h"
#include "Engine/CAudioEngine.h"

namespace SkylichtAudio
{
	IAudioDecoder::EDecoderType CAudioEmitter::getDecode(const char *fileName)
	{
		char ext[512] = { 0 };
		int len = strlen(fileName);
		for (int i = len; i >= 0; i--)
		{
			if (fileName[i] == '.')
			{
				strcpy(ext, &fileName[i + 1]);
				break;
			}
		}

		SkylichtAudio::IAudioDecoder::EDecoderType decode = SkylichtAudio::IAudioDecoder::Wav;

		if (strcmp(ext, "mp3") == 0)
			decode = SkylichtAudio::IAudioDecoder::Mp3;

		return decode;
	}

	CAudioEmitter::CAudioEmitter(IStream *stream, IAudioDecoder::EDecoderType type, ISoundDriver* driver)
	{
		initDefaultValue();

		m_decodeType = type;
		m_driver = driver;
		m_stream = stream;
	}

	CAudioEmitter::CAudioEmitter(const char *file, bool cache, ISoundDriver* driver)
	{
		initDefaultValue();

		m_fileName = file;
		m_cache = cache;

		m_decodeType = getDecode(file);
		m_driver = driver;
	}

	void CAudioEmitter::initDefaultValue()
	{
		m_pitch = 1.0f;
		m_gain = 1.0f;
		m_rollOff = 10.0f;	// 10m
		m_is3DSound = false;

		m_loop = false;
		m_cache = false;
		m_forcePlay = false;

		m_mutex = IMutex::createMutex();
		m_state = ISoundSource::StateStopped;

		m_decodeType = IAudioDecoder::Wav;
		m_driver = NULL;
		m_stream = NULL;
		m_source = NULL;
		m_decoder = NULL;

		m_currentBuffer = 0;
		m_numBuffer = 0;
		m_decodeBuffer = NULL;

		m_init = true;
	}

	CAudioEmitter::~CAudioEmitter()
	{
		if (m_source)
			m_driver->destroyDriverSource(m_source);

		// delete decode buffer
		for (int i = 0; i < m_numBuffer; i++)
		{
			if (m_buffer[i] != NULL)
				delete m_buffer[i];
		}

		if (m_decodeBuffer)
			delete m_decodeBuffer;

		if (m_numBuffer > 0)
			delete m_buffer;

		if (m_decoder)
			delete m_decoder;

		delete m_mutex;
	}

	bool CAudioEmitter::initEmitter()
	{
		m_source = m_driver->createSource();

		if (m_stream == NULL && m_fileName.empty() == false)
		{
			printLog("[SkylichtAudio] Init emitter: %s\n", m_fileName.c_str());
			m_stream = CAudioEngine::getSoundEngine()->createStreamFromFileAndCache(m_fileName.c_str(), m_cache);
		}

		if (m_stream == NULL)
		{
			return false;
		}

		switch (m_decodeType)
		{
		case IAudioDecoder::Wav:
			m_decoder = new CAudioDecoderWav(m_stream);
			break;
		case IAudioDecoder::Mp3:
			// m_decoder = new CAudioDecoderMpc(m_stream);
			break;
		default:
			m_decoder = NULL;
			break;
		}

		if (m_decoder == NULL)
			return false;

		m_currentBuffer = 0;
		m_numBuffer = 0;

		m_decodeBuffer = NULL;

		// init decode
		if (m_decoder->initDecode() == true)
		{
			// get param
			SSourceParam sourceParam;
			STrackParams trackParam;

			m_driver->getSourceParam(&sourceParam);
			m_decoder->getTrackParam(&trackParam);

			// init source
			m_source->init(trackParam, sourceParam);

			// init decode buffer
			m_numBuffer = sourceParam.NumBuffer;
			m_bufferSize = m_source->getBufferSize();
			m_buffer = new unsigned char*[m_numBuffer];
			m_allocSize = m_bufferSize;

			// init decode
			m_decodeBuffer = new unsigned char[(int)(m_bufferSize * SKYLICHTAUDIO_MAX_PITCH)];

			// silent buffer
			for (int i = 0; i < m_numBuffer; i++)
			{
				m_buffer[i] = new unsigned char[m_bufferSize];
				memset(m_buffer[i], 0, m_bufferSize);
			}

			m_state = ISoundSource::StateStopped;
		}
		else
		{
			m_driver->destroyDriverSource(m_source);

			if (m_decoder)
				delete m_decoder;

			m_source = NULL;
			m_decoder = NULL;

			m_state = ISoundSource::StateError;
		}

		return true;
	}

	void CAudioEmitter::update()
	{
		SScopeMutex scopelock(m_mutex);
		if (m_init == true)
		{
			initEmitter();
			m_init = false;
		}

		if (m_decoder == NULL || m_source == NULL)
			return;

		// sync state
		if (m_source)
		{
			m_source->setState(m_state);
			m_source->set3DSound(m_is3DSound);

			if (m_is3DSound)
			{
				m_source->setPosition(m_position);
				m_source->setRollOff(m_rollOff);
			}

		}

		// force play (fix play when source is not init)
		if (m_forcePlay == true)
		{
			m_state = ISoundSource::StatePlaying;
			m_source->play();
			m_forcePlay = false;
		}

		// sync decoder
		if (m_decoder)
			m_decoder->setLoop(m_loop);

		// todo update fadeout
		if (m_fadeout > 0.0f)
		{
			m_fadeout = m_fadeout - 1000.0f / 30.0f;
			if (m_fadeout < 0.0f)
			{
				m_fadeout = -1.0f;
				m_stopWithFade = -1.0f;

				// todo stop
				if (m_buffer)
				{
					for (int i = 0; i < m_numBuffer; i++)
						memset(m_buffer[i], 0, m_bufferSize);
				}

				if (m_decoder)
					m_decoder->seek(0);

				m_state = ISoundSource::StateStopped;
			}
			else
			{
				m_gain = m_fadeGain * m_fadeout / m_stopWithFade;
			}
		}

		// change buffer duration
		if (m_bufferSize != m_source->getBufferSize())
		{
			m_bufferSize = m_source->getBufferSize();

			if (m_allocSize < m_bufferSize)
			{
				// init decode
				delete m_decodeBuffer;
				m_decodeBuffer = new unsigned char[(int)(m_bufferSize * SKYLICHTAUDIO_MAX_PITCH)];

				// silent buffer
				for (int i = 0; i < m_numBuffer; i++)
				{
					delete m_buffer[i];
					m_buffer[i] = new unsigned char[m_bufferSize];
					memset(m_buffer[i], 0, m_bufferSize);
				}

				m_allocSize = m_bufferSize;
			}
		}

		// todo update emitter
		if (m_state == ISoundSource::StatePlaying)
		{
			if (m_source->needData() == true)
			{
				int sizeDecode = 0;

				if (m_pitch == 1.0f)
				{
					sizeDecode = m_decoder->decode(m_buffer[m_currentBuffer], m_bufferSize);
				}
				else
				{
					int pitchSize = (int)(m_bufferSize*m_pitch);

					memset(m_buffer[m_currentBuffer], 0, m_bufferSize);

					sizeDecode = m_decoder->decode(m_decodeBuffer, pitchSize);

					STrackParams trackParam;
					m_decoder->getTrackParam(&trackParam);

					int nbSample = (m_bufferSize / (trackParam.BitsPerSample / 8)) / trackParam.NumChannels;

					short *src = (short*)m_decodeBuffer;
					short *dest = (short*)m_buffer[m_currentBuffer];

					float currentSample = 0;

					int sample = 0;

					// in mono
					float pitch = sizeDecode / (float)m_bufferSize;

					float fract = 0.0f;

					if (trackParam.NumChannels == 2)
					{
						// STEREO
						for (int i = 0; i < nbSample; i++)
						{
							sample = (int)currentSample;
							fract = currentSample - sample;

							// left
							*dest = (short)(src[sample * 2] * (1.0f - fract) + src[sample * 2 + 2] * fract);
							++dest;

							// right
							*dest = (short)(src[sample * 2 + 1] * (1.0f - fract) + src[sample * 2 + 3] * fract);
							++dest;

							currentSample = i * pitch;
						}
					}
					else
					{
						// MONO
						for (int i = 0; i < nbSample; i++)
						{
							sample = (int)currentSample;
							fract = currentSample - sample;

							// mix
							*dest = (short)(src[sample] * (1.0f - fract) + src[sample + 1] * fract);
							++dest;

							currentSample = i * pitch;
						}
					}

				}

				// update data to source
				m_source->uploadData(m_buffer[m_currentBuffer], m_bufferSize);

				// update gain
				m_source->setGain(m_gain);
				m_source->setPitch(m_pitch);

				// swap buffer
				m_currentBuffer++;
				m_currentBuffer = m_currentBuffer % m_numBuffer;

				// stop
				if (sizeDecode == 0)
					m_state = ISoundSource::StateStopped;
			}
		}
	}

	void CAudioEmitter::stopWithFade(float time)
	{
		m_fadeGain = m_gain;
		m_fadeout = time;
		m_stopWithFade = time;
	}

	void CAudioEmitter::play(bool fromBegin)
	{
		m_fadeout = -1.0f;
		m_stopWithFade = -1.0f;

		SScopeMutex scopelock(m_mutex);
		m_state = ISoundSource::StatePlaying;

		if (fromBegin && m_decoder)
			m_decoder->seek(0);

		if (m_source)
			m_source->play();
		else
			m_forcePlay = true;
	}

	void CAudioEmitter::setGain(float g)
	{
		// SScopeMutex scopelock(m_mutex);
		m_gain = g;

		// clamp the gain
		if (m_gain < SKYLICHTAUDIO_MIN_GAIN)
			m_gain = SKYLICHTAUDIO_MIN_GAIN;
		if (m_gain > SKYLICHTAUDIO_MAX_GAIN)
			m_gain = SKYLICHTAUDIO_MAX_GAIN;
	}

	void CAudioEmitter::stop()
	{
		if (m_state == ISoundSource::StateStopped)
			return;

		SScopeMutex scopelock(m_mutex);

		// silent buffer
		// fix bug dirty sample
		if (m_state != ISoundSource::StateStopped && m_source)
		{
			m_source->lockThread();

			if (m_buffer)
			{
				for (int i = 0; i < m_numBuffer; i++)
					memset(m_buffer[i], 0, m_bufferSize);
			}

			if (m_decoder)
				m_decoder->seek(0);

			m_source->unlockThread();
		}

		m_state = ISoundSource::StateStopped;
		if (m_source)
			m_source->stop();
	}

	void CAudioEmitter::pause()
	{
		if (m_state == ISoundSource::StatePause)
			return;

		SScopeMutex scopelock(m_mutex);

		// silent buffer
		// fix bug dirty sample
		if (m_state != ISoundSource::StatePause && m_source)
		{
			m_source->lockThread();

			if (m_buffer)
			{
				for (int i = 0; i < m_numBuffer; i++)
					memset(m_buffer[i], 0, m_bufferSize);
			}

			m_source->unlockThread();
		}

		m_state = ISoundSource::StatePause;

		if (m_source)
			m_source->pause();
	}

	void CAudioEmitter::setPitch(float p)
	{
		// SScopeMutex scopelock(m_mutex);
		m_pitch = p;

		// clamp the pitch
		if (m_pitch < SKYLICHTAUDIO_MIN_PITCH)
			m_pitch = SKYLICHTAUDIO_MIN_PITCH;
		if (m_pitch > SKYLICHTAUDIO_MAX_PITCH)
			m_pitch = SKYLICHTAUDIO_MAX_PITCH;
	}

	void CAudioEmitter::reset()
	{
		// SScopeMutex scopelock(m_mutex);
		m_state = ISoundSource::StateStopped;
		if (m_source)
			m_source->stop();
	}

	void CAudioEmitter::setLoop(bool loop)
	{
		// SScopeMutex scopelock(m_mutex);
		m_loop = loop;
	}

	bool CAudioEmitter::isPlaying()
	{
		return m_state == ISoundSource::StatePlaying;
	}

	void CAudioEmitter::setPosition(float x, float y, float z)
	{
		// SScopeMutex scopelock(m_mutex);
		m_is3DSound = true;
		m_position.X = x;
		m_position.Y = y;
		m_position.Z = z;
	}

	void CAudioEmitter::setRollOff(float rollOff)
	{
		// SScopeMutex scopelock(m_mutex);
		m_rollOff = rollOff;
	}
}
