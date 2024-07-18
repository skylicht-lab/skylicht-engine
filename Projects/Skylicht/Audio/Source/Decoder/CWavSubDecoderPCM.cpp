#include "stdafx.h"
#include "CWavSubDecoderPCM.h"

namespace Skylicht
{
	namespace Audio
	{
		CWavSubDecoderPCM::CWavSubDecoderPCM(IStreamCursor* streamCursor, SWaveChunk* waveChunks)
		:IWavSubDecoder(streamCursor, waveChunks)
		{
			m_currentDataNode = 0;
			m_decodedSamples = 0;
			
			m_trackParams.BitsPerSample = m_waveChunks->FormatHeader.SignificantBitsPerSample;
			m_trackParams.NumChannels = m_waveChunks->FormatHeader.NumChannels;
			m_trackParams.SamplingRate = m_waveChunks->FormatHeader.SampleRate;
			m_trackParams.NumSamples = getDataSize() / (m_trackParams.NumChannels * (m_trackParams.BitsPerSample / 8));
			
			goToNextDataChunk();
		}
		
		CWavSubDecoderPCM::~CWavSubDecoderPCM()
		{
		}
		
		int CWavSubDecoderPCM::decode(void* buffer, int sizeBuffer)
		{
			if ((unsigned int)m_currentChunkPosition >= m_waveChunks->DataHeader.ChunkSize)
			{
				// end decode file
				return 0;
			}
			
			int size = 0;
			sizeBuffer -= (sizeBuffer % m_waveChunks->FormatHeader.BlockAlign);
			
			unsigned int chunkSize = m_waveChunks->DataHeader.ChunkSize;
			
			while (size < sizeBuffer)
			{
				int readsize = 0;
				
				if (chunkSize >= (unsigned int)(m_currentChunkPosition + (sizeBuffer - size)))
				{
					readsize = m_streamCursor->read(((unsigned char*)buffer) + size, (sizeBuffer - size));
					m_currentChunkPosition += readsize;
				}
				else
				{
					readsize = m_streamCursor->read(((unsigned char*)buffer) + size, chunkSize - m_currentChunkPosition);
					m_currentChunkPosition = m_waveChunks->DataHeader.ChunkSize;
				}
				
				size += readsize;
				
				m_decodedSamples += readsize / (m_trackParams.NumChannels * (m_trackParams.BitsPerSample / 8));
				
				if ((unsigned int)m_currentChunkPosition >= chunkSize && m_decodedSamples < m_trackParams.NumSamples) //end of chunk, but not data
				{
					goToNextDataChunk();
					
					if (m_waveChunks->DataHeader.ChunkSize == 0)
					{
						if (m_loop)
						{
							if (seek(0))
								break;
						}
						else
						{
							m_decodedSamples = m_trackParams.NumSamples;
							break;
						}
					}
				}
				else if (m_decodedSamples >= m_trackParams.NumSamples) // end of data
				{
					if (m_loop)
					{
						if (seek(0)) //Rewind (seek failed if return != 0)
							break;
					}
					else
					{
						break;
					}
				}
				else if (readsize == 0)
				{
					m_isDecoderInError = true;
					break;
				}
			}
			
			return size;
		}
		
		int CWavSubDecoderPCM::seek(int sampleNum)
		{
			if (sampleNum > m_trackParams.NumSamples)
			{
				if (m_loop && m_trackParams.NumSamples != 0)
					sampleNum = sampleNum % m_trackParams.NumSamples;
				else
					sampleNum = m_trackParams.NumSamples;
			}
			
			m_currentDataNode = 0;
			m_decodedSamples = 0;
			
			// reset to first chunk
			m_currentDataNode = NULL;
			goToNextDataChunk();
			
			while (sampleNum > 0 && m_currentDataNode)
			{
				if ((m_waveChunks->DataHeader.ChunkSize / m_waveChunks->FormatHeader.BlockAlign) > (unsigned int)sampleNum)
				{
					m_decodedSamples += sampleNum;
					m_streamCursor->seek(sampleNum * m_waveChunks->FormatHeader.BlockAlign, IStreamCursor::OriginCurrent);
					m_currentChunkPosition = sampleNum * m_waveChunks->FormatHeader.BlockAlign;
					sampleNum = 0;
				}
				else
				{
					m_decodedSamples += m_waveChunks->DataHeader.ChunkSize / m_waveChunks->FormatHeader.BlockAlign;
					goToNextDataChunk();
					sampleNum -= m_waveChunks->DataHeader.ChunkSize / m_waveChunks->FormatHeader.BlockAlign;
				}
			}
			
			return 0;
		}
		
		int CWavSubDecoderPCM::getDataSize()
		{
			int size = 0;
			if (m_waveChunks->DataNodes)
			{
				SDataNode* node = m_waveChunks->DataNodes;
				
				size += node->ByteSize;
				while (node->Next)
				{
					node = node->Next;
					size += node->ByteSize;
				}
			}
			
			return size;
		}
	}
}
