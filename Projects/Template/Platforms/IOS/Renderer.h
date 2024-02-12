//
//  Renderer.h
//  DefaultMetal
//
//  Created by Pham Hong Duc on 10/02/2024.
//

#import <MetalKit/MetalKit.h>
#include "AngleApplication.h"

// Our platform independent renderer class.   Implements the MTKViewDelegate protocol which
//   allows it to accept per-frame update and drawable resize callbacks.
@interface Renderer : NSObject <MTKViewDelegate>

-(nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view;

-(void)setApplication:(AngleApplication*)app;

@end

