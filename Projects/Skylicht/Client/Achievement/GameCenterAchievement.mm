#ifdef IOS

#include "GameCenterAchievement.h"

#import <GameKit/GameKit.h>

void gamecenter_updateAchievement(const char* id, int step, float percent)
{
	if (id == NULL)
		return;

	(void)step;

	if (percent < 0.0f)
		percent = 0.0f;
	else if (percent > 100.0f)
		percent = 100.0f;

	NSString* achievementID = [NSString stringWithUTF8String:id];
	if (achievementID == nil)
		return;

	GKAchievement* achievement = [[GKAchievement alloc] initWithIdentifier:achievementID];
	if (achievement == nil)
		return;

	achievement.percentComplete = percent;
	achievement.showsCompletionBanner = YES;

	[GKAchievement reportAchievements:@[achievement] withCompletionHandler:^(NSError* error)
	{
	}];
}

#endif
