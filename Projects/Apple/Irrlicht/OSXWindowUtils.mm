#import <Cocoa/Cocoa.h>

NSWindow* g_window = nil;

extern "C" void exitOSXApp();

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
    
    exitOSXApp();
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
