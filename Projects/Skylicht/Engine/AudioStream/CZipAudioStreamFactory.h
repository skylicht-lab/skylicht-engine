#pragma once

#ifdef BUILD_SKYLICHT_AUDIO
#include "SkylichtAudio.h"
#include "Engine/CStreamFactory.h"

namespace Skylicht
{
	class SKYLICHT_API CZipAudioStreamFactory : public Skylicht::Audio::CStreamFactory
	{
	public:
		virtual Skylicht::Audio::IStream* createStreamFromFile(const char* fileName);
	};

}
#endif
