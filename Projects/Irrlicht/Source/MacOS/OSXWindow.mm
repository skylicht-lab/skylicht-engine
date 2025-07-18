#if defined(MACOS)
#import <CoreGraphics/CGEvent.h>
#import <Cocoa/Cocoa.h>

#include <string>

NSWindow* g_window = nil;

void OSXSetWindow(void* window)
{
	g_window = (NSWindow*)window;
}

void OSXSetDefaultCaption()
{
	NSDictionary<NSString *, id> *info = [[NSBundle mainBundle] infoDictionary];
	NSString* name = [info objectForKey:@"CFBundleExecutable"];
	if (name != nil)
		[g_window setTitle:name];
}

void OSXSetWindowCaption(const wchar_t* caption)
{
	size_t size;
	char title[1024];
	
	if (g_window != nil)
	{
		size = wcstombs(title,caption,1024);
		title[1023] = 0;
		NSString* name = [NSString stringWithCString:title encoding:NSUTF8StringEncoding];
		[g_window setTitle:name];
		[name release];
	}
}

void OSXClose()
{
	if (g_window != nil)
	{
		[g_window setIsVisible:FALSE];
		[g_window setReleasedWhenClosed:TRUE];
		[g_window release];
		g_window = nil;
	}
}

void OSXSetResizable(bool resize)
{
	if (g_window != nil)
	{
		if (resize)
			[g_window setStyleMask:NSTitledWindowMask|NSClosableWindowMask|NSMiniaturizableWindowMask|NSResizableWindowMask];
		else
			[g_window setStyleMask:NSTitledWindowMask|NSClosableWindowMask];
	}
}

void OSXMinimizeWindow()
{
	if (g_window != nil)
		[g_window miniaturize:[NSApp self]];
}

void OSXMaximizeWindow()
{
	if (g_window != nil)
	{
		// not yet test
		NSScreen* screen = [NSScreen mainScreen];
		[g_window setFrame:screen.visibleFrame display:true animate:true];
	}
}

void OSXRestoreWindow()
{
	if (g_window != nil)
	{
		[g_window deminiaturize:[NSApp self]];
	}
}

void OSXGetWindowPosition(int& x, int &y)
{
	x = 0;
	y = 0;
	if (g_window != nil)
	{
		NSRect rect = [g_window frame];
		int screenHeight = [[[NSScreen screens] objectAtIndex:0] frame].size.height;
		x = rect.origin.x;
		y = screenHeight - rect.origin.y - rect.size.height;
	}
}

void OSXSetCursorVisible(bool visible)
{
	if (visible)
		CGDisplayShowCursor(CGMainDisplayID());
	else
		CGDisplayHideCursor(CGMainDisplayID());
}

float translateWindowMouseY(float y)
{
	NSView *view = [g_window contentView];
	return [view frame].size.height - y;
}

float translateScreenMouseY(float y)
{
	int screenHeight = [[[NSScreen screens] objectAtIndex:0] frame].size.height;
	return screenHeight - y;
}

void OSXSetMouseLocation(int x, int y)
{
	NSPoint p;
	CGPoint c;
	
	if (g_window != NULL)
	{
		p.x = (float) x;
		p.y = (float) translateWindowMouseY(y);
		
		p = [g_window convertBaseToScreen:p];
		p.y = translateScreenMouseY(p.y);
	}
	
	c.x = p.x;
	c.y = p.y;
	
	CGWarpMouseCursorPosition(c);
}

void OSXSetCursorIcon(const char *icon)
{
	std::string iconName = icon;
	
	if (iconName == "normal")
	{
		[[NSCursor arrowCursor] set];
	}
	else if (iconName == "cross")
	{
		[[NSCursor crosshairCursor] set];
	}
	else if (iconName == "hand")
	{
		[[NSCursor openHandCursor] set];
	}
	else if (iconName == "ibeam")
	{
		[[NSCursor IBeamCursor] set];
	}
	else if (iconName == "no")
	{
		[[NSCursor operationNotAllowedCursor] set];
	}
	else if (iconName == "wait")
	{
		[[NSCursor arrowCursor] set];
	}
	else if (iconName == "sizeall")
	{
		[[NSCursor arrowCursor] set];
	}
	else if (iconName == "sizenesw")
	{
		[[NSCursor arrowCursor] set];
	}
	else if (iconName == "sizenwse")
	{
		[[NSCursor arrowCursor] set];
	}
	else if (iconName == "sizens")
	{
		[[NSCursor resizeUpDownCursor] set];
	}
	else if (iconName == "sizewe")
	{
		[[NSCursor resizeLeftRightCursor] set];
	}
	else if (iconName == "sizeup")
	{
		[[NSCursor resizeUpCursor] set];
	}
}
#endif
