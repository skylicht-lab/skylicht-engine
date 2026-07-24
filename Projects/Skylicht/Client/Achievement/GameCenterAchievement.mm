#ifdef IOS

#include "GameCenterAchievement.h"

#import <GameKit/GameKit.h>
#import <UIKit/UIKit.h>

@interface SkylichtGameCenterAchievementDelegate : NSObject<GKGameCenterControllerDelegate>
@end

@implementation SkylichtGameCenterAchievementDelegate

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController*)gameCenterViewController
{
	[gameCenterViewController dismissViewControllerAnimated:YES completion:nil];
}

@end

static SkylichtGameCenterAchievementDelegate* s_gameCenterAchievementDelegate = nil;

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

void gamecenter_showDefaultAchievementsUI()
{
	dispatch_async(dispatch_get_main_queue(), ^
	{
		if ([GKLocalPlayer localPlayer].isAuthenticated == NO)
			return;

		GKGameCenterViewController* gameCenterViewController = [[GKGameCenterViewController alloc] init];
		if (gameCenterViewController == nil)
			return;

		if (s_gameCenterAchievementDelegate == nil)
			s_gameCenterAchievementDelegate = [[SkylichtGameCenterAchievementDelegate alloc] init];

		gameCenterViewController.gameCenterDelegate = s_gameCenterAchievementDelegate;
		gameCenterViewController.viewState = GKGameCenterViewControllerStateAchievements;

		UIViewController* rootViewController = [[[UIApplication sharedApplication] keyWindow] rootViewController];
		if (rootViewController == nil)
			return;

		[rootViewController presentViewController:gameCenterViewController animated:YES completion:nil];
	});
}

#endif
