#pragma once

#include "Utils/CSingleton.h"
#include "IAchievement.h"

namespace Skylicht
{
	class CGameCenterAchievement : public IAchievement
	{
	public:
		CGameCenterAchievement();

		virtual ~CGameCenterAchievement();

		DECLARE_SINGLETON(CGameCenterAchievement)

		virtual void updateAchievement(const char* id, int step, float percent);
	};
}
