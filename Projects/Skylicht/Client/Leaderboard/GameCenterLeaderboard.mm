#include "GameCenterLeaderboard.h"

#ifdef IOS
#import <GameKit/GameKit.h>
#import <UIKit/UIKit.h>

@interface SkylichtGameCenterLeaderboardDelegate : NSObject<GKGameCenterControllerDelegate>
@end

@implementation SkylichtGameCenterLeaderboardDelegate
- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController*)controller
{
	[controller dismissViewControllerAnimated:YES completion:nil];
}
@end

static SkylichtGameCenterLeaderboardDelegate* s_gameCenterLeaderboardDelegate = nil;

void gamecenter_postScore(const char* id, int64_t score)
{
	if (id == nullptr || id[0] == '\0')
		return;

	NSString* leaderboardID = [NSString stringWithUTF8String:id];
	if (leaderboardID == nil)
		return;

	dispatch_async(dispatch_get_main_queue(), ^
	{
		GKLocalPlayer* player = [GKLocalPlayer localPlayer];
		if (!player.isAuthenticated)
		{
			NSLog(@"Skylicht: Cannot post leaderboard score before Game Center sign-in");
			return;
		}

		if (@available(iOS 14.0, *))
		{
			[GKLeaderboard submitScore:(NSInteger)score context:0 player:player leaderboardIDs:@[leaderboardID] withCompletionHandler:^(NSError* error)
			{
				if (error != nil)
					NSLog(@"Skylicht: Post leaderboard score failed for %@: %@", leaderboardID, error);
				else
					NSLog(@"Skylicht: Posted leaderboard score for %@: %lld", leaderboardID, (long long)score);
			}];
		}
		else
		{
			GKScore* entry = [[GKScore alloc] initWithLeaderboardIdentifier:leaderboardID];
			entry.value = score;
			[GKScore reportScores:@[entry] withCompletionHandler:^(NSError* error)
			{
				if (error != nil)
					NSLog(@"Skylicht: Post leaderboard score failed for %@: %@", leaderboardID, error);
				else
					NSLog(@"Skylicht: Posted leaderboard score for %@: %lld", leaderboardID, (long long)score);
			}];
		}
	});
}

void gamecenter_showDefaultLeaderboardUI(const char* id)
{
	NSString* leaderboardID = id != nullptr && id[0] != '\0' ? [NSString stringWithUTF8String:id] : nil;
	dispatch_async(dispatch_get_main_queue(), ^
	{
		if (![GKLocalPlayer localPlayer].isAuthenticated)
			return;

		UIViewController* root = [[[UIApplication sharedApplication] keyWindow] rootViewController];
		while (root.presentedViewController != nil)
			root = root.presentedViewController;
		if (root == nil)
			return;

		if (s_gameCenterLeaderboardDelegate == nil)
			s_gameCenterLeaderboardDelegate = [[SkylichtGameCenterLeaderboardDelegate alloc] init];

		GKGameCenterViewController* controller;
		if (@available(iOS 14.0, *))
		{
			if (leaderboardID != nil)
				controller = [[GKGameCenterViewController alloc] initWithLeaderboardID:leaderboardID playerScope:GKLeaderboardPlayerScopeGlobal timeScope:GKLeaderboardTimeScopeAllTime];
			else
				controller = [[GKGameCenterViewController alloc] initWithState:GKGameCenterViewControllerStateLeaderboards];
		}
		else
		{
			controller = [[GKGameCenterViewController alloc] init];
			controller.viewState = GKGameCenterViewControllerStateLeaderboards;
			controller.leaderboardIdentifier = leaderboardID;
			controller.leaderboardTimeScope = GKLeaderboardTimeScopeAllTime;
		}
		controller.gameCenterDelegate = s_gameCenterLeaderboardDelegate;
		[root presentViewController:controller animated:YES completion:nil];
	});
}
#endif
