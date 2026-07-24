#include "pch.h"
#include "CGameCenterAchievement.h"

#ifdef IOS
void gamecenter_updateAchievement(const char* id, int step, float percent);
#endif

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CGameCenterAchievement);

	CGameCenterAchievement::CGameCenterAchievement()
	{
	}

	CGameCenterAchievement::~CGameCenterAchievement()
	{
	}

	void CGameCenterAchievement::updateAchievement(const char* id, int step, float percent)
	{
#ifdef IOS
		gamecenter_updateAchievement(id, step, percent);
#endif
	}
}
