//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// OSXWindow.mm: Implementation of OSWindow for OSX

#if defined(MACOS)

#include "util/osx/OSXWindow.h"

#include <set>
// Include Carbon to use the keycode names in Carbon's Event.h
#include <Carbon/Carbon.h>

#include "anglebase/no_destructor.h"
#include "common/debug.h"

// On OSX 10.12 a number of AppKit interfaces have been renamed for consistency, and the previous
// symbols tagged as deprecated. However we can't simply use the new symbols as it would break
// compilation on our automated testing that doesn't use OSX 10.12 yet. So we just ignore the
// warnings.
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// Some events such as "ShouldTerminate" are sent to the whole application so we keep a list of
// all the windows in order to forward the event to each of them. However this and calling pushEvent
// in ApplicationDelegate is inherently unsafe in a multithreaded environment.
static std::set<OSXWindow *> &AllWindows()
{
    static angle::base::NoDestructor<std::set<OSXWindow *>> allWindows;
    return *allWindows;
}

@interface Application : NSApplication
@end

@implementation Application
- (void)sendEvent:(NSEvent *)nsEvent
{
    if ([nsEvent type] == NSApplicationDefined)
    {
        for (auto window : AllWindows())
        {
            if ([window->getNSWindow() windowNumber] == [nsEvent windowNumber])
            {
                Event event;
                event.Type = Event::EVENT_TEST;
                window->pushEvent(event);
            }
        }
    }
    [super sendEvent:nsEvent];
}

// Override internal method to try to diagnose unexpected crashes in Core Animation.
// anglebug.com/6570
// See also:
//   https://github.com/microsoft/appcenter-sdk-apple/issues/1944
//   https://stackoverflow.com/questions/220159/how-do-you-print-out-a-stack-trace-to-the-console-log-in-cocoa
- (void)_crashOnException:(NSException *)exception
{
    NSLog(@"*** OSXWindow aborting on exception:  <%@> %@", [exception name], [exception reason]);
    NSLog(@"%@", [exception callStackSymbols]);
    abort();
}
@end

// The Delegate receiving application-wide events.
@interface ApplicationDelegate : NSObject
@end

@implementation ApplicationDelegate
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    Event event;
    event.Type = Event::EVENT_CLOSED;
    for (auto window : AllWindows())
    {
        window->pushEvent(event);
    }
    return NSTerminateCancel;
}
@end
static ApplicationDelegate *gApplicationDelegate = nil;

static bool InitializeAppKit()
{
    if (NSApp != nil)
    {
        return true;
    }

    // Initialize the global variable "NSApp"
    [Application sharedApplication];

    // Make us appear in the dock
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    // Register our global event handler
    gApplicationDelegate = [[ApplicationDelegate alloc] init];
    if (gApplicationDelegate == nil)
    {
        return false;
    }
    [NSApp setDelegate:static_cast<id>(gApplicationDelegate)];

    // Set our status to "started" so we are not bouncing in the doc and can activate
    [NSApp finishLaunching];
    return true;
}

// NS's and CG's coordinate systems start at the bottom left, while OSWindow's coordinate
// system starts at the top left. This function converts the Y coordinate accordingly.
static float YCoordToFromCG(float y)
{
    float screenHeight = CGDisplayBounds(CGMainDisplayID()).size.height;
    return screenHeight - y;
}

// Delegate for window-wide events, note that the protocol doesn't contain anything input related.
@implementation WindowDelegate
- (id)initWithWindow:(OSXWindow *)window
{
    self = [super init];
    if (self != nil)
    {
        mWindow = window;
    }
    return self;
}

- (void)onOSXWindowDeleted
{
    mWindow = nil;
}

- (BOOL)windowShouldClose:(id)sender
{
    Event event;
    event.Type = Event::EVENT_CLOSED;
    mWindow->pushEvent(event);
    return NO;
}

- (void)windowDidResize:(NSNotification *)notification
{
    NSSize windowSize = [[mWindow->getNSWindow() contentView] frame].size;
    Event event;
    event.Type        = Event::EVENT_RESIZED;
    event.Size.Width  = (int)windowSize.width;
    event.Size.Height = (int)windowSize.height;
    mWindow->pushEvent(event);
}

- (void)windowDidMove:(NSNotification *)notification
{
    NSRect screenspace = [mWindow->getNSWindow() frame];
    Event event;
    event.Type   = Event::EVENT_MOVED;
    event.Move.X = (int)screenspace.origin.x;
    event.Move.Y = (int)YCoordToFromCG(screenspace.origin.y + screenspace.size.height);
    mWindow->pushEvent(event);
}

- (void)windowDidBecomeKey:(NSNotification *)notification
{
    Event event;
    event.Type = Event::EVENT_GAINED_FOCUS;
    mWindow->pushEvent(event);
    [self retain];
}

- (void)windowDidResignKey:(NSNotification *)notification
{
    if (mWindow != nil)
    {
        Event event;
        event.Type = Event::EVENT_LOST_FOCUS;
        mWindow->pushEvent(event);
    }
    [self release];
}
@end

static Angle::Key NSCodeToKey(int keyCode)
{
    // Missing Angle::KEY_PAUSE
    switch (keyCode)
    {
        case kVK_Shift:
            return Angle::KEY_LSHIFT;
        case kVK_RightShift:
            return Angle::KEY_RSHIFT;
        case kVK_Option:
            return Angle::KEY_LALT;
        case kVK_RightOption:
            return Angle::KEY_RALT;
        case kVK_Control:
            return Angle::KEY_LCONTROL;
        case kVK_RightControl:
            return Angle::KEY_RCONTROL;
        case kVK_Command:
            return Angle::KEY_LSYSTEM;
        // Right System doesn't have a name, but shows up as 0x36.
        case 0x36:
            return Angle::KEY_RSYSTEM;
        case kVK_Function:
            return Angle::KEY_MENU;

        case kVK_ANSI_Semicolon:
            return Angle::KEY_SEMICOLON;
        case kVK_ANSI_Slash:
            return Angle::KEY_SLASH;
        case kVK_ANSI_Equal:
            return Angle::KEY_EQUAL;
        case kVK_ANSI_Minus:
            return Angle::KEY_DASH;
        case kVK_ANSI_LeftBracket:
            return Angle::KEY_LBRACKET;
        case kVK_ANSI_RightBracket:
            return Angle::KEY_RBRACKET;
        case kVK_ANSI_Comma:
            return Angle::KEY_COMMA;
        case kVK_ANSI_Period:
            return Angle::KEY_PERIOD;
        case kVK_ANSI_Backslash:
            return Angle::KEY_BACKSLASH;
        case kVK_ANSI_Grave:
            return Angle::KEY_TILDE;
        case kVK_Escape:
            return Angle::KEY_ESCAPE;
        case kVK_Space:
            return Angle::KEY_SPACE;
        case kVK_Return:
            return Angle::KEY_RETURN;
        case kVK_Delete:
            return Angle::KEY_BACK;
        case kVK_Tab:
            return Angle::KEY_TAB;
        case kVK_PageUp:
            return Angle::KEY_PAGEUP;
        case kVK_PageDown:
            return Angle::KEY_PAGEDOWN;
        case kVK_End:
            return Angle::KEY_END;
        case kVK_Home:
            return Angle::KEY_HOME;
        case kVK_Help:
            return Angle::KEY_INSERT;
        case kVK_ForwardDelete:
            return Angle::KEY_DELETE;
        case kVK_ANSI_KeypadPlus:
            return Angle::KEY_ADD;
        case kVK_ANSI_KeypadMinus:
            return Angle::KEY_SUBTRACT;
        case kVK_ANSI_KeypadMultiply:
            return Angle::KEY_MULTIPLY;
        case kVK_ANSI_KeypadDivide:
            return Angle::KEY_DIVIDE;

        case kVK_F1:
            return Angle::KEY_F1;
        case kVK_F2:
            return Angle::KEY_F2;
        case kVK_F3:
            return Angle::KEY_F3;
        case kVK_F4:
            return Angle::KEY_F4;
        case kVK_F5:
            return Angle::KEY_F5;
        case kVK_F6:
            return Angle::KEY_F6;
        case kVK_F7:
            return Angle::KEY_F7;
        case kVK_F8:
            return Angle::KEY_F8;
        case kVK_F9:
            return Angle::KEY_F9;
        case kVK_F10:
            return Angle::KEY_F10;
        case kVK_F11:
            return Angle::KEY_F11;
        case kVK_F12:
            return Angle::KEY_F12;
        case kVK_F13:
            return Angle::KEY_F13;
        case kVK_F14:
            return Angle::KEY_F14;
        case kVK_F15:
            return Angle::KEY_F15;

        case kVK_LeftArrow:
            return Angle::KEY_LEFT;
        case kVK_RightArrow:
            return Angle::KEY_RIGHT;
        case kVK_DownArrow:
            return Angle::KEY_DOWN;
        case kVK_UpArrow:
            return Angle::KEY_UP;

        case kVK_ANSI_Keypad0:
            return Angle::KEY_NUMPAD0;
        case kVK_ANSI_Keypad1:
            return Angle::KEY_NUMPAD1;
        case kVK_ANSI_Keypad2:
            return Angle::KEY_NUMPAD2;
        case kVK_ANSI_Keypad3:
            return Angle::KEY_NUMPAD3;
        case kVK_ANSI_Keypad4:
            return Angle::KEY_NUMPAD4;
        case kVK_ANSI_Keypad5:
            return Angle::KEY_NUMPAD5;
        case kVK_ANSI_Keypad6:
            return Angle::KEY_NUMPAD6;
        case kVK_ANSI_Keypad7:
            return Angle::KEY_NUMPAD7;
        case kVK_ANSI_Keypad8:
            return Angle::KEY_NUMPAD8;
        case kVK_ANSI_Keypad9:
            return Angle::KEY_NUMPAD9;

        case kVK_ANSI_A:
            return Angle::KEY_A;
        case kVK_ANSI_B:
            return Angle::KEY_B;
        case kVK_ANSI_C:
            return Angle::KEY_C;
        case kVK_ANSI_D:
            return Angle::KEY_D;
        case kVK_ANSI_E:
            return Angle::KEY_E;
        case kVK_ANSI_F:
            return Angle::KEY_F;
        case kVK_ANSI_G:
            return Angle::KEY_G;
        case kVK_ANSI_H:
            return Angle::KEY_H;
        case kVK_ANSI_I:
            return Angle::KEY_I;
        case kVK_ANSI_J:
            return Angle::KEY_J;
        case kVK_ANSI_K:
            return Angle::KEY_K;
        case kVK_ANSI_L:
            return Angle::KEY_L;
        case kVK_ANSI_M:
            return Angle::KEY_M;
        case kVK_ANSI_N:
            return Angle::KEY_N;
        case kVK_ANSI_O:
            return Angle::KEY_O;
        case kVK_ANSI_P:
            return Angle::KEY_P;
        case kVK_ANSI_Q:
            return Angle::KEY_Q;
        case kVK_ANSI_R:
            return Angle::KEY_R;
        case kVK_ANSI_S:
            return Angle::KEY_S;
        case kVK_ANSI_T:
            return Angle::KEY_T;
        case kVK_ANSI_U:
            return Angle::KEY_U;
        case kVK_ANSI_V:
            return Angle::KEY_V;
        case kVK_ANSI_W:
            return Angle::KEY_W;
        case kVK_ANSI_X:
            return Angle::KEY_X;
        case kVK_ANSI_Y:
            return Angle::KEY_Y;
        case kVK_ANSI_Z:
            return Angle::KEY_Z;

        case kVK_ANSI_1:
            return Angle::KEY_NUM1;
        case kVK_ANSI_2:
            return Angle::KEY_NUM2;
        case kVK_ANSI_3:
            return Angle::KEY_NUM3;
        case kVK_ANSI_4:
            return Angle::KEY_NUM4;
        case kVK_ANSI_5:
            return Angle::KEY_NUM5;
        case kVK_ANSI_6:
            return Angle::KEY_NUM6;
        case kVK_ANSI_7:
            return Angle::KEY_NUM7;
        case kVK_ANSI_8:
            return Angle::KEY_NUM8;
        case kVK_ANSI_9:
            return Angle::KEY_NUM9;
        case kVK_ANSI_0:
            return Angle::KEY_NUM0;
    }

    return Angle::Key(0);
}

static void AddNSKeyStateToEvent(Event *event, NSEventModifierFlags state)
{
	event->Key.Shift   = state & NSEventModifierFlagShift;
	event->Key.Control = state & NSEventModifierFlagControl;
    event->Key.Alt     = state & NSEventModifierFlagOption;
	event->Key.System  = state & NSEventModifierFlagCommand;
}

static MouseButton TranslateMouseButton(NSInteger button)
{
    switch (button)
    {
        case 2:
            return MOUSEBUTTON_MIDDLE;
        case 3:
            return MOUSEBUTTON_BUTTON4;
        case 4:
            return MOUSEBUTTON_BUTTON5;
        default:
            return MOUSEBUTTON_UNKNOWN;
    }
}

// Delegate for NSView events, mostly the input events
@implementation ContentView
- (id)initWithWindow:(OSXWindow *)window
{
    self = [super init];
    if (self != nil)
    {
        mWindow          = window;
        mTrackingArea    = nil;
        mCurrentModifier = 0;
        [self updateTrackingAreas];
    }
    return self;
}

- (void)dealloc
{
    [mTrackingArea release];
    [super dealloc];
}

- (void)updateTrackingAreas
{
    if (mTrackingArea != nil)
    {
        [self removeTrackingArea:mTrackingArea];
        [mTrackingArea release];
        mTrackingArea = nil;
    }

    NSRect bounds               = [self bounds];
    NSTrackingAreaOptions flags = NSTrackingMouseEnteredAndExited | NSTrackingActiveInKeyWindow |
                                  NSTrackingCursorUpdate | NSTrackingInVisibleRect |
                                  NSTrackingAssumeInside;
    mTrackingArea = [[NSTrackingArea alloc] initWithRect:bounds
                                                 options:flags
                                                   owner:self
                                                userInfo:nil];

    [self addTrackingArea:mTrackingArea];
    [super updateTrackingAreas];
}

// Helps with performance
- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

-(BOOL)acceptsFirstMouse:(NSEvent *)event
{
    return YES;
}

// Handle mouse events from the NSResponder protocol
- (float)translateMouseY:(float)y
{
    return [self frame].size.height - y;
}

- (void)addButtonEvent:(NSEvent *)nsEvent
                  type:(Event::EventType)eventType
                button:(MouseButton)button
{
    Event event;
    event.Type               = eventType;
    event.MouseButton.Button = button;
    event.MouseButton.X      = (int)[nsEvent locationInWindow].x;
    event.MouseButton.Y      = (int)[self translateMouseY:[nsEvent locationInWindow].y];
    mWindow->pushEvent(event);
}

- (void)mouseDown:(NSEvent *)event
{
    [self addButtonEvent:event type:Event::EVENT_MOUSE_BUTTON_PRESSED button:MOUSEBUTTON_LEFT];
}

- (void)mouseDragged:(NSEvent *)event
{
    [self mouseMoved:event];
}

- (void)mouseUp:(NSEvent *)event
{
    [self addButtonEvent:event type:Event::EVENT_MOUSE_BUTTON_RELEASED button:MOUSEBUTTON_LEFT];
}

- (void)mouseMoved:(NSEvent *)nsEvent
{
    Event event;
    event.Type        = Event::EVENT_MOUSE_MOVED;
    event.MouseMove.X = (int)[nsEvent locationInWindow].x;
    event.MouseMove.Y = (int)[self translateMouseY:[nsEvent locationInWindow].y];
    mWindow->pushEvent(event);
}

- (void)mouseEntered:(NSEvent *)nsEvent
{
    Event event;
    event.Type = Event::EVENT_MOUSE_ENTERED;
    mWindow->pushEvent(event);
}

- (void)mouseExited:(NSEvent *)nsEvent
{
    Event event;
    event.Type = Event::EVENT_MOUSE_LEFT;
    mWindow->pushEvent(event);
}

- (void)rightMouseDown:(NSEvent *)event
{
    [self addButtonEvent:event type:Event::EVENT_MOUSE_BUTTON_PRESSED button:MOUSEBUTTON_RIGHT];
}

- (void)rightMouseDragged:(NSEvent *)event
{
    [self mouseMoved:event];
}

- (void)rightMouseUp:(NSEvent *)event
{
    [self addButtonEvent:event type:Event::EVENT_MOUSE_BUTTON_RELEASED button:MOUSEBUTTON_RIGHT];
}

- (void)otherMouseDown:(NSEvent *)event
{
    [self addButtonEvent:event
                    type:Event::EVENT_MOUSE_BUTTON_PRESSED
                  button:TranslateMouseButton([event buttonNumber])];
}

- (void)otherMouseDragged:(NSEvent *)event
{
    [self mouseMoved:event];
}

- (void)otherMouseUp:(NSEvent *)event
{
    [self addButtonEvent:event
                    type:Event::EVENT_MOUSE_BUTTON_RELEASED
                  button:TranslateMouseButton([event buttonNumber])];
}

- (void)scrollWheel:(NSEvent *)nsEvent
{
    if (static_cast<int>([nsEvent deltaY]) == 0)
    {
        return;
    }

    Event event;
    event.Type             = Event::EVENT_MOUSE_WHEEL_MOVED;
    event.MouseWheel.Delta = (int)[nsEvent deltaY];
    mWindow->pushEvent(event);
}

- (unsigned int)getChar:(NSEvent*)nsEvent
{
    NSString *str;
    unsigned int c,mchar;
    const unsigned char *cStr;
    BOOL skipCommand;

    str = [nsEvent characters];
    if ((str != nil) && ([str length] > 0))
    {
        mchar = 0;
        c = [str characterAtIndex:0];
        mchar = c;
            
        {
            // workaround for period character
            if (c == 0x2E)
            {
                mchar = '.';
            }
            else if (c == 127)
            {
                // backspace
                mchar = '\b';
            }
            else
            {
                cStr = (unsigned char *)[str cStringUsingEncoding:NSWindowsCP1252StringEncoding];
                if (cStr != NULL && strlen((char*)cStr) > 0)
                {
                    mchar = cStr[0];
                }
                else
                {
                    mchar = 0;
                }
            }
        }
    }
    return mchar;
}

// Handle key events from the NSResponder protocol
- (void)keyDown:(NSEvent *)nsEvent
{
    // TODO(cwallez) also send text events
    Event event;
    event.Type     = Event::EVENT_KEY_PRESSED;
    event.Key.Code = NSCodeToKey([nsEvent keyCode]);
    event.Key.Char = [self getChar:nsEvent];
    AddNSKeyStateToEvent(&event, [nsEvent modifierFlags]);
    mWindow->pushEvent(event);
}

- (void)keyUp:(NSEvent *)nsEvent
{
    Event event;
    event.Type     = Event::EVENT_KEY_RELEASED;
    event.Key.Code = NSCodeToKey([nsEvent keyCode]);
    event.Key.Char = [self getChar:nsEvent];
    AddNSKeyStateToEvent(&event, [nsEvent modifierFlags]);
    mWindow->pushEvent(event);
}

// Modifier keys do not trigger keyUp/Down events but only flagsChanged events.
- (void)flagsChanged:(NSEvent *)nsEvent
{
    Event event;

    // Guess if the key has been pressed or released with the change of modifiers
    // It currently doesn't work when modifiers are unchanged, such as when pressing
    // both shift keys. GLFW has a solution for this but it requires tracking the
    // state of the keys. Implementing this is still TODO(cwallez)
    int modifier = [nsEvent modifierFlags] & NSDeviceIndependentModifierFlagsMask;
    if (modifier < mCurrentModifier)
    {
        event.Type = Event::EVENT_KEY_RELEASED;
    }
    else
    {
        event.Type = Event::EVENT_KEY_PRESSED;
    }
    mCurrentModifier = modifier;

    event.Key.Code = NSCodeToKey([nsEvent keyCode]);
    AddNSKeyStateToEvent(&event, [nsEvent modifierFlags]);
    mWindow->pushEvent(event);
}
@end

OSXWindow::OSXWindow() : mWindow(nil), mDelegate(nil), mView(nil) {}

OSXWindow::~OSXWindow()
{
    destroy();
}

bool OSXWindow::initializeImpl(const std::string &name, int width, int height)
{
    if (!InitializeAppKit())
    {
        return false;
    }

    unsigned int styleMask = NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask |
                             NSMiniaturizableWindowMask;
    mWindow = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, width, height)
                                          styleMask:styleMask
                                            backing:NSBackingStoreBuffered
                                              defer:NO];

    if (mWindow == nil)
    {
        return false;
    }

    mDelegate = [[WindowDelegate alloc] initWithWindow:this];
    if (mDelegate == nil)
    {
        return false;
    }
    [mWindow setDelegate:static_cast<id>(mDelegate)];

    mView = [[ContentView alloc] initWithWindow:this];
    if (mView == nil)
    {
        return false;
    }
    [mView setWantsLayer:YES];

    // Disable scaling for this view. If scaling is enabled, the metal backend's
    // frame buffer's size will be this window's size multiplied by contentScale.
    // It will cause inconsistent testing & example apps' results.
    mView.layer.contentsScale = 1;

    [mWindow setContentView:mView];
    [mWindow setTitle:[NSString stringWithUTF8String:name.c_str()]];
    [mWindow setAcceptsMouseMovedEvents:YES];  
    [mWindow center];

    [NSApp activateIgnoringOtherApps:YES];

    mX      = 0;
    mY      = 0;
    mWidth  = width;
    mHeight = height;

    AllWindows().insert(this);
    return true;
}

void OSXWindow::disableErrorMessageDialog() {}

void OSXWindow::destroy()
{
    AllWindows().erase(this);

    [mView release];
    mView = nil;
    [mDelegate onOSXWindowDeleted];
    [mDelegate release];
    mDelegate = nil;
    // NSWindow won't be completely released unless its content view is set to nil:
    [mWindow setContentView:nil];
    [mWindow release];
    mWindow = nil;
}

void OSXWindow::resetNativeWindow() {}

EGLNativeWindowType OSXWindow::getNativeWindow() const
{
    return [mView layer];
}

EGLNativeDisplayType OSXWindow::getNativeDisplay() const
{
    // TODO(cwallez): implement it once we have defined what EGLNativeDisplayType is
    return static_cast<EGLNativeDisplayType>(0);
}

void OSXWindow::messageLoop()
{
    @autoreleasepool
    {
        while (true)
        {
            // TODO(http://anglebug.com/6570): @try/@catch is a workaround for
            // exceptions being thrown from Cocoa-internal function
            // NS_setFlushesWithDisplayLink starting in macOS 11.
            @try
            {
                NSEvent *event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                                    untilDate:[NSDate distantPast]
                                                       inMode:NSDefaultRunLoopMode
                                                      dequeue:YES];
                if (event == nil)
                {
                    break;
                }

                if ([event type] == NSAppKitDefined)
                {
                    continue;
                }
                [NSApp sendEvent:event];
            }
            @catch (NSException *localException)
            {
                NSLog(@"*** OSXWindow discarding exception: <%@> %@", [localException name],
                      [localException reason]);
            }
        }
    }
}

void OSXWindow::setMousePosition(int x, int y)
{
    y = (int)([mWindow frame].size.height) - y - 1;
    NSPoint screenspace;

#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_7
    screenspace = [mWindow convertBaseToScreen:NSMakePoint(x, y)];
#else
    screenspace = [mWindow convertRectToScreen:NSMakeRect(x, y, 0, 0)].origin;
#endif
    CGWarpMouseCursorPosition(CGPointMake(screenspace.x, YCoordToFromCG(screenspace.y)));
}

bool OSXWindow::setOrientation(int width, int height)
{
    UNIMPLEMENTED();
    return false;
}

bool OSXWindow::setPosition(int x, int y)
{
    // Given CG and NS's coordinate system, the "Y" position of a window is the Y coordinate
    // of the bottom of the window.
    int newBottom    = (int)([mWindow frame].size.height) + y;
    NSRect emptyRect = NSMakeRect(x, YCoordToFromCG(newBottom), 0, 0);
    [mWindow setFrameOrigin:[mWindow frameRectForContentRect:emptyRect].origin];
    return true;
}

bool OSXWindow::resize(int width, int height)
{
    [mWindow setContentSize:NSMakeSize(width, height)];
    return true;
}

void OSXWindow::setVisible(bool isVisible)
{
    if (isVisible)
    {
        [mWindow makeKeyAndOrderFront:nil];
    }
    else
    {
        [mWindow orderOut:nil];
    }
}

void OSXWindow::signalTestEvent()
{
    @autoreleasepool
    {
        NSEvent *event = [NSEvent otherEventWithType:NSApplicationDefined
                                            location:NSMakePoint(0, 0)
                                       modifierFlags:0
                                           timestamp:0.0
                                        windowNumber:[mWindow windowNumber]
                                             context:nil
                                             subtype:0
                                               data1:0
                                               data2:0];
        [NSApp postEvent:event atStart:YES];
    }
}

NSWindow *OSXWindow::getNSWindow() const
{
    return mWindow;
}

void* OSXWindow::getNativeOSWindow()
{
    return mWindow;
}

// static
OSWindow *OSWindow::New()
{
    return new OSXWindow;
}

#endif
