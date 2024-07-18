#ifndef _SKYLICHTAUDIO_WAVESUBDECODER_
#define _SKYLICHTAUDIO_WAVESUBDECODER_

#include "WavHeader.h"
#include "Stream/IStream.h"
#include "Driver/ISoundSource.h"
#include "AudioDebugLog.h"

namespace Skylicht
{
	namespace Audio
	{
		class IWavSubDecoder
		{
		public:
			IWavSubDecoder(IStreamCursor* streamCursor, SWaveChunk* waveChunks)
			:m_streamCursor(streamCursor),
			m_waveChunks(waveChunks),
			m_currentDataNode(0),
			m_currentChunkPosition(0),
			m_decodedSamples(0),
			m_loop(false),
			m_isDecoderInError(false)
			{
			}
			
			virtual ~IWavSubDecoder()
			{
			}
			
			const STrackParams& getTrackParams()
			{
				return m_trackParams;
			}
			
			virtual int decode(void* buffer, int size) = 0;
			
			virtual int seek(int size) = 0;
			
			inline void setLoop(bool b)
			{
				m_loop = b;
			}
			
		protected:
			IStreamCursor* m_streamCursor;
			
			SWaveChunk* m_waveChunks;
			SDataNode* m_currentDataNode;
			STrackParams m_trackParams;
			
			int m_currentChunkPosition;
			int m_decodedSamples;
			bool m_loop;
			bool m_isDecoderInError;
			
			void goToNextDataChunk(void)
			{
				if (!(m_waveChunks && m_streamCursor))
					return;
				
				if (!m_currentDataNode)
				{
					m_currentDataNode = m_waveChunks->DataNodes;
				}
				else
				{
					if (m_currentDataNode->Next)
					{
						m_currentDataNode = m_currentDataNode->Next;
					}
					else
					{
						m_currentDataNode = NULL;
						m_waveChunks->DataHeader.ChunkSize = 0;
						return;
					}
				}
				
				m_streamCursor->seek(m_currentDataNode->FileOffset + ChunkHeaderSize, IStreamCursor::OriginStart);
				m_waveChunks->DataHeader.ChunkSize = m_currentDataNode->ByteSize;
				m_currentChunkPosition = 0;
			}
		};
	}
}

#endif
