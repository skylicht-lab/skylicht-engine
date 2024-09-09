#include "stdafx.h"
#include "CWavSubDecoderPCM.h"
#include "CWavSubDecoderMSADPCM.h"
#include "CWavSubDecoderIMAADPCM.h"
#include "CAudioDecoderWav.h"

namespace Skylicht
{
	namespace Audio
	{
		CAudioDecoderWav::CAudioDecoderWav(IStream* stream)
		:IAudioDecoder(stream)
		{
			m_streamCursor = stream->createCursor();
			m_subDecoder = NULL;
		}
		
		CAudioDecoderWav::~CAudioDecoderWav()
		{
			if (m_subDecoder)
				delete m_subDecoder;
			
			delete m_streamCursor;
		}
		
		EStatus CAudioDecoderWav::initDecode()
		{
			SChunkHeader chunkHeader;
			
			bool success = false;
			
			// wait stream download
			if (m_streamCursor->readyReadData(FmtChunkSize) == false)
				return WaitData;
			
			// todo parse wav file
			while (m_streamCursor->endOfStream() == false)
			{
				// read chunk header
				m_streamCursor->read((unsigned char*)&chunkHeader, ChunkHeaderSize);
				
				if (memcmp(chunkHeader.ChunkId, "RIFF", 4) == 0)
				{
					// copy data chunk id
					memcpy(m_waveChunk.RiffHeader.ChunkId, chunkHeader.ChunkId, 4);
					
					// copy chunk size
					m_waveChunk.RiffHeader.Filesize = chunkHeader.ChunkSize;
					
					// read riffType
					m_streamCursor->read((unsigned char*)&m_waveChunk.RiffHeader.RiffType, RiffChunkSize - ChunkHeaderSize);
				}
				else if (memcmp(chunkHeader.ChunkId, "fmt ", 4) == 0)
				{
					// copy data chunk id
					memcpy(m_waveChunk.FormatHeader.ChunkId, chunkHeader.ChunkId, 4);
					
					// copy chunk size
					m_waveChunk.FormatHeader.ChunkDataSize = chunkHeader.ChunkSize;
					
					// read format
					m_streamCursor->read((unsigned char*)&m_waveChunk.FormatHeader.CompressionCode, FmtChunkSize - ChunkHeaderSize);
					
					// seek next chunk
					if (m_waveChunk.FormatHeader.ChunkDataSize + ChunkHeaderSize > FmtChunkSize) //extended - skip
					{
						m_streamCursor->seek(m_waveChunk.FormatHeader.ChunkDataSize + ChunkHeaderSize - FmtChunkSize, IStreamCursor::OriginCurrent);
					}
					
					success = true;
				}
				else if (memcmp(chunkHeader.ChunkId, "fact", 4) == 0)
				{
					// copy data chunk id
					memcpy(m_waveChunk.FactHeader.ChunkId, chunkHeader.ChunkId, 4);
					
					// copy chunk size
					m_waveChunk.FactHeader.ChunkDataSize = chunkHeader.ChunkSize;
					
					m_streamCursor->read((unsigned char*)&m_waveChunk.FactHeader.FactData, FactChunkSize - ChunkHeaderSize);
					
					success = true;
				}
				else if (memcmp(chunkHeader.ChunkId, "data", 4) == 0)
				{
					// copy data chunk id
					memcpy(m_waveChunk.DataHeader.ChunkId, chunkHeader.ChunkId, 4);
					
					// copy chunk size
					m_waveChunk.DataHeader.ChunkSize = chunkHeader.ChunkSize;
					
					if (m_waveChunk.DataNodes)
					{
						m_waveChunk.DataNodes->AddNode(m_streamCursor->tell() - ChunkHeaderSize, m_waveChunk.DataHeader.ChunkSize);
					}
					else
					{
						m_waveChunk.DataNodes = new SDataNode(m_streamCursor->tell() - ChunkHeaderSize, m_waveChunk.DataHeader.ChunkSize);
					}
					
					m_streamCursor->seek(m_waveChunk.DataHeader.ChunkSize, IStreamCursor::OriginCurrent);
					
					success = true;
				}
				else // skip other chunk
				{
					m_streamCursor->seek(chunkHeader.ChunkSize, IStreamCursor::OriginCurrent);
				}
			}
			
			// error decode
			if (m_waveChunk.DataNodes == NULL)
			{
				printLog("[SkylichtAudio] Error Wave Chunk Data");
				success = false;
			}
			
			if (success)
			{
				if (m_waveChunk.FormatHeader.CompressionCode == CompressPCM)
				{
					printLog("[SkylichtAudio] Detect decoder CompressPCM");
					m_subDecoder = new CWavSubDecoderPCM(m_streamCursor, &m_waveChunk);
				}
				else if (m_waveChunk.FormatHeader.CompressionCode == CompressMSADPCM)
				{
					printLog("[SkylichtAudio] Detect decoder CompressMSADPCM");
					m_subDecoder = new CWavSubDecoderMSADPCM(m_streamCursor, &m_waveChunk);
				}
				else if (m_waveChunk.FormatHeader.CompressionCode == CompressIMAADPCM)
				{
					printLog("[SkylichtAudio] Detect decoder CompressIMAADPCM");
					m_subDecoder = new CWavSubDecoderIMAADPCM(m_streamCursor, &m_waveChunk);
				}
				else
				{
					printLog("[SkylichtAudio] Can not crate WAV SUBDECODE");
					success = false;
				}
			}
			
			if (success == true)
				return Success;
			
			return Failed;
		}
		
		EStatus CAudioDecoderWav::decode(void* outputBuffer, int bufferSize)
		{
			if (m_subDecoder == NULL)
				return Failed;
			
			void* decodeBuffer = NULL;
			int decodeSize = 0;
			
			int bitRate = 16;
			int numChannels = 0;
			int samplingRate = 0;
			
			if (m_subDecoder == NULL)
			{
				bitRate = m_waveChunk.FormatHeader.SignificantBitsPerSample;
				numChannels = m_waveChunk.FormatHeader.NumChannels;
				samplingRate = m_waveChunk.FormatHeader.SampleRate;
			}
			else
			{
				const STrackParams& params = m_subDecoder->getTrackParams();
				bitRate = params.BitsPerSample;
				numChannels = params.NumChannels;
				samplingRate = params.SamplingRate;
			}
			
			if (bitRate == 16)
			{
				// no need change on 16bit
				decodeBuffer = outputBuffer;
				decodeSize = bufferSize;
			}
			else
			{
				// need convert to a buffer
				int numSample = bufferSize / (numChannels * sizeof(short));
				
				decodeSize = numSample * numChannels * (bitRate / 8);
				decodeBuffer = new unsigned char[decodeSize];
			}
			
			// silent buffer
			memset(decodeBuffer, 0, decodeSize);
			
			// need wait data
			// check safe data avaiable will be encode
			if (m_streamCursor->readyReadData(decodeSize) == false)
			{
				// return state wait data
				return WaitData;
			}
			
			// update loop
			m_subDecoder->setLoop(m_loop);
			
			if (m_subDecoder->decode(decodeBuffer, decodeSize) > 0)
			{
				if (bitRate == 24)
				{
					// need convert to 16bit
					int totalSample = bufferSize / sizeof(short);
					
					unsigned char* s24 = (unsigned char*)decodeBuffer;
					short* s16 = (short*)outputBuffer;
					float f;
					int value = 0;
					
					while (totalSample > 0)
					{
						value = (int)((s24[0] | (s24[1] << 8) | (s24[2] << 16)) << 8) >> 8;
						f = value / 16777216.0f;
						
						value = (int)(f * 32768);
						if ((unsigned int)(value + 32768) > 65535)
							*s16 = (short)(value < 0 ? -32768 : 32767);
						else
							*s16 = (short)value;
						
						s24 += 3;
						s16++;
						--totalSample;
					}
					
					delete decodeBuffer;
				}
				else if (bitRate == 32)
				{
					// need convert to 32bit
					int totalSample = bufferSize / sizeof(short);
					
					int* src = (int*)decodeBuffer;
					short* dst = (short*)outputBuffer;
					
					while (totalSample > 0)
					{
						*dst = (short)((*src) >> 16);
						dst++;
						src++;
						--totalSample;
					}
					delete decodeBuffer;
				}
				
				return Success;
			}
			else
			{
				return EndStream;
			}
		}
		
		int CAudioDecoderWav::seek(int bufferSize)
		{
			if (m_subDecoder == NULL)
				return 0;
			
			STrackParams track;
			getTrackParam(&track);
			int sampleNum = bufferSize / (track.NumChannels * (track.BitsPerSample / 8));
			
			return m_subDecoder->seek(sampleNum);
		}
		
		void CAudioDecoderWav::getTrackParam(STrackParams* track)
		{
			if (m_subDecoder == NULL)
			{
				track->NumChannels = m_waveChunk.FormatHeader.NumChannels;
				track->SamplingRate = m_waveChunk.FormatHeader.SampleRate;
				track->BitsPerSample = m_waveChunk.FormatHeader.SignificantBitsPerSample;
			}
			else
			{
				*track = m_subDecoder->getTrackParams();
			}
			
			// i will convert the data to 16bit (short)
			track->BitsPerSample = 16;
		}
		
		float CAudioDecoderWav::getCurrentTime()
		{
			// implement later
			return 0.0f;
		}
	}
}
