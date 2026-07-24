#pragma once

#include "Utils/CSingleton.h"
#include "IAchievement.h"

namespace Skylicht
{
	class CPlayGamesAchievement : public IAchievement
	{
	public:
		CPlayGamesAchievement();

		virtual ~CPlayGamesAchievement();

		DECLARE_SINGLETON(CPlayGamesAchievement)

		virtual void updateAchievement(const char* id, int step, float percent);

		virtual void showDefaultAchievementsUI();
	};
}
