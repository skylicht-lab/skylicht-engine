#pragma once

#ifdef IOS
#include <cstdint>

void gamecenter_postScore(const char* id, int64_t score);
void gamecenter_showDefaultLeaderboardUI(const char* id);
#endif
