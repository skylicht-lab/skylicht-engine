#pragma once

namespace Skylicht
{
	class IAchievement
	{
	public:
		IAchievement();

		virtual ~IAchievement();

		// percent is in range 0.0f to 100.0f.
		virtual void updateAchievement(const char* id, int step, float percent) = 0;
	};

	IAchievement* getOSAchievement();
}
