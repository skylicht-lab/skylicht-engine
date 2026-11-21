//
//  GameViewController.m
//  DefaultMetal
//
//  Created by Pham Hong Duc on 10/02/2024.
//

#import "GameViewController.h"
#import "Renderer.h"

#import <SystemConfiguration/SystemConfiguration.h>
#import <netinet/in.h>

#include "SkylichtApplication.h"
#include "IOSScene.h"

SkylichtApplication* _angleApplication = NULL;
static __weak GameViewController *activeGameViewController = nil;

@implementation GameViewController
{
	MTKView *_view;
	Renderer *_renderer;
}

- (void)viewDidLoad
{
	[super viewDidLoad];
	activeGameViewController = self;
	
	_view = (MTKView *)self.view;
	_view.multipleTouchEnabled = YES;
	_view.backgroundColor = UIColor.blackColor;
	_view.preferredFramesPerSecond = 60;
	
	_renderer = [[Renderer alloc] initWithMetalKitView:_view];
	
	[_renderer mtkView:_view drawableSizeWillChange:_view.drawableSize];
	
	_view.delegate = _renderer;

	// Scene lifecycle events pause and resume rendering.
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillResignActive:) name:UISceneWillDeactivateNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillBecomeActive:) name:UISceneDidActivateNotification object:nil];
}

- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];
	if (_angleApplication != NULL)
		return;

	if (self.view.window == nil || irr::getIOSWindow() != self.view.window)
	{
		NSLog(@"ANGLE: Game view is not attached to the active iOS scene window");
		return;
	}

	// Initialize ANGLE after the storyboard view is attached to its scene window.
	int width = (int)_view.drawableSize.width;
	int height = (int)_view.drawableSize.height;
	
	_angleApplication = new SkylichtApplication(0, NULL, width, height);
	
	[_renderer setApplication:_angleApplication];
	
	// get the name of the app
	NSString* bundle = [[NSBundle mainBundle]bundleIdentifier];
	
	NSString *savePath = [NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES) objectAtIndex:0];
	savePath = [savePath stringByAppendingPathComponent:bundle];
	
	// set bundle id
	char bundleID[1024];
	[bundle getCString:bundleID maxLength:1024 encoding:NSASCIIStringEncoding];
	NSLog(@"Set bundle id: %@", bundle);
	_angleApplication->setBundleId(bundleID);
	
	// create save folder
	NSFileManager* fileManager = [NSFileManager defaultManager];
	if([fileManager fileExistsAtPath:savePath] == NO)
	{
		NSError *pError;
		if([fileManager createDirectoryAtPath:savePath withIntermediateDirectories:YES attributes:nil error:&pError] == NO)
		{
			savePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
			NSLog(@"Error %@", pError);
		}
	}

	NSString* downloadPath = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0];
	downloadPath = [downloadPath stringByAppendingPathComponent:bundle];

	if([fileManager fileExistsAtPath:downloadPath] == NO)
	{
		NSError *pError;
		if([fileManager createDirectoryAtPath:downloadPath withIntermediateDirectories:YES attributes:nil error:&pError] == NO)
		{
			downloadPath = savePath;
			NSLog(@"Error %@", pError);
		}
	}
	
	// NSLog(@"Save path %@", savePath);
	// NSLog(@"Download path %@", downloadPath);
	
	char path[1024];
	savePath = [savePath stringByAppendingFormat:@"/"];
	[savePath getCString:path maxLength:1024 encoding:NSASCIIStringEncoding];
	_angleApplication->setSaveFolder(path);
	
	char download[1024];
	downloadPath = [downloadPath stringByAppendingFormat:@"/"];
	[downloadPath getCString:download maxLength:1024 encoding:NSASCIIStringEncoding];
	_angleApplication->setDownloadFolder(download);

	_angleApplication->initialize();
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	if (activeGameViewController == self)
		activeGameViewController = nil;
	delete _angleApplication;
	_angleApplication = NULL;
}

- (void)setLimitFPS: (int)fps
{
	_view.preferredFramesPerSecond = fps;
}

#pragma mark - touches methods

- (int)getTouchId:(UITouch*)touch
{
	long pointer = (long)[touch hash];
	int touchId = (int)(pointer % 32000);
	return touchId;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	float scale = self.view.contentScaleFactor;
	
	for (UITouch* touch in touches)
	{
		int touchId = [self getTouchId:touch];
		CGPoint cursor = [touch locationInView:[self view]];
		
		if (_angleApplication)
			_angleApplication->onTouchDown(touchId, (int)(cursor.x*scale), (int)(cursor.y*scale));
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	float scale = self.view.contentScaleFactor;
	
	for (UITouch* touch in touches)
	{
		int touchId = [self getTouchId:touch];
		CGPoint cursor = [touch locationInView:[self view]];
		
		if (_angleApplication)
			_angleApplication->onTouchMove(touchId, (int)(cursor.x*scale), (int)(cursor.y*scale));
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	float scale = self.view.contentScaleFactor;
	
	for (UITouch* touch in touches)
	{
		int touchId = [self getTouchId:touch];
		CGPoint cursor = [touch locationInView:[self view]];
		
		if (_angleApplication)
			_angleApplication->onTouchUp(touchId, (int)(cursor.x*scale), (int)(cursor.y*scale));
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesEnded:touches withEvent:event];
}

#pragma mark - app pause/resume
-(void)appWillResignActive:(NSNotification*)note
{
	NSLog(@"appWillResignActive");
	
	if (_angleApplication)
	{
		_view.paused = YES;
		_angleApplication->pause();
	}
}

-(void)appWillBecomeActive:(NSNotification*)note
{
	NSLog(@"appWillBecomeActive");
	
	if (_angleApplication)
	{
		_angleApplication->resume();
		_view.paused = NO;
	}
}

@end

extern "C" void application_setFPS(int fps)
{
	[activeGameViewController setLimitFPS:(fps <= 0 ? 60 : fps)];
}

extern "C" void application_openURL(const char *url)
{
	NSString *urlString = [NSString stringWithUTF8String:url];
	NSURL *nsurl = [NSURL URLWithString:urlString];
	[[UIApplication sharedApplication] openURL:nsurl options:@{} completionHandler:nil];
}

extern "C" void application_openApplicationSetting()
{
	NSURL* url = [NSURL URLWithString:UIApplicationOpenSettingsURLString];
	UIApplication* application = [UIApplication sharedApplication];
	if ([application canOpenURL:url])
		[application openURL:url options:@{} completionHandler:nil];
}

extern "C" bool application_isNetworkAvailable()
{
	struct sockaddr_in zeroAddress;
	bzero(&zeroAddress, sizeof(zeroAddress));
	zeroAddress.sin_len = sizeof(zeroAddress);
	zeroAddress.sin_family = AF_INET;
	
	SCNetworkReachabilityRef reachability = SCNetworkReachabilityCreateWithAddress(kCFAllocatorDefault, (const struct sockaddr*)&zeroAddress);
	if (reachability != NULL)
	{
		SCNetworkReachabilityFlags flags;
		bool success = SCNetworkReachabilityGetFlags(reachability, &flags);
		CFRelease(reachability);
		if (success)
		{
			bool isReachable = ((flags & kSCNetworkReachabilityFlagsReachable) != 0);
			bool needsConnection = ((flags & kSCNetworkReachabilityFlagsConnectionRequired) != 0);
			return (isReachable && !needsConnection);
		}
	}
	return false;
}

extern "C" const char* application_getDeviceLanguage()
{
	static char language[16] = "en";
	NSString* preferredLanguage = [[NSLocale preferredLanguages] firstObject];
	if (preferredLanguage != nil)
	{
		NSDictionary* components = [NSLocale componentsFromLocaleIdentifier:preferredLanguage];
		NSString* languageCode = [components objectForKey:NSLocaleLanguageCode];
		if (languageCode != nil && [languageCode length] > 0)
			[languageCode getCString:language maxLength:sizeof(language) encoding:NSUTF8StringEncoding];
	}
	return language;
}
