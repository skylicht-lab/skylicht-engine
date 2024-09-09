#include "stdafx.h"
#include "CWavSubDecoderIMAADPCM.h"

#if defined(__sparc__) || defined(__sun__)
#define __BIG_ENDIAN__
#endif

namespace Skylicht
{
	namespace Audio
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////
		const char CWavSubDecoderIMAADPCM::s_adpcmIndexTable[CWavSubDecoderIMAADPCM::ADPCM_NUM_INDEX_IN_TABLE] =
		{
			-1, -1, -1, -1, 2, 4, 6, 8,
			-1, -1, -1, -1, 2, 4, 6, 8,
		};
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
		const short CWavSubDecoderIMAADPCM::s_adpcmStepSizeTable[CWavSubDecoderIMAADPCM::ADPCM_INDEX_NUM] =
		{
			7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
			19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
			50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
			130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
			337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
			876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
			2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
			5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
			15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
		};
		
		CWavSubDecoderIMAADPCM::CWavSubDecoderIMAADPCM(IStreamCursor* streamCursor, SWaveChunk* waveChunks)
		:IWavSubDecoder(streamCursor, waveChunks)
		, m_readBuffer(0)
		, m_totalDataBytesRead(0)
		, m_samplesInBuffer(0)
		, m_samplesInBufferConsumed(0)
		, m_totalSampleDecoded(0)
		, m_blockReadBuffer(0)
		, m_extraReadBuffer(0)
		, m_readBufferSpecialAlignment(0)
		, m_extraReadBufferSpecialAlignment(0)
		, m_extraBlockReadBuffer(0)
		, m_samplesInExtraBuffer(0)
		, m_samplesInExtraBufferConsumed(0)
		{
			goToNextDataChunk();
			
			m_dataStartPosition = m_streamCursor->tell();
			
			m_readBuffer = new unsigned char[m_waveChunks->FormatHeader.BlockAlign * 4];
			m_blockReadBuffer = new unsigned char[m_waveChunks->FormatHeader.BlockAlign];
			
			unsigned int preambleSize = sizeof(CAdpcmState)*m_waveChunks->FormatHeader.NumChannels;
			
			if (((m_waveChunks->FormatHeader.BlockAlign - preambleSize) << 1) % m_waveChunks->FormatHeader.NumChannels != 0)
			{
				printLog("[SkylichtAudio] Block size of adpcm is not compatible with %d channels, may cause seek issues", m_waveChunks->FormatHeader.NumChannels);
			}
			
			if (m_waveChunks->FormatHeader.NumChannels != 0)
			{
				m_samplesPerBlock = ((m_waveChunks->FormatHeader.BlockAlign - preambleSize) << 1) / m_waveChunks->FormatHeader.NumChannels + 1;
			}
			else
			{
				m_trackParams.reset();
				return;
			}
			
			m_trackParams.BitsPerSample = 16;
			m_trackParams.NumChannels = m_waveChunks->FormatHeader.NumChannels;
			m_trackParams.SamplingRate = m_waveChunks->FormatHeader.SampleRate;
			m_trackParams.NumSamples = m_waveChunks->FactHeader.FactData;
			
			if (m_trackParams.NumChannels > 8)
				m_trackParams.reset();
		}
		
		CWavSubDecoderIMAADPCM::~CWavSubDecoderIMAADPCM()
		{
			delete m_readBuffer;
			delete m_blockReadBuffer;
		}
		
		int CWavSubDecoderIMAADPCM::decode(void* buffer, int size)
		{
			int nbSamplesDesired = size / (m_trackParams.NumChannels * (m_trackParams.BitsPerSample >> 3));
			int nbSamples = nbSamplesDesired;
			int bufferoffset;
			int sampleAvailable;
			int nbSamplesToCopy;
			
			while (nbSamples > 0)
			{
				// Get a new decoded block if all samples have been consumed.
				if (m_samplesInBufferConsumed == m_samplesInBuffer)
				{
					m_samplesInBuffer = decodeBlock((void*)m_readBuffer);
					m_samplesInBufferConsumed = 0;
				}
				
				// Set write offset in output buffer.
				bufferoffset = (nbSamplesDesired - nbSamples) * m_trackParams.NumChannels;
				
				// Calculate the number of samples available in the current decoded block.
				sampleAvailable = m_samplesInBuffer - m_samplesInBufferConsumed;
				
				// Calculate the number of samples to copy from the decoded block to the output buffer.
				nbSamplesToCopy = (sampleAvailable > nbSamples) ? nbSamples : sampleAvailable;
				
				memcpy(&((short*)buffer)[bufferoffset], &(((short*)m_readBuffer)[m_samplesInBufferConsumed * m_trackParams.NumChannels]), nbSamplesToCopy * m_trackParams.NumChannels * sizeof(short));
				
				nbSamples -= nbSamplesToCopy;
				m_samplesInBufferConsumed += nbSamplesToCopy;
				m_totalSampleDecoded += nbSamplesToCopy;
				
				if (((int)m_totalSampleDecoded >= m_trackParams.NumSamples) || ((m_totalDataBytesRead >= m_waveChunks->DataHeader.ChunkSize) && (m_samplesInBufferConsumed == m_samplesInBuffer)))
				{
					if (!((int)m_totalSampleDecoded >= m_trackParams.NumSamples))
					{
						printLog("[SkylichtAudio] Reached end of file but still waiting for samples, missing : %d", m_trackParams.NumSamples - m_totalSampleDecoded);
					}
					if (!m_loop)
					{
						return 0;
					}
					else // If looping, seek to beginning of stream.
					{
						if (seek(0) != 0)
							break;		//could not go back to beginning
					}
				}
			}
			
			return (nbSamplesDesired - nbSamples)*(m_trackParams.NumChannels * (m_trackParams.BitsPerSample >> 3));
		}
		
		int CWavSubDecoderIMAADPCM::seek(int sample)
		{
			if (sample > m_trackParams.NumSamples)
			{
				printLog("%s", "Decoder seek failed : position is outside stream");
				return -1;
			}
			
			if (sample < m_trackParams.NumSamples)
			{
				unsigned int blockIndex = sample / m_samplesPerBlock;
				
				// Consider all blocks preceding the one containing the requested sample as read.
				m_totalDataBytesRead = blockIndex * m_waveChunks->FormatHeader.BlockAlign;
				
				// Seek to start of block containing requested sample.
				m_streamCursor->seek(m_dataStartPosition + m_totalDataBytesRead, IStreamCursor::OriginStart);
				
				unsigned int nbSamplesInPrecedingBlocks = blockIndex * m_samplesPerBlock;
				
				// Consider all samples before the one requested as consumed.
				m_samplesInBufferConsumed = sample - nbSamplesInPrecedingBlocks;
				m_samplesInExtraBufferConsumed = 0;
				
				// Set all samples before the block containing the requested sample as decoded
				m_totalSampleDecoded = nbSamplesInPrecedingBlocks;
				
				
				m_samplesInBuffer = decodeBlock((void*)m_readBuffer);
				
				// Set all consumed samples from current block as decoded.
				m_totalSampleDecoded += m_samplesInBufferConsumed;
				
				return 0;
			}
			return -1;
		}
		
		int CWavSubDecoderIMAADPCM::decodeBlock(void* buffer)
		{
			unsigned int readSize = m_waveChunks->FormatHeader.BlockAlign <= m_waveChunks->DataHeader.ChunkSize - m_totalDataBytesRead ?
			m_waveChunks->FormatHeader.BlockAlign :
			m_waveChunks->DataHeader.ChunkSize - m_totalDataBytesRead;
			
			unsigned char* readBuffer = m_blockReadBuffer;
			
			readSize = m_streamCursor->read(readBuffer, readSize);
			
			CAdpcmState * curState = (CAdpcmState*)readBuffer;
			
			m_states[0] = *curState;
			
	#if __BIG_ENDIAN__
			m_states[0].convertToBE();
	#endif
			for (unsigned int i = 1; i < m_waveChunks->FormatHeader.NumChannels; i++)
			{
				++curState;
				m_states[i] = *curState;
				
	#if __BIG_ENDIAN__
				m_states[i].convertToBE();
	#endif
			}
			
			// prepare destination
			short* dst[8];
			dst[0] = (short*)buffer;
			for (unsigned int i = 1; i < m_waveChunks->FormatHeader.NumChannels; i++)
			{
				dst[i] = dst[i - 1] + 1;
			}
			
			unsigned char* c = readBuffer;
			
			// process data
			for (unsigned int i = 0; i < m_waveChunks->FormatHeader.NumChannels; i++)
			{
				*dst[i] = m_states[i].PrevSample;
				dst[i] += m_waveChunks->FormatHeader.NumChannels;
			}
			
			// skip our read pointer over the block preambles
			unsigned int preambleSize = sizeof(CAdpcmState) * m_waveChunks->FormatHeader.NumChannels;
			c += preambleSize;
			
			unsigned int nbSamples = 1;
			unsigned int data;
			
			for (unsigned int i = 0; i < (readSize - preambleSize);)
			{
				for (unsigned int j = 0; j < m_waveChunks->FormatHeader.NumChannels; j++)
				{
					data = (unsigned char)c[0] + ((unsigned char)c[1] << 8) + ((unsigned char)c[2] << 16) + ((unsigned char)c[3] << 24);
					
					// decode adpcm
					decodeAdpcm(data, &(m_states[j]), dst[j], m_waveChunks->FormatHeader.NumChannels, 8);
					
					dst[j] += 8 * m_waveChunks->FormatHeader.NumChannels;
					i += 4;
					c += 4;
				}
				nbSamples += 8;
			}
			
			m_totalDataBytesRead += readSize;
			
			if (m_totalSampleDecoded + nbSamples > (unsigned int)m_trackParams.NumSamples)
			{
				nbSamples = m_trackParams.NumSamples - m_totalSampleDecoded;
			}
			
			return (int)nbSamples;
		}
		
		void CWavSubDecoderIMAADPCM::decodeAdpcm(int code, CAdpcmState* state, short* output, int offset, int chunk)
		{
			int step;
			int sample;
			unsigned char index;
			int d;
			
			//code &= 0xFF;
			int _code = code & 0x0F;
			
			sample = state->PrevSample;
			index = state->PrevIndex;
			
			for (int i = 0; i < chunk; i++)
			{
				step = s_adpcmStepSizeTable[index];
				
				
				d = step >> 3;
				if (_code & 4) d += step;
				if (_code & 2) d += step >> 1;
				if (_code & 1) d += step >> 2;
				
				if (_code & 8)
				{
					sample -= d;
					if (sample < -32768)
					{
						sample = -32768;
					}
				}
				else
				{
					sample += d;
					
					if (sample > 32767)
					{
						sample = 32767;
					}
				}
				
				
				index += s_adpcmIndexTable[_code];
				
				if (index > 127)
				{
					index = 0;
				}
				else if (index > ADPCM_INDEX_NUM - 1)
				{
					index = ADPCM_INDEX_NUM - 1;
				}
				
				code = code >> 4;
				_code = code & 0x0F;
				*output = sample;
				output += offset;
			}
			
			state->PrevSample = ((short)sample);
			state->PrevIndex = ((unsigned char)index);
		}
	}
}
