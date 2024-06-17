#pragma once

#ifdef BUILD_SKYLICHT_AUDIO
#include "SkylichtAudio.h"
#include "Engine/CStreamFactory.h"

namespace Skylicht
{

	class SKYLICHT_API CZipAudioStreamFactory : public SkylichtAudio::CStreamFactory
	{
	public:
		virtual SkylichtAudio::IStream* createStreamFromFile(const char* fileName);
	};

}
#endif