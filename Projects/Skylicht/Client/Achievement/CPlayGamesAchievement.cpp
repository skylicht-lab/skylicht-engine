#include "pch.h"
#include "CPlayGamesAchievement.h"

#ifdef ANDROID
extern "C"
{
	void playGamesAchievement_updateAchievement(const char* id, int step, float percent);
};
#endif

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
}
