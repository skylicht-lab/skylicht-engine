#include "stdafx.h"
#include "CWavSubDecoderMSADPCM.h"

namespace Skylicht
{
	namespace Audio
	{
		CWavSubDecoderMSADPCM::CWavSubDecoderMSADPCM(IStreamCursor* streamCursor, SWaveChunk* waveChunks)
		:IWavSubDecoder(streamCursor, waveChunks)
		,m_readBuffer(0)
		,m_totalDataBytesRead(0)
		,m_dataStartPosition(0)
		,m_samplesInBuffer(0)
		,m_samplesInBufferConsumed(0)
		,m_totalSampleDecoded(0)
		,m_blockReadBuffer(0)
		{
			m_streamCursor->seek(RiffChunkSize + FmtChunkSize, IStreamCursor::OriginStart);
			
			// read extra data
			int numFmtExtraBytes = waveChunks->FormatHeader.ChunkDataSize - (FmtChunkSize - ChunkHeaderSize);
			m_streamCursor->read((unsigned char*)&m_fmtExtendedInfos, numFmtExtraBytes);
			
			// goto next chunk
			goToNextDataChunk();
			m_dataStartPosition = m_streamCursor->tell();
			
			// setting param
			m_trackParams.BitsPerSample = 16;						// Intended to be nb of decoded bits per sample
			m_trackParams.NumChannels	= waveChunks->FormatHeader.NumChannels;
			m_trackParams.SamplingRate	= waveChunks->FormatHeader.SampleRate;
			m_trackParams.NumSamples	= waveChunks->FactHeader.FactData;
			
			
			// read buffer
			m_readBuffer		= new unsigned char[waveChunks->FormatHeader.BlockAlign * 4];
			m_blockReadBuffer	= new unsigned char[waveChunks->FormatHeader.BlockAlign];
			
			if (m_trackParams.NumChannels <= 0 || m_trackParams.NumChannels > 2)
			{
				printLog("[SkylichtAudio] Only mono and stereo files are presently supported for MS-ADPCM format");
				m_trackParams.reset();
				return;
			}
			
			int preambleSize = MS_ADPCM_MONO_HEADER_SIZE * waveChunks->FormatHeader.NumChannels;
			
			if ( ((waveChunks->FormatHeader.BlockAlign - preambleSize) * 2) % waveChunks->FormatHeader.NumChannels != 0)
			{
				printLog("[SkylichtAudio] Block size of adpcm is not compatible with %d channels, may cause seek issues", waveChunks->FormatHeader.NumChannels);
			}
		}
		
		CWavSubDecoderMSADPCM::~CWavSubDecoderMSADPCM()
		{
			delete m_readBuffer;
			delete m_blockReadBuffer;
		}
		
		int CWavSubDecoderMSADPCM::decode(void* buffer, int size)
		{
			int nbSamplesDesired = size / (m_trackParams.NumChannels * (m_trackParams.BitsPerSample / 8));
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
				
				// If no samples available, put decoder in error
				if (m_samplesInBuffer <= 0)
				{
					m_isDecoderInError = true;
					break;
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
				
				if ((m_totalSampleDecoded >= (unsigned int)m_trackParams.NumSamples) || ((m_totalDataBytesRead >= m_waveChunks->DataHeader.ChunkSize) && (m_samplesInBufferConsumed == m_samplesInBuffer)))
				{
					if(!(m_totalSampleDecoded >= (unsigned int)m_trackParams.NumSamples))
					{
						printLog("[SkylichtAudio] Reached end of file but still waiting for samples, missing : %d", m_trackParams.NumSamples - m_totalSampleDecoded);
					}
					
					if (!m_loop)
					{
						break;
					}
					
					else // If looping, seek to beginning of stream.
					{
						if (seek(0) != 0)
							break;		//could not go back to beginning
					}
				}
			}
			
			return (nbSamplesDesired - nbSamples) * (m_trackParams.NumChannels*(m_trackParams.BitsPerSample/8));
		}
		
		int CWavSubDecoderMSADPCM::decodeBlock(void* outbuf)
		{
			int		i;
			short	*coefficient[2];
			SMsAdpcmState decoderState[2];
			SMsAdpcmState *state[2];
			
			short *decoded = static_cast<short*> (outbuf);
			
			int readSize = m_waveChunks->FormatHeader.BlockAlign <= m_waveChunks->DataHeader.ChunkSize - m_totalDataBytesRead?
			m_waveChunks->FormatHeader.BlockAlign :
			m_waveChunks->DataHeader.ChunkSize - m_totalDataBytesRead;
			
			unsigned char* readBuffer = m_blockReadBuffer;
			
			readSize = m_streamCursor->read(readBuffer, readSize);
			
			if(readSize <= 0)
				return 0;
			
			state[0] = &decoderState[0];
			if (m_trackParams.NumChannels == 2)
				state[1] = &decoderState[1];
			else
				state[1] = &decoderState[0];
			
			// Initialize predictor.
			for(i = 0; i < m_trackParams.NumChannels; i++)
			{
				state[i]->Predictor = *readBuffer++;
			}
			
			// Initialize delta.
			for(i = 0; i < m_trackParams.NumChannels; i++)
			{
				state[i]->Delta = (readBuffer[1] << 8) | readBuffer[0];
				readBuffer += sizeof(short);
			}
			
			// Initialize first two samples.
			for(i = 0; i < m_trackParams.NumChannels; i++)
			{
				state[i]->Sample1 = (readBuffer[1] << 8) | readBuffer[0];
				readBuffer += sizeof(short);
			}
			
			for (i = 0; i < m_trackParams.NumChannels; i++)
			{
				state[i]->Sample2 = (readBuffer[1] << 8) | readBuffer[0];
				readBuffer += sizeof(short);
			}
			
			coefficient[0] = m_fmtExtendedInfos.Coefficients[state[0]->Predictor];
			coefficient[1] = m_fmtExtendedInfos.Coefficients[state[1]->Predictor];
			
			for(i = 0; i < m_trackParams.NumChannels; i++)
				*decoded++ = state[i]->Sample2;
			
			for (i = 0; i < m_trackParams.NumChannels; i++)
				*decoded++ = state[i]->Sample1;
			
			int loopCount = readSize - 7 * m_waveChunks->FormatHeader.NumChannels;
			
			// The first two samples have already been 'decoded' in	the block header.
			int nbSamples = (loopCount << 1) / m_trackParams.NumChannels + 2;
			
			while(loopCount > 0)
			{
				*decoded++ = decodeSample(state[0], static_cast<unsigned int> (*readBuffer >> 4), coefficient[0]);
				*decoded++ = decodeSample(state[1], static_cast<unsigned int> (*readBuffer & 0x0f), coefficient[1]);
				
				readBuffer++;
				loopCount--;
			}
			
			m_totalDataBytesRead += readSize;
			if ((int)m_totalSampleDecoded + nbSamples > m_trackParams.NumSamples)
			{
				nbSamples = m_trackParams.NumSamples - m_totalSampleDecoded;
			}
			
			return nbSamples;
		}
		
		short CWavSubDecoderMSADPCM::decodeSample(SMsAdpcmState *state, unsigned int code, const short *coefficient)
		{
			int	linearSample, delta;
			
			linearSample = ((state->Sample1 * coefficient[0]) +	(state->Sample2 * coefficient[1])) >> 8;
			
			// Variable 'code' is a 4-bit interpreted as signed complement-2 (reason for 28-shifts and s32-cast)
			linearSample += state->Delta * ((static_cast<int> (code << 28)) >> 28);
			
			// Clamp linearSample to a signed 16-bit value.
			if (linearSample < MIN_INT16)
				linearSample = MIN_INT16;
			else if (linearSample > MAX_INT16)
				linearSample = MAX_INT16;
			
			delta = ((int) state->Delta * Adaptive[code]) >> 8;
			if(delta < 16)
			{
				delta = 16;
			}
			
			state->Delta = (short)delta;
			state->Sample2 = state->Sample1;
			state->Sample1 = (short)linearSample;
			
			// Because of earlier range checking, new_sample will be in the range of an s16.
			return static_cast<short> (linearSample);
		}
		
		int CWavSubDecoderMSADPCM::seek(int sample)
		{
			if (sample > m_trackParams.NumSamples)
			{
				if (m_loop && m_trackParams.NumSamples != 0)
					sample = sample % m_trackParams.NumSamples;
				else
					sample = m_trackParams.NumSamples;
			}
			
			int blockIndex = sample / m_fmtExtendedInfos.SamplesPerBlock;
			
			// Consider all blocks preceding the one containing the requested sample as read.
			m_totalDataBytesRead = blockIndex * m_waveChunks->FormatHeader.BlockAlign;
			
			// Seek to start of block containing requested sample.
			m_streamCursor->seek(m_dataStartPosition + m_totalDataBytesRead, IStreamCursor::OriginStart);
			
			int nbSamplesInPrecedingBlocks = blockIndex * m_fmtExtendedInfos.SamplesPerBlock;
			
			// Consider all samples before the one requested as consumed.
			m_samplesInBufferConsumed = sample - nbSamplesInPrecedingBlocks;
			
			// Set all samples before the block containing the requested sample as decoded
			m_totalSampleDecoded = nbSamplesInPrecedingBlocks;
			
			m_samplesInBuffer = decodeBlock((void*)m_readBuffer);
			
			// Set all consumed samples from current block as decoded.
			m_totalSampleDecoded += m_samplesInBufferConsumed;
			
			return 0;
		}
	}
}
