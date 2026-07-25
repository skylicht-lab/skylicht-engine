#pragma once

#include <functional>
#include <string>
#include <vector>

namespace Skylicht
{
	struct SAchievementInfo
	{
		std::string Id;
		bool Unlock;
		float Percent;
		int CurrentSteps;
		int TotalSteps;
	};

	class IAchievement
	{
	public:
		std::function<void(const std::vector<SAchievementInfo>& data)> OnFetchData;

	public:
		IAchievement();

		virtual ~IAchievement();

		// percent is in range 0.0f to 100.0f.
		virtual void updateAchievement(const char* id, int step, float percent) = 0;

		virtual void fetch() = 0;

		virtual void showDefaultAchievementsUI() = 0;

		inline void clearCallback()
		{
			OnFetchData = nullptr;
		}
	};

	IAchievement* getOSAchievement();
}
