#if defined(MACOS)
// Copyright (C) 2005-2006 Etienne Petitjean
// Copyright (C) 2007-2012 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#import <Cocoa/Cocoa.h>

void OSXCopyToClipboard(const char *text)
{
	NSString *str;
	NSPasteboard *board;
	
	if ((text != NULL) && (strlen(text) > 0))
	{
		str = [NSString stringWithCString:text encoding:NSWindowsCP1252StringEncoding];
		board = [NSPasteboard generalPasteboard];
		[board declareTypes:[NSArray arrayWithObject:NSPasteboardTypeString] owner:NSApp];
		[board setString:str forType:NSPasteboardTypeString];
	}
}

char* OSXCopyFromClipboard()
{
	NSString* str;
	NSPasteboard* board;
	char* result;
	
	result = NULL;
	board = [NSPasteboard generalPasteboard];
	str = [board stringForType:NSPasteboardTypeString];
	if (str != nil)
		result = (char*)[str cStringUsingEncoding:NSWindowsCP1252StringEncoding];
	return (result);
}
#endif
