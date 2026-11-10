#include "pch.h"
#include "CPlayGamesLeaderboard.h"

#ifdef ANDROID
extern "C"
{
	void playGamesLeaderboard_postScore(const char* id, int64_t score);
	void playGamesLeaderboard_showDefaultLeaderboardUI(const char* id);
}
#endif

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CPlayGamesLeaderboard);

	CPlayGamesLeaderboard::CPlayGamesLeaderboard()
	{
	}

	CPlayGamesLeaderboard::~CPlayGamesLeaderboard()
	{
	}

	void CPlayGamesLeaderboard::postScore(const char* id, int64_t score)
	{
#ifdef ANDROID
		playGamesLeaderboard_postScore(id, score);
#endif
	}

	void CPlayGamesLeaderboard::showDefaultLeaderboardUI(const char* id)
	{
#ifdef ANDROID
		playGamesLeaderboard_showDefaultLeaderboardUI(id);
#endif
	}
}
