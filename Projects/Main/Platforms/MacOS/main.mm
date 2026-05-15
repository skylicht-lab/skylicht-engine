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

#import <Cocoa/Cocoa.h>

extern "C" void application_openURL(const char *url)
{
	NSString *urlString = [NSString stringWithUTF8String:url];
	NSURL *nsurl = [NSURL URLWithString:urlString];
	[[NSWorkspace sharedWorkspace] openURL:nsurl];
}

#ifndef TEST_APP

int main(int argc, const char * argv[]) {
	NSBundle *main = [NSBundle mainBundle];
	const char *resourcePath = [[main resourcePath] UTF8String];
	CBuildConfig::getInstance()->DataFolder = resourcePath;
	
	SkylichtApplication app(argc, (char**)argv);
	return app.run();
}

#endif
