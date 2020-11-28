#include "stdafx.h"
#include "CWavSubDecoderPCM.h"
#include "CWavSubDecoderMSADPCM.h"
#include "CWavSubDecoderIMAADPCM.h"
#include "CAudioDecoderWav.h"

namespace SkylichtAudio
{
	CAudioDecoderWav::CAudioDecoderWav(IStream *stream)
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
				// An adpcm file is valid if a "voxu" or "voxa" chunk has been found
				if (memcmp(chunkHeader.ChunkId, "vox", 3) == 0)
				{
					// todo adpcm file
				}

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

		// silent buffer
		memset(outputBuffer, 0, bufferSize);

		// need wait data
		// check safe data avaiable will be encode
		if (m_streamCursor->readyReadData(bufferSize) == false)
		{
			// return state wait data
			return WaitData;
		}

		// update loop
		m_subDecoder->setLoop(m_loop);

		if (m_subDecoder->decode(outputBuffer, bufferSize) > 0)
			return Success;
		else
			return EndStream;
	}

	int CAudioDecoderWav::seek(int bufferSize)
	{
		if (m_subDecoder == NULL)
			return 0;

		return m_subDecoder->seek(bufferSize);
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
	}

	float CAudioDecoderWav::getCurrentTime()
	{
		// implement later
		return 0.0f;
	}
}
