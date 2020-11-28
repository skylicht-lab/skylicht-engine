#include "pch.h"
#include "CZipAudioStreamFactory.h"
#include "CZipAudioFileStream.h"

namespace Skylicht
{

	SkylichtAudio::IStream* CZipAudioStreamFactory::createStreamFromFile(const char *fileName)
	{
		CZipAudioFileStream *file = new CZipAudioFileStream(fileName);

		if (file->isError() == true)
		{
			delete file;
			file = NULL;
		}

		return file;
	}


}
