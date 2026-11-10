#pragma once

#include "Utils/CSingleton.h"
#include "ILeaderboard.h"

namespace Skylicht
{
	class CPlayGamesLeaderboard : public ILeaderboard
	{
	public:
		CPlayGamesLeaderboard();
		virtual ~CPlayGamesLeaderboard();

		DECLARE_SINGLETON(CPlayGamesLeaderboard)

		virtual void postScore(const char* id, int64_t score) override;
		virtual void showDefaultLeaderboardUI(const char* id = nullptr) override;
	};
}
