#include "pch.h"
#include "CGameCenterLeaderboard.h"
#include "GameCenterLeaderboard.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CGameCenterLeaderboard);

	CGameCenterLeaderboard::CGameCenterLeaderboard()
	{
	}

	CGameCenterLeaderboard::~CGameCenterLeaderboard()
	{
	}

	void CGameCenterLeaderboard::postScore(const char* id, int64_t score)
	{
#ifdef IOS
		gamecenter_postScore(id, score);
#endif
	}

	void CGameCenterLeaderboard::showDefaultLeaderboardUI(const char* id)
	{
#ifdef IOS
		gamecenter_showDefaultLeaderboardUI(id);
#endif
	}
}
