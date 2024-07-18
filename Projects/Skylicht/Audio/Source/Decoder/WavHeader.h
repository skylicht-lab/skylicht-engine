#ifndef _WAVEHEADER_
#define _WAVEHEADER_

namespace Skylicht
{
	namespace Audio
	{
		enum ECompressionCode
		{
			CompressUnknown          = 0x0000,
			CompressPCM              = 0x0001,
			CompressMSADPCM          = 0x0002,
			CompressITUG_711_A_LAW   = 0x0006,
			CompressITUG_711_AMU_LAW = 0x0007,
			CompressIMAADPCM         = 0x0011,
			CompressITUG_723_ADPCM   = 0x0016,
			CompressGSM_6_10         = 0x0031,
			CompressITUG_721_ADPCM   = 0x0040,
			CompressMPEG             = 0x0050
		};
		
		enum EChunkSize
		{
			ChunkHeaderSize = 8,
			RiffChunkSize   = 12,
			FmtChunkSize    = 24,
			FactChunkSize   = 12
		};
		
		struct SRiffHeader
		{
			char               ChunkId[4];  // "RIFF"
			unsigned int	   Filesize;    // total file size -8 bytes for this header
			char               RiffType[4]; // usually "WAVE"
		};
		
		struct SFormatHeader
		{
			char               ChunkId[4];          // "fmt "
			unsigned int       ChunkDataSize;       // size of this header: 16 + extra format bytes
			unsigned short     CompressionCode;     // 0x0001 = PCM, 0x0011 = IMA ADPCM
			unsigned short     NumChannels;
			unsigned int       SampleRate;
			unsigned int       AvgBytesPerSample;
			unsigned short     BlockAlign;
			unsigned short     SignificantBitsPerSample;
		};
		
		struct SFactHeader
		{
			char               ChunkId[4];     // "fact"
			unsigned int       ChunkDataSize;  // for IMA ADPCM this is the # of samples
			unsigned int       FactData;
		};
		
		struct SDataHeader
		{
			char               ChunkId[4]; // "data"
			unsigned int       ChunkSize;  // data size including any padding
		};
		
		struct SChunkHeader
		{
			char               ChunkId[4];
			unsigned int       ChunkSize;
		};
		
		struct SDataNode
		{
			SDataNode():
			FileOffset(0),
			ByteSize(0),
			Next(0)
			{
				
			}
			
			SDataNode(int fileOffset, int byteSize):
			FileOffset(fileOffset),
			ByteSize(byteSize),
			Next(0)
			{
			}
			
			int FileOffset;
			int ByteSize;
			SDataNode* Next;
			
			void AddNode(int fileOffset, int byteSize)
			{
				if (Next)
					Next->AddNode( fileOffset, byteSize);
				else
					Next = new SDataNode(fileOffset, byteSize);
			}
			
			void DropNodes()
			{
				if (Next)
				{
					Next->DropNodes();
					delete Next;
				}
				Next = NULL;
			}
		};
		
		struct SWaveChunk
		{
			SWaveChunk():
			DataNodes(0)
			{
			}
			
			~SWaveChunk()
			{
				if (DataNodes)
				{
					DataNodes->DropNodes();
					delete DataNodes;
				}
				DataNodes = NULL;
			}
			
			SRiffHeader   RiffHeader;
			SFormatHeader FormatHeader;
			SDataHeader   DataHeader;
			SFactHeader   FactHeader;
			SDataNode*    DataNodes;
		};
	}
}

#endif
