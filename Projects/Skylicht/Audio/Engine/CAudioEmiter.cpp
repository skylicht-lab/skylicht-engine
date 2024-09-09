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
#include "Decoder/CAudioDecoderMp3.h"
#include "Decoder/CAudioDecoderRawWav.h"
#include "Engine/CAudioEngine.h"

// todo event
/*
#define EVENT_DECODE_FAILED	-1
#define EVENT_PLAYING 0
#define EVENT_STOP 1
#define EVENT_PAUSE 2
#define EVENT_ENDTRACK 3
*/

namespace Skylicht
{
	namespace Audio
	{
		IAudioDecoder::EDecoderType CAudioEmitter::getDecode(const char* fileName)
		{
			char ext[512] = { 0 };
			int len = (int)strlen(fileName);
			for (int i = len; i >= 0; i--)
			{
				if (fileName[i] == '.')
				{
					strcpy(ext, &fileName[i + 1]);
					break;
				}
			}
			
			IAudioDecoder::EDecoderType decode = IAudioDecoder::Wav;
			
			if (strcmp(ext, "mp3") == 0)
				decode = IAudioDecoder::Mp3;
			
			return decode;
		}
		
		CAudioEmitter::CAudioEmitter(IStream* stream, IAudioDecoder::EDecoderType type, ISoundDriver* driver)
		{
			initDefaultValue();
			
			m_decodeType = type;
			m_driver = driver;
			
			m_stream = stream;
			m_stream->grab();
		}
		
		CAudioEmitter::CAudioEmitter(const char* file, bool cache, ISoundDriver* driver)
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
			m_currentTime = 0.0f;
			
			m_loop = false;
			m_cache = false;
			m_forcePlay = false;
			
			m_mutex = IMutex::createMutex();
			m_state = ISoundSource::StateStopped;
			
			m_decodeType = IAudioDecoder::Mp3;
			m_driver = NULL;
			m_stream = NULL;
			m_source = NULL;
			m_decoder = NULL;
			
			m_bufferLengthTime = 0.0f;
			m_currentBuffer = 0;
			m_numBuffer = 0;
			m_decodeBuffer = NULL;
			
			m_fadeGain = 0.0f;
			m_fadeout = -1.0f;
			m_stopWithFade = -1.0f;
			m_playWithFade = -1.0f;
			
			m_init = true;
			m_initState = 0;
		}
		
		CAudioEmitter::~CAudioEmitter()
		{
			if (m_source)
				m_driver->destroyDriverSource(m_source);
			
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
			
			if (m_stream)
				m_stream->drop();
			
			delete m_mutex;
		}
		
		EStatus CAudioEmitter::initEmitter()
		{
			// state 0
			// init config & streaming
			if (m_initState == 0)
			{
				m_source = m_driver->createSource();
				
				if (m_stream == NULL && m_fileName.empty() == false)
				{
					printf("[SkylichtAudio] init emitter: %s\n", m_fileName.c_str());
					m_stream = CAudioEngine::getSoundEngine()->createStreamFromFileAndCache(m_fileName.c_str(), m_cache);
				}
				
				if (m_stream == NULL)
				{
					return Failed;
				}
				
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
				
				if (m_decoder == NULL)
					return Failed;
				
				m_currentBuffer = 0;
				m_numBuffer = 0;
				
				m_decodeBuffer = NULL;
				m_initState++;
			}
			
			// state 1
			// init decode
			if (m_initState == 1)
			{
				EStatus status = m_decoder->initDecode();
				if (status == WaitData)
					return status;
				
				// init decode
				if (status == Success)
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
					m_buffer = new unsigned char* [m_numBuffer];
					
					// calc buffer length
					m_bufferLengthTime = m_source->getBufferLength();
					
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
				
				m_initState++;
			}
			
			return Success;
		}
		
		bool CAudioEmitter::init()
		{
			SScopeMutex scopelock(m_mutex);
			
			if (m_init == true)
				return true;
			
			EStatus status = initEmitter();
			
			if (status == Success || status == Failed)
				m_init = false;
			
			return status == Success;
		}
		
		void CAudioEmitter::update()
		{
			SScopeMutex scopelock(m_mutex);
			if (m_init == true)
			{
				EStatus status = initEmitter();
				
				if (status == Success)
				{
					// ok
					m_init = false;
				}
				else if (status == Failed)
				{
					// todo event decode failed
					// CAudioEngine::getSoundEngine()->pushEvent(EVENT_DECODE_FAILED);
					
					// init decode failed
					if (m_decoder != NULL)
					{
						delete m_decoder;
						m_decoder = NULL;
					}
					m_init = false;
				}
				else
				{
					// need wait download data stream
					return;
				}
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
					
					if (m_stopWithFade > 0)
					{
						m_stopWithFade = -1.0f;
						
						// todo stop
						if (m_buffer)
						{
							for (int i = 0; i < m_numBuffer; i++)
								memset(m_buffer[i], 0, m_bufferSize);
						}
						
						if (m_decoder)
							m_decoder->seek(0);
						
						m_gain = 1.0f;
						m_state = ISoundSource::StateStopped;
						
						// todo post event stop
						// CAudioEngine::getSoundEngine()->pushEvent(EVENT_STOP);
					}
					else
					{
						m_playWithFade = -1.0f;
					}
				}
				else
				{
					if (m_stopWithFade > 0)
						m_gain = m_fadeGain * m_fadeout / m_stopWithFade;
					else
						m_gain = m_fadeGain * (1.0f - m_fadeout / m_playWithFade);
				}
			}
			
			EStatus decodeResult;
			
			// todo update emitter
			if (m_state == ISoundSource::StatePlaying)
			{
				if (m_source->needData() == true && m_decoder != NULL)
				{
					if (m_pitch == 1.0f)
					{
						// printf("m_decoder::decode %d\n", m_bufferSize);
						decodeResult = m_decoder->decode(m_buffer[m_currentBuffer], m_bufferSize);
					}
					else
					{
						int pitchSize = (int)(m_bufferSize * m_pitch);
						
						memset(m_buffer[m_currentBuffer], 0, m_bufferSize);
						
						decodeResult = m_decoder->decode(m_decodeBuffer, pitchSize);
						
						STrackParams trackParam;
						m_decoder->getTrackParam(&trackParam);
						
						int nbSample = (m_bufferSize / (trackParam.BitsPerSample / 8)) / trackParam.NumChannels;
						
						short* src = (short*)m_decodeBuffer;
						short* dest = (short*)m_buffer[m_currentBuffer];
						
						float currentSample = 0;
						
						int sample = 0;
						
						// in mono
						float pitch = pitchSize / (float)m_bufferSize;
						
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
					if (decodeResult == WaitData)
					{
						m_state = ISoundSource::StatePauseWaitData;
					}
					else if (decodeResult == Failed || decodeResult == EndStream)
					{
						// todo post event stop
						/*
						 if (decodeResult == Failed)
						 CAudioEngine::getSoundEngine()->pushEvent(EVENT_STOP);
						 else
						 CAudioEngine::getSoundEngine()->pushEvent(EVENT_ENDTRACK);
						 */
						
						m_state = ISoundSource::StateStopped;
					}
					else if (decodeResult == Success)
					{
						m_currentTime = m_currentTime + m_pitch * m_bufferLengthTime * 1000.0f;
					}
				}
			}
			else if (m_state == ISoundSource::StatePauseWaitData)
			{
				decodeResult = m_decoder->decode(m_buffer[m_currentBuffer], m_bufferSize);
				
				if (decodeResult == Success)
				{
					// change to state playing
					m_state = ISoundSource::StatePlaying;
					
					// todo
					// will seek to pause time
					STrackParams trackParam;
					m_decoder->getTrackParam(&trackParam);
					
					int seekBufferSize = (int)(trackParam.SamplingRate * (m_currentTime / 1000.0f)) * 4;
					m_decoder->seek(seekBufferSize);
					
					if (m_source)
					{
						m_source->lockThread();
						
						if (m_buffer)
						{
							for (int i = 0; i < m_numBuffer; i++)
								memset(m_buffer[i], 0, m_bufferSize);
						}
						
						m_source->unlockThread();
					}
				}
				
			}
		}
		
		void CAudioEmitter::stopWithFade(float time)
		{
			m_fadeGain = m_gain;
			m_fadeout = time;
			m_stopWithFade = time;
			m_playWithFade = -1.0f;
		}
		
		void CAudioEmitter::playWithFade(float gain, float time)
		{
			m_fadeGain = gain;
			m_gain = 0.0f;
			play();
			m_fadeout = time;
			m_playWithFade = time;
			m_stopWithFade = -1.0f;
		}
		
		void CAudioEmitter::seek(float time)
		{
			if (m_decoder == NULL && m_init == true)
				return;
			
			SScopeMutex scopelock(m_mutex);
			
			STrackParams trackParam;
			m_decoder->getTrackParam(&trackParam);
			
			int seekBufferSize = (int)(trackParam.SamplingRate * (time / 1000.0f)) * 2 * trackParam.NumChannels;
			m_decoder->seek(seekBufferSize);
			
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
			
			m_currentTime = time;
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
			
			// CAudioEngine::getSoundEngine()->pushEvent(EVENT_PLAYING);
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
			
			// CAudioEngine::getSoundEngine()->pushEvent(EVENT_PAUSE);
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
		
		void CAudioEmitter::stopStream()
		{
			m_state = ISoundSource::StateStopped;
			if (m_source)
				m_source->stop();
			if (m_decoder)
				m_decoder->stopStream();
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
}
