//
//  GameViewController.m
//  DefaultMetal
//
//  Created by Pham Hong Duc on 10/02/2024.
//

#import "GameViewController.h"
#import "Renderer.h"

#include "SkylichtApplication.h"

SkylichtApplication* _angleApplication = NULL;

@implementation GameViewController
{
    MTKView *_view;
    Renderer *_renderer;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    _view = (MTKView *)self.view;
    
    _view.backgroundColor = UIColor.blackColor;
    _renderer = [[Renderer alloc] initWithMetalKitView:_view];

    [_renderer mtkView:_view drawableSizeWillChange:_view.drawableSize];

    _view.delegate = _renderer;
    
    // init Angle Application
    int width = (int)_view.drawableSize.width;
    int height = (int)_view.drawableSize.height;
        
    _angleApplication = new SkylichtApplication(0, NULL, width, height);
    _angleApplication->initialize();
    
    [_renderer setApplication:_angleApplication];
    
    // register event (https://stackoverflow.com/questions/9011868/whats-the-best-way-to-detect-when-the-app-is-entering-the-background-for-my-vie)
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillResignActive:) name:UIApplicationWillResignActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillBecomeActive:) name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillTerminate:) name:UIApplicationWillTerminateNotification object:nil];
    
    // get the name of the app
    NSString* bundle = [[NSBundle mainBundle]bundleIdentifier];
    
    NSString* savePath = [NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    savePath = [savePath stringByAppendingFormat:@"/%@", bundle];
    
    // set bundle id
    char bundleID[1024];
    [bundle getCString:bundleID maxLength:1024 encoding:NSASCIIStringEncoding];
    NSLog(@"Set bundle id: %@", bundle);
    _angleApplication->setBundleId(bundleID);
    
    // create folder
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
    
    char path[1024];
    savePath = [savePath stringByAppendingFormat:@"/"];
    [savePath getCString:path maxLength:1024 encoding:NSASCIIStringEncoding];
    _angleApplication->setSaveFolder(path);
}

- (void)dealloc
{
    delete _angleApplication;
}

#pragma mark - touches methods

- (int)getTouchId:(UITouch*)touch
{
    long pointer = (long)touch;
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
        _angleApplication->onPause();
}

-(void)appWillBecomeActive:(NSNotification*)note
{
    NSLog(@"appWillBecomeActive");
    
    if (_angleApplication)
        _angleApplication->onResume();
}

-(void)appWillTerminate:(NSNotification*)note
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationWillResignActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationDidBecomeActiveNotification  object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationWillTerminateNotification object:nil];
}
@end
