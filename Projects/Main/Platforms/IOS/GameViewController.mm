//
//  GameViewController.m
//  DefaultMetal
//
//  Created by Pham Hong Duc on 10/02/2024.
//

#import "GameViewController.h"
#import "Renderer.h"

#include "SkylichtApplication.h"

@implementation GameViewController
{
    MTKView *_view;
    Renderer *_renderer;
    AngleApplication* _angleApplication;
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
}

@end
