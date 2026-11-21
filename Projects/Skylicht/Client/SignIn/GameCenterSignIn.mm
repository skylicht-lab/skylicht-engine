#ifdef IOS

#include "GameCenterSignIn.h"
#include "IOSScene.h"

#import <GameKit/GameKit.h>

void gamecenter_signInSuccess(const char* playerId, const char* playerName, const char *code);
void gamecenter_signInFailed(const char* log);

void gamecenter_signIn()
{
	GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];

	if (localPlayer.isAuthenticated)
    {
		NSString *playerID = localPlayer.playerID;
		NSString *alias = localPlayer.alias;

		const char* playerId = [playerID UTF8String];
		const char* playerName = [alias UTF8String];

		gamecenter_signInSuccess(playerId, playerName, "");
		return;
	}

	localPlayer.authenticateHandler = ^(UIViewController *viewController, NSError *error)
	{
		GKLocalPlayer* localPlayer = [GKLocalPlayer localPlayer];
		
		if (viewController != nil)
		{
			dispatch_async(dispatch_get_main_queue(), ^{
				UIViewController *presenter = irr::getIOSPresentingViewController();
				if (presenter == nil)
				{
					gamecenter_signInFailed("No active iOS scene window for Game Center sign-in");
					return;
				}
				[presenter presentViewController:viewController animated:YES completion:nil];
			});
		}
		else if (localPlayer.isAuthenticated)
		{
			NSString *playerID = localPlayer.playerID;
			NSString *alias = localPlayer.alias;
			
			const char* playerId = [playerID UTF8String];
			const char* playerName = [alias UTF8String];
			
			gamecenter_signInSuccess(playerId, playerName, "");
		}
		else
		{
			if (error != nil)
			{
				const char* message = [[error localizedDescription] UTF8String];
				gamecenter_signInFailed(message);
			}
			else
			{
				gamecenter_signInFailed("Unknown error");
			}
		}
	};
}

bool gamecenter_isSignedIn()
{
	return [GKLocalPlayer localPlayer].isAuthenticated;
}

#endif
