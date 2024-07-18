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
#include "CAudioDecoderMp3.h"

namespace Skylicht
{
	namespace Audio
	{
		CAudioDecoderMp3::CAudioDecoderMp3(IStream* stream)
		:IAudioDecoder(stream)
		{
			m_dataInBuffer = 0;
			m_dataInBufferConsumed = 0;
			
			m_bufferSize = 16384 * 2 * 2;
			m_sampleBuffer = new unsigned char[m_bufferSize];
			m_sampleDecodeBuffer = NULL;
			
			m_streamCursor = stream->createCursor();
			
			m_initState = 0;
			m_floatSample = false;
			m_mp3Handle = NULL;
		}

		CAudioDecoderMp3::~CAudioDecoderMp3()
		{
			delete m_streamCursor;
			delete m_sampleBuffer;
			
			if (m_sampleDecodeBuffer != NULL)
				delete m_sampleDecodeBuffer;
			
			mpg123_delete(m_mp3Handle);
		}

		EStatus CAudioDecoderMp3::initDecode()
		{
			int ret;
			
			// state 0
			// init mpg123 config
			if (m_initState == 0)
			{
				m_trackParams.NumSamples = 0;
				m_trackParams.reset();
				
				// todo test init libmpeg
				m_mp3Handle = mpg123_new(NULL, &ret);
				
				// todo
				// setup params
				mpg123_param(m_mp3Handle, MPG123_VERBOSE, 4, 0);
				ret = mpg123_param(m_mp3Handle, MPG123_FLAGS, MPG123_FUZZY | MPG123_SEEKBUFFER | MPG123_GAPLESS, 0);
				if (ret != MPG123_OK)
				{
					printLog("%s\n", mpg123_plain_strerror(ret));
					return Failed;
				}
				
				ret = mpg123_param(m_mp3Handle, MPG123_INDEX_SIZE, -1, 0);
				if (ret != MPG123_OK)
				{
					printLog("%s\n", mpg123_plain_strerror(ret));
					return Failed;
				}
				
				ret = mpg123_format_none(m_mp3Handle);
				if (ret != MPG123_OK)
				{
					printLog("%s\n", mpg123_plain_strerror(ret));
					return Failed;
				}
				
				const long* rates = NULL;
				size_t nrates = 0;
				mpg123_rates(&rates, &nrates);
				for (size_t i = 0; i < nrates; i++)
				{
					ret = mpg123_format(m_mp3Handle, rates[i], MPG123_MONO | MPG123_STEREO, MPG123_ENC_FLOAT_32);
					if (ret != MPG123_OK)
					{
						printLog("%s\n", mpg123_plain_strerror(ret));
						return Failed;
					}
				}
				
				ret = mpg123_open_feed(m_mp3Handle);
				if (ret != MPG123_OK)
				{
					printLog("%s\n", mpg123_plain_strerror(ret));
					return Failed;
				}
				
				m_initState++;
			}
			/////////////////////////////////////////
			
			if (m_initState == 1)
			{
				// todo seek and read format
				off_t num;
				unsigned char* audio;
				size_t bytes;
				
				bool readFormatEnd = false;
				while (readFormatEnd == false)
				{
					// todo need wait data
					if (m_streamCursor->readyReadData(m_bufferSize) == false)
						return WaitData;
					
					// read data
					int readSize = m_streamCursor->read(m_sampleBuffer, m_bufferSize);
					
					// failed data
					if (readSize == 0)
					{
						printLog("CAudioDecoderMp3: init decoder: FAILED!\n");
						return Failed;
					}
					
					ret = mpg123_feed(m_mp3Handle, m_sampleBuffer, readSize);
					
					while (ret != MPG123_ERR && ret != MPG123_NEED_MORE)
					{
						ret = mpg123_decode_frame(m_mp3Handle, &num, &audio, &bytes);
						
						if (ret == MPG123_NEW_FORMAT)
						{
							long rate;
							int channels, enc;
							
							mpg123_getformat(m_mp3Handle, &rate, &channels, &enc);
							
							m_trackParams.NumChannels = channels;
							m_trackParams.SamplingRate = rate;
							m_trackParams.BitsPerSample = 16;
							m_trackParams.NumSamples = mpg123_length(m_mp3Handle);
							
							if (enc & MPG123_ENC_FLOAT_64)
							{
								//WAVE_FORMAT_IEEE_FLOAT; (double)
								m_bitPerSample = 64;
								m_floatSample = true;
							}
							else if (enc & MPG123_ENC_FLOAT_32)
							{
								//WAVE_FORMAT_IEEE_FLOAT; (float)
								m_bitPerSample = 32;
								m_floatSample = true;
							}
							else if (enc & MPG123_ENC_16)
							{
								//WAVE_FORMAT_PCM; (short)
								m_bitPerSample = 16;
								m_floatSample = false;
							}
							else
							{
								//WAVE_FORMAT_PCM; (byte)
								m_bitPerSample = 8;
								m_floatSample = false;
							}
							
							readFormatEnd = true;
							break;
						}
					}
					
					if (ret == MPG123_ERR)
						return Failed;
				}
				
				// reset feed
				off_t offset;
				mpg123_feedseek(m_mp3Handle, 0, SEEK_SET, &offset);
				m_streamCursor->seek(offset, IStreamCursor::OriginStart);
				
				m_initState++;
			}
			
			return Success;
		}

		EStatus CAudioDecoderMp3::decode(void* outputBuffer, int nbBytes)
		{
			memset(outputBuffer, 0, nbBytes);
			
			// need wait data
			// check safe data avaiable will be encode
			if (m_streamCursor->readyReadData(nbBytes) == false)
			{
				// return state wait data
				return WaitData;
			}
			
			
			int offset = m_dataInBuffer - m_dataInBufferConsumed;
			
			if (nbBytes <= m_dataInBufferConsumed)
			{
				// copy data in buffer
				memcpy(outputBuffer, m_sampleDecodeBuffer + offset, nbBytes);
				
				// copy
				m_dataInBufferConsumed -= nbBytes;
			}
			else
			{
				int bytesRemaining = nbBytes;
				int bytesCopyed = 0;
				
				// copy data on buffer
				if (m_dataInBufferConsumed > 0)
				{
					memcpy(outputBuffer, m_sampleDecodeBuffer + offset, m_dataInBufferConsumed);
					
					bytesRemaining -= m_dataInBufferConsumed;
					bytesCopyed = m_dataInBufferConsumed;
					
					m_dataInBufferConsumed = 0;
				}
				
				unsigned char* out = (unsigned char*)outputBuffer;
				out += bytesCopyed;
				
				while (bytesRemaining > 0)
				{
					off_t num;
					unsigned char* audio;
					size_t bytes;
					int resultDataSize = 0;
					
					int ret = mpg123_decode_frame(m_mp3Handle, &num, &audio, &bytes);
					
					if (ret == MPG123_OK && bytes > 0)
					{
						if (m_floatSample == true)
						{
							if (m_bitPerSample == 32)
							{
								int numSample = (int)(bytes / 4);
								
								resultDataSize = numSample * sizeof(short);
								resizeData(resultDataSize);
								
								convertFloat32ToShort((short*)m_sampleDecodeBuffer, (float*)audio, numSample);
							}
						}
						else
						{
							if (m_bitPerSample == 16)
							{
								resultDataSize = (int)bytes;
								resizeData(resultDataSize);
								
								memcpy(m_sampleDecodeBuffer, audio, bytes);
							}
						}
						
						m_dataInBuffer = resultDataSize;
						m_dataInBufferConsumed = resultDataSize;
						
						int copySize = bytesRemaining;
						if (copySize > m_dataInBufferConsumed)
							copySize = m_dataInBufferConsumed;
						
						memcpy(out, m_sampleDecodeBuffer, copySize);
						out += copySize;
						
						m_dataInBufferConsumed -= copySize;
						bytesRemaining -= copySize;
					}
					else if (ret == MPG123_NEED_MORE)
					{
						// todo encode mp3
						int readSize = m_streamCursor->read(m_sampleBuffer, m_bufferSize);
						if (readSize == 0)
						{
							if (m_streamCursor->endOfStream() == true)
							{
								if (m_loop == true)
								{
									if (seek(0) < 0)
										return Failed;
									return Success;
								}
								return EndStream;
							}
							else
								return Failed;
						}
						
						mpg123_feed(m_mp3Handle, m_sampleBuffer, readSize);
					}
				}
				
			}
			
			return Success;
		}

		int CAudioDecoderMp3::seek(int bufferSize)
		{
			int ret;
			int state;
			off_t inoffset;
			
			if (m_trackParams.NumChannels == 0)
				return 0;
			
			int sample = bufferSize / (m_trackParams.NumChannels * 2);
			
			printLog("CAudioDecoderMp3: Begin seek: %d\n", sample);
			
			// seek to begin
			mpg123_feedseek(m_mp3Handle, 0, SEEK_SET, &inoffset);
			m_streamCursor->seek(inoffset, IStreamCursor::OriginStart);
			
			// seek to sample
			while (ret = mpg123_feedseek(m_mp3Handle, sample, SEEK_SET, &inoffset) == MPG123_NEED_MORE)
			{
				int readSize = m_streamCursor->read(m_sampleBuffer, m_bufferSize);
				state = mpg123_feed(m_mp3Handle, m_sampleBuffer, readSize);
				printLog("CAudioDecoderMp3: Read seek data: %d\n", readSize);
				
				if (state == MPG123_ERR)
				{
					printLog("!!! Error seek: %s", mpg123_strerror(m_mp3Handle));
					return -1;
				}
			}
			
			m_streamCursor->seek(inoffset, IStreamCursor::OriginStart);
			
			m_dataInBuffer = 0;
			m_dataInBufferConsumed = 0;
			
			return 0;
		}

		float CAudioDecoderMp3::getCurrentTime()
		{
			off_t frame = 0;
			off_t frameLeft = 0;
			double sec = 0.0f;
			double secLeft = 0.0f;
			
			mpg123_position(m_mp3Handle, 0, 0, &frame, &frameLeft, &sec, &secLeft);
			
			return (float)sec * 1000.0f;
		}

		void CAudioDecoderMp3::getTrackParam(STrackParams* track)
		{
			*track = m_trackParams;
		}

		void CAudioDecoderMp3::resizeData(int size)
		{
			if (size > m_dataInBuffer)
			{
				delete m_sampleDecodeBuffer;
				m_sampleDecodeBuffer = new unsigned char[size];
			}
		}

		void CAudioDecoderMp3::convertFloat32ToShort(short* dest, float* src, int size)
		{
			int tempvalue;
			for (int i = 0; i < size; i++)
			{
				tempvalue = (int)(src[i] * 32768);
				if ((unsigned int)(tempvalue + 32768) > 65535)
					dest[i] = (short)(tempvalue < 0 ? -32768 : 32767);
				else
					dest[i] = (short)tempvalue;
			}
		}
	}
}
