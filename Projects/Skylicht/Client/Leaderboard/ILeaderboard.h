#pragma once

#include <cstdint>

namespace Skylicht
{
	class ILeaderboard
	{
	public:
		virtual ~ILeaderboard() = default;

		// Submit an absolute score, not a delta. Ranking rules are configured in the store console.
		// The caller must sign in first and supply the leaderboard ID for the current platform.
		virtual void postScore(const char* id, int64_t score) = 0;

		// A null or empty ID opens the list of leaderboards.
		virtual void showDefaultLeaderboardUI(const char* id = nullptr) = 0;
	};

	// Returns null on unsupported platforms.
	ILeaderboard* getOSLeaderboard();
}
