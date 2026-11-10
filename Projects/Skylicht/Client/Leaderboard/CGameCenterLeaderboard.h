#pragma once

#include "Utils/CSingleton.h"
#include "ILeaderboard.h"

namespace Skylicht
{
	class CGameCenterLeaderboard : public ILeaderboard
	{
	public:
		CGameCenterLeaderboard();
		virtual ~CGameCenterLeaderboard();

		DECLARE_SINGLETON(CGameCenterLeaderboard)

		virtual void postScore(const char* id, int64_t score) override;
		virtual void showDefaultLeaderboardUI(const char* id = nullptr) override;
	};
}
