#include "pch.h"
#include "ISignIn.h"

#include "CPlayGamesSignIn.h"
#include "CGameCenterSignIn.h"

namespace Skylicht
{
	ISignIn::ISignIn()
	{
	}

	ISignIn::~ISignIn()
	{
	}

	ISignIn* getOSSignIn()
	{
#if defined(IOS)
		return CGameCenterSignIn::getInstance();
#elif defined(ANDROID)
		return CPlayGamesSignIn::getInstance();
#else
		return NULL;
#endif
	}
}
