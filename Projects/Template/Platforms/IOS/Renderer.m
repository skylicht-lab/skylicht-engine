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
    
}

-(nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view;
{
    self = [super init];
    return self;
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
    /// Per frame updates here
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    /// Respond to drawable size or orientation changes here
    NSLog(@"drawableSizeWillChange %f %f", size.width, size.height);
}

@end
