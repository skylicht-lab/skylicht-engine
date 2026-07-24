#include "pch.h"
#include "IAchievement.h"

#include "CPlayGamesAchievement.h"
#include "CGameCenterAchievement.h"

namespace Skylicht
{
	IAchievement::IAchievement()
	{
	}

	IAchievement::~IAchievement()
	{
	}

	IAchievement* getOSAchievement()
	{
#if defined(IOS)
		return CGameCenterAchievement::createGetInstance();
#elif defined(ANDROID)
		return CPlayGamesAchievement::createGetInstance();
#else
		return NULL;
#endif
	}
}
