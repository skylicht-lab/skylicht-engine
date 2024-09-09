#ifndef _SKYLICHTAUDIO_WAVESUBDECODER_IMAADPCM_
#define _SKYLICHTAUDIO_WAVESUBDECODER_IMAADPCM_

#include "IWavSubDecoder.h"

namespace Skylicht
{
	namespace Audio
	{
		class CAdpcmState
		{
		public:
			short PrevSample;
			unsigned char PrevIndex;
			unsigned char Padding;
			
			CAdpcmState(CAdpcmState& state)
			{
			};
			
		public:
			CAdpcmState()
			{
			}
			
			~CAdpcmState()
			{
			}
			
			inline void setPrevSample(short sample);
			
			inline short getPrevSample() const;
			
			inline void setPrevIndex(unsigned char index);
			
			inline unsigned char setPrevIndex() const;
			
			inline void convertToBE();
		};
		
		void CAdpcmState::setPrevSample(short sample)
		{
			PrevSample = sample;
		}
		
		short CAdpcmState::getPrevSample() const
		{
			return PrevSample;
		}
		
		void CAdpcmState::setPrevIndex(unsigned char index)
		{
			PrevIndex = index;
		}
		
		unsigned char CAdpcmState::setPrevIndex() const
		{
			return PrevIndex;
		}
		
		void CAdpcmState::convertToBE()
		{
			unsigned short value = (unsigned short)PrevSample;
			PrevSample = (value << 8) | (value >> 8);
		}
		
		
		class CWavSubDecoderIMAADPCM : public IWavSubDecoder
		{
		protected:
			enum
			{
				MTAG_MCAS = (0x5341434D),
				MTAG_MCAW = (0x5741434D),
				ADPCM_INDEX_NUM = 89,
				ADPCM_NUM_INDEX_IN_TABLE = 16,
				STATIC_BUFFER_SIZE = 1024
			};
			
			static const char s_adpcmIndexTable[ADPCM_NUM_INDEX_IN_TABLE];
			static const short s_adpcmStepSizeTable[ADPCM_INDEX_NUM];
			
		public:
			CWavSubDecoderIMAADPCM(IStreamCursor* streamCursor, SWaveChunk* waveChunks);
			
			virtual ~CWavSubDecoderIMAADPCM();
			
			virtual int decode(void* buffer, int size);
			
			virtual int seek(int size);
			
			int decodeBlock(void* buffer);
			
			void decodeAdpcm(int code, CAdpcmState* state, short* output, int offset = 1, int chunk = 2);
			
		protected:
			
			CAdpcmState m_states[8];
			
			unsigned char* m_readBuffer;
			
			unsigned int m_samplesPerBlock;
			unsigned int m_totalDataBytesRead;
			int m_dataStartPosition;
			
			int m_samplesInBuffer;
			int m_samplesInBufferConsumed;
			unsigned int m_totalSampleDecoded;
			
			unsigned char* m_blockReadBuffer;
			
			unsigned char* m_extraReadBuffer;
			unsigned char* m_readBufferSpecialAlignment;
			unsigned char* m_extraReadBufferSpecialAlignment;
			unsigned char* m_extraBlockReadBuffer;
			int m_samplesInExtraBuffer;
			int m_samplesInExtraBufferConsumed;
		};
	}
}

#endif
