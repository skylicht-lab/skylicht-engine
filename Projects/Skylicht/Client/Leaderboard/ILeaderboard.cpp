#include "pch.h"
#include "ILeaderboard.h"
#include "CPlayGamesLeaderboard.h"
#include "CGameCenterLeaderboard.h"

namespace Skylicht
{
	ILeaderboard* getOSLeaderboard()
	{
#if defined(IOS)
		return CGameCenterLeaderboard::createGetInstance();
#elif defined(ANDROID)
		return CPlayGamesLeaderboard::createGetInstance();
#else
		return nullptr;
#endif
	}
}
