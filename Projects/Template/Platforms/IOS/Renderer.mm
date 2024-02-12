//
//  Renderer.m
//  DefaultMetal
//
//  Created by Pham Hong Duc on 10/02/2024.
//

#import <simd/simd.h>
#import <ModelIO/ModelIO.h>

#import "Renderer.h"

static const NSUInteger MaxBuffersInFlight = 3;

@implementation Renderer
{
    AngleApplication *_angleApplication;
}

-(nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view;
{
    _angleApplication = NULL;
    self = [super init];
    return self;
}

-(void)setApplication:(AngleApplication*)app
{
    _angleApplication = app;
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
    /// Per frame updates here
    if (_angleApplication == NULL)
        return;
    
    _angleApplication->draw();
    _angleApplication->swap();
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    /// Respond to drawable size or orientation changes here
    NSLog(@"drawableSizeWillChange %f %f", size.width, size.height);
    if (_angleApplication == NULL)
        return;
    
    _angleApplication->onResized((int)size.width, (int)size.height);
}

@end
