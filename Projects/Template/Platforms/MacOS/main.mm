//
//  main.m
//  mainapp
//
//  Created by Tuyet Nguyet on 4/11/20.
//  Copyright © 2020 Skylicht Co LTD. All rights reserved.
//

#include "SkylichtApplication.h"
#include "BuildConfig/CBuildConfig.h"
#include <Carbon/Carbon.h>

int main(int argc, const char * argv[]) {
    NSBundle *main = [NSBundle mainBundle];
    const char *resourcePath = [[main resourcePath] UTF8String];
    CBuildConfig::DataFolder = resourcePath;
    
    SkylichtApplication app(argc, (char**)argv);
    return app.run();
}
