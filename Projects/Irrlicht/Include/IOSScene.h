#pragma once

#ifdef IOS
#import <UIKit/UIKit.h>

namespace irr
{
static inline UIWindow *getIOSWindowInScene(UIWindowScene *scene)
{
	for (UIWindow *window in scene.windows)
	{
		if (window.isKeyWindow)
			return window;
	}

	// During viewDidLoad the storyboard window may not be key yet.
	for (UIWindow *window in scene.windows)
	{
		if (window.rootViewController != nil)
			return window;
	}
	return nil;
}

static inline UIWindow *getIOSWindow(void)
{
	UIWindow *launchingWindow = nil;
	UIWindow *otherWindow = nil;
	for (UIScene *scene in [UIApplication sharedApplication].connectedScenes)
	{
		if (![scene isKindOfClass:[UIWindowScene class]])
			continue;

		UIWindow *window = getIOSWindowInScene((UIWindowScene *)scene);
		if (window == nil)
			continue;
		if (scene.activationState == UISceneActivationStateForegroundActive)
			return window;
		if (scene.activationState == UISceneActivationStateForegroundInactive && launchingWindow == nil)
			launchingWindow = window;
		if (otherWindow == nil)
			otherWindow = window;
	}
	return launchingWindow != nil ? launchingWindow : otherWindow;
}

static inline UIViewController *getIOSPresentingViewController(void)
{
	UIViewController *controller = getIOSWindow().rootViewController;
	while (controller.presentedViewController != nil)
		controller = controller.presentedViewController;
	return controller;
}
}
#endif
