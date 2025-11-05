#include "GameCenterSignIn.h"

#import <GameKit/GameKit.h>

void gamecenter_signInSuccess(const char* playerId, const char* playerName, const char *code);
void gamecenter_signInFailed(const char* log);

void gamecenter_signIn()
{
	GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
	localPlayer.authenticateHandler = ^(UIViewController *viewController, NSError *error)
	{
		GKLocalPlayer* localPlayer = [GKLocalPlayer localPlayer];
		
		if (viewController != nil)
		{
			[[[[UIApplication sharedApplication] keyWindow] rootViewController] presentViewController:viewController animated:YES completion:nil];
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
		}
	};
}
