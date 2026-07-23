#if defined(IOS)
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <UserNotifications/UserNotifications.h>

extern "C" void ios_onPostNotificationPermissionUpdate(int permission);
extern "C" void ios_onPostNotificationPermissionCheck(int permission);

enum EPostNotificationPermission
{
	PostNotificationNotDetermined = 0,
	PostNotificationDenied,
	PostNotificationAllowed
};

static int getPermissionFromStatus(UNAuthorizationStatus status)
{
	switch (status)
	{
	case UNAuthorizationStatusDenied:
		return PostNotificationDenied;
	case UNAuthorizationStatusAuthorized:
	case UNAuthorizationStatusProvisional:
		return PostNotificationAllowed;
	case UNAuthorizationStatusNotDetermined:
	default:
		return PostNotificationNotDetermined;
	}
}

static NSString* getString(const char* value)
{
	if (value == NULL || value[0] == 0)
		return @"";

	return [NSString stringWithUTF8String:value];
}

extern "C" {
	
void ios_checkPostNotificationPermissionAsync()
{
	UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];

	[center getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings* _Nonnull settings) {
		int ret = getPermissionFromStatus(settings.authorizationStatus);
		ios_onPostNotificationPermissionCheck(ret);
	}];
}

void ios_requestPostNotificationPermission()
{
	UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
	UNAuthorizationOptions options = UNAuthorizationOptionAlert | UNAuthorizationOptionSound | UNAuthorizationOptionBadge;

	[center requestAuthorizationWithOptions:options completionHandler:^(BOOL granted, NSError* _Nullable error) {
		if (error != nil)
			NSLog(@"Failed to request notification permission: %@", [error localizedDescription]);

		int ret = granted ? PostNotificationAllowed : PostNotificationDenied;
		ios_onPostNotificationPermissionUpdate(ret);
	}];
}

void ios_schedulePostNotification(const char* id, const char* title, const char* content, unsigned long delayFromNow)
{
	NSString* identifier = getString(id);
	if ([identifier length] == 0)
		return;

    NSLog(@"Schedule notification %@", identifier);
    
	UNMutableNotificationContent* notificationContent = [[UNMutableNotificationContent alloc] init];
	notificationContent.title = getString(title);
	notificationContent.body = getString(content);
	notificationContent.sound = [UNNotificationSound defaultSound];

	NSTimeInterval timeInterval = delayFromNow > 0 ? (NSTimeInterval)delayFromNow : 1.0;
	UNTimeIntervalNotificationTrigger* trigger = [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:timeInterval repeats:NO];
	UNNotificationRequest* request = [UNNotificationRequest requestWithIdentifier:identifier content:notificationContent trigger:trigger];

	UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
	[center removePendingNotificationRequestsWithIdentifiers:@[identifier]];
	[center addNotificationRequest:request withCompletionHandler:^(NSError* _Nullable error) {
		if (error != nil)
			NSLog(@"Failed to schedule notification %@: %@", identifier, [error localizedDescription]);
	}];
}

void ios_removePostNotification(const char* id)
{
	NSString* identifier = getString(id);
	if ([identifier length] == 0)
		return;

	UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
	[center removePendingNotificationRequestsWithIdentifiers:@[identifier]];
	[center removeDeliveredNotificationsWithIdentifiers:@[identifier]];
}

void ios_clearPostNotification()
{
	UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
	[center removeAllPendingNotificationRequests];
	[center removeAllDeliveredNotifications];
}

}
#endif
