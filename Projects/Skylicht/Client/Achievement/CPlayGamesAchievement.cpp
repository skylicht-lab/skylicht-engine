#include "pch.h"
#include "CPlayGamesAchievement.h"

#ifdef ANDROID
extern "C"
{
	void playGamesAchievement_updateAchievement(const char* id, int step, float percent);
	void playGamesAchievement_fetch();
	void playGamesAchievement_showDefaultAchievementsUI();
};
#endif

extern "C" void playGamesAchievement_onFetchData(const char** ids, int* unlocks, float* percents, int* currentSteps, int* totalSteps, int count)
{
	std::vector<Skylicht::SAchievementInfo> data;
	data.reserve(count);

	for (int i = 0; i < count; i++)
	{
		Skylicht::SAchievementInfo info;
		info.Id = ids[i] != nullptr ? ids[i] : "";
		info.Unlock = unlocks[i] != 0;
		info.Percent = percents[i];
		info.CurrentSteps = currentSteps[i];
		info.TotalSteps = totalSteps[i];
		data.push_back(info);
	}

	Skylicht::CPlayGamesAchievement* achievement = Skylicht::CPlayGamesAchievement::getInstance();
	if (achievement != nullptr && achievement->OnFetchData != nullptr)
		achievement->OnFetchData(data);
}

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CPlayGamesAchievement);

	CPlayGamesAchievement::CPlayGamesAchievement()
	{
	}

	CPlayGamesAchievement::~CPlayGamesAchievement()
	{
	}

	void CPlayGamesAchievement::updateAchievement(const char* id, int step, float percent)
	{
#ifdef ANDROID
		playGamesAchievement_updateAchievement(id, step, percent);
#endif
	}

	void CPlayGamesAchievement::fetch()
	{
#ifdef ANDROID
		playGamesAchievement_fetch();
#else
		if (OnFetchData != nullptr)
			OnFetchData(std::vector<SAchievementInfo>());
#endif
	}

	void CPlayGamesAchievement::showDefaultAchievementsUI()
	{
#ifdef ANDROID
		playGamesAchievement_showDefaultAchievementsUI();
#endif
	}
}
