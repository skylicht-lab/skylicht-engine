#ifndef _SKYLICHTAUDIO_WAVESUBDECODER_MSADPCM_
#define _SKYLICHTAUDIO_WAVESUBDECODER_MSADPCM_

#include "IWavSubDecoder.h"

#define MAX_INT16 32767
#define MIN_INT16 -32768
#define MS_ADPCM_MONO_HEADER_SIZE	7
#define MS_ADPCM_STEREO_HEADER_SIZE	14

namespace Skylicht
{
	namespace Audio
	{
		const int Adaptive[] =
		{
			230, 230, 230, 230, 307, 409, 512, 614,
			768, 614, 512, 409, 307, 230, 230, 230
		};
		
		const int Adaptive16[] =
		{
			230, 230, 230, 230, 307, 409, 512, 614,
			768, 614, 512, 409, 307, 230, 230, 230
		};
		
		struct SMsAdpcmState
		{
			unsigned char Predictor;
			unsigned short Delta;
			short Sample1, Sample2;
		};
		
		
		struct SFmtExtendedInfos
		{
			short Size;
			short SamplesPerBlock;
			short NumCoefficients;
			short Coefficients[256][2];
			
			SFmtExtendedInfos() :
			Size(0),
			SamplesPerBlock(0),
			NumCoefficients(0)
			{
			}
		};
		
		class CWavSubDecoderMSADPCM : public IWavSubDecoder
		{
		public:
			CWavSubDecoderMSADPCM(IStreamCursor* streamCursor, SWaveChunk* waveChunks);
			virtual ~CWavSubDecoderMSADPCM();
			
			virtual int decode(void* buffer, int size);
			
			virtual int seek(int size);
			
		protected:
			int decodeBlock(void* outbuf);
			
			short decodeSample(SMsAdpcmState* state, unsigned int code, const short* coefficient);
			
		protected:
			
			unsigned char* m_readBuffer;
			unsigned int m_totalDataBytesRead;
			int	m_dataStartPosition;
			int m_samplesInBuffer;
			int m_samplesInBufferConsumed;
			unsigned int m_totalSampleDecoded;
			unsigned char* m_blockReadBuffer;
			
			SFmtExtendedInfos m_fmtExtendedInfos;
			
		};
	}
}

#endif
