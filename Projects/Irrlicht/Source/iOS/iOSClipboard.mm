// Copyright (C) 2024 Skylicht
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#if defined(_IRR_IOS_PLATFORM_)
#import <UIKit/UIKit.h>

extern "C" void iOSCopyToClipboard(const char *text)
{
	NSString *str;
	UIPasteboard *board;
	
	if ((text != NULL) && (strlen(text) > 0))
	{
		str = [NSString stringWithCString:text encoding:NSWindowsCP1252StringEncoding];
		board = [UIPasteboard generalPasteboard];
		[board setString:str];
	}
}

extern "C" char* iOSCopyFromClipboard()
{
	NSString* str;
	UIPasteboard* board;
	char* result;
	
	result = NULL;
	board = [UIPasteboard generalPasteboard];
	str = [board string];
	if (str != nil)
		result = (char*)[str cStringUsingEncoding:NSWindowsCP1252StringEncoding];
	return (result);
}
#endif
