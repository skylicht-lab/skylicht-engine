#include "pch.h"

#ifndef TEST_APP

#include "SkylichtApplication.h"

#if defined(SKYLICHT_EDITOR)
// Generated in CMakeLists.txt
#include "ProjectPath.h"
#include "CWindowConfig.h"
#endif

using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::io;

CApplication *g_mainApp = NULL;
IrrlichtDevice *g_device = NULL;

void installApplication(const std::vector<std::string>& argv);

SkylichtApplication::SkylichtApplication(int argc, char **argv)
    :AngleApplication(std::string("Skylicht Engine"), argc, argv, 3, 0),
    m_shiftHold(false),
    m_controlHold(false),
    m_leftMouseDown(false),
    m_midMouseDown(false),
    m_rightMouseDown(false),
    m_mouseX(0),
    m_mouseY(0)
{
    g_mainApp = new CApplication();
    
#if defined(SKYLICHT_EDITOR)
    // read size from last session
    u32 x, y, w, h;
    bool maximize = false;
    bool haveConfig = false;
    
    CWindowConfig::setSaveDirectory(PROJECT_PATH);
    haveConfig = CWindowConfig::loadConfig(x, y, w, h, maximize);
    
    if (haveConfig)
    {
        mWidth = w;
        mHeight = h;
    }
    else
    {
        mWidth = 1440;
        mHeight = 900;
    }
    g_mainApp->enableWriteLog(true);
#endif
    
    std::vector<std::string> params;
    for (int i = 1; i < argc; i++)
        params.push_back(std::string(argv[i]));
    g_mainApp->setParams(params);
    
    createKeyMap();
    
    g_mainApp->setLimitFPS(60);
    g_mainApp->OnExitApplication = [&](){
        // call AngleApplication::exit
        exit();
    };
}

SkylichtApplication::~SkylichtApplication()
{

}

bool SkylichtApplication::initialize()
{
    int w = getWindow()->getWidth();
    int h = getWindow()->getHeight();

    OSWindow* window = getWindow();
    
    SIrrlichtCreationParameters p;
    p.DriverType = video::EDT_OPENGLES;
    p.WindowSize = dimension2d<u32>(w, h);
    p.Bits = (u8)32;
    p.Fullscreen = false;
    p.Stencilbuffer = false;
    p.Vsync = false;
    p.EventReceiver = g_mainApp;
    p.WindowId = window->getNativeOSWindow();
    
    g_device = createDeviceEx(p);
    
    g_mainApp->initApplication(g_device);
    
    installApplication(g_mainApp->getParams());
    
    g_mainApp->onInit();
    
    return true;
}

void SkylichtApplication::destroy()
{
#if defined(SKYLICHT_EDITOR)
    CWindowConfig::saveConfig(
        0,
        0,
        mWidth,
        mHeight,
        false
    );
#endif
    
    g_mainApp->destroyApplication();

    g_device->drop();

    delete g_mainApp;
    g_mainApp = NULL;
    g_device = NULL;
    
    AngleApplication::destroy();
}

void SkylichtApplication::step(float dt, double totalTime)
{
    AngleApplication::step(dt, totalTime);
}

void SkylichtApplication::draw()
{
    g_device->run();
    g_mainApp->mainLoop();
}

void SkylichtApplication::onKeyUp(const Event::KeyEvent &keyEvent)
{
    m_shiftHold = keyEvent.Shift;
    m_controlHold = keyEvent.Control;
    
    SKeyMap mp;
    mp.MacOSKey = keyEvent.Code;
    s32 idx = m_keyMap.binary_search(mp);

    EKEY_CODE key;
    if (idx == -1)
        key = (EKEY_CODE)0;
    else
        key = (EKEY_CODE)m_keyMap[idx].Win32Key;

    SEvent event;
    event.EventType = irr::EET_KEY_INPUT_EVENT;
    event.KeyInput.Char = keyEvent.Char;
    event.KeyInput.Key = key;
    event.KeyInput.PressedDown = false;
    event.KeyInput.Shift = m_shiftHold;
    event.KeyInput.Control = m_controlHold;
    
    IrrlichtDevice *dev  = getIrrlichtDevice();
    if (dev)
        dev->postEventFromUser(event);
}

void SkylichtApplication::onKeyDown(const Event::KeyEvent &keyEvent)
{
    m_shiftHold = keyEvent.Shift;
    m_controlHold = keyEvent.Control;
    
    SKeyMap mp;
    mp.MacOSKey = keyEvent.Code;
    s32 idx = m_keyMap.binary_search(mp);

    EKEY_CODE key;
    if (idx == -1)
        key = (EKEY_CODE)0;
    else
        key = (EKEY_CODE)m_keyMap[idx].Win32Key;

    SEvent event;
    event.EventType = irr::EET_KEY_INPUT_EVENT;
    event.KeyInput.Char = keyEvent.Char;
    event.KeyInput.Key = key;
    event.KeyInput.PressedDown = true;
    event.KeyInput.Shift = m_shiftHold;
    event.KeyInput.Control = m_controlHold;
    
    IrrlichtDevice *dev  = getIrrlichtDevice();
    if (dev)
        dev->postEventFromUser(event);
}

void SkylichtApplication::onMouseMoved(const Event::MouseMoveEvent &mouseEvent)
{
    irr::SEvent event;
    event.EventType = irr::EET_MOUSE_INPUT_EVENT;
    event.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
    event.MouseInput.X = mouseEvent.X;
    event.MouseInput.Y = mouseEvent.Y;
    event.MouseInput.Shift = m_shiftHold;
    event.MouseInput.Control = m_controlHold;
    event.MouseInput.ID = 0;
    
    m_mouseX = mouseEvent.X;
    m_mouseY = mouseEvent.Y;
    
    if (m_leftMouseDown)
        event.MouseInput.ButtonStates |= irr::EMBSM_LEFT;
    
    if (m_midMouseDown)
        event.MouseInput.ButtonStates |= irr::EMBSM_MIDDLE;
    
    if (m_rightMouseDown)
        event.MouseInput.ButtonStates |= irr::EMBSM_RIGHT;
    
    IrrlichtDevice *dev  = getIrrlichtDevice();
    if (dev)
    {
        dev->getCursorControl()->updateInternalCursorPosition(m_mouseX, m_mouseY);
        dev->postEventFromUser(event);
    }
}

void SkylichtApplication::onMouseButtonPressed(const Event::MouseButtonEvent &mouseEvent)
{
    bool sendEvent = false;
    
    irr::SEvent event;
    event.EventType = irr::EET_MOUSE_INPUT_EVENT;
    
    if (mouseEvent.Button == MOUSEBUTTON_LEFT)
    {
        event.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
        m_leftMouseDown = true;
        sendEvent = true;
    }
    else if (mouseEvent.Button == MOUSEBUTTON_MIDDLE)
    {
        event.MouseInput.Event = irr::EMIE_MMOUSE_PRESSED_DOWN;
        m_midMouseDown = true;
        sendEvent = true;
    }
    else if (mouseEvent.Button == MOUSEBUTTON_RIGHT)
    {
        event.MouseInput.Event = irr::EMIE_RMOUSE_PRESSED_DOWN;
        m_rightMouseDown = true;
        sendEvent = true;
    }
    
    event.MouseInput.X = mouseEvent.X;
    event.MouseInput.Y = mouseEvent.Y;
    event.MouseInput.Shift = m_shiftHold;
    event.MouseInput.Control = m_controlHold;
    event.MouseInput.ID = 0;
    
    if (sendEvent == true)
    {
        IrrlichtDevice *dev  = getIrrlichtDevice();
        if (dev)
            dev->postEventFromUser(event);
    }
}

void SkylichtApplication::onMouseButtonRelease(const Event::MouseButtonEvent &mouseEvent)
{
    bool sendEvent = false;
    
    irr::SEvent event;
    event.EventType = irr::EET_MOUSE_INPUT_EVENT;
    
    if (mouseEvent.Button == MOUSEBUTTON_LEFT)
    {
        event.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
        m_leftMouseDown = false;
        sendEvent = true;
    }
    else if (mouseEvent.Button == MOUSEBUTTON_MIDDLE)
    {
        event.MouseInput.Event = irr::EMIE_MMOUSE_LEFT_UP;
        m_midMouseDown = false;
        sendEvent = true;
    }
    else if (mouseEvent.Button == MOUSEBUTTON_RIGHT)
    {
        event.MouseInput.Event = irr::EMIE_RMOUSE_LEFT_UP;
        m_leftMouseDown = false;
        sendEvent = true;
    }
    
    event.MouseInput.X = mouseEvent.X;
    event.MouseInput.Y = mouseEvent.Y;
    event.MouseInput.Shift = m_shiftHold;
    event.MouseInput.Control = m_controlHold;
    event.MouseInput.ID = 0;
    
    if (sendEvent == true)
    {
        IrrlichtDevice *dev  = getIrrlichtDevice();
        if (dev)
            dev->postEventFromUser(event);
    }
}

void SkylichtApplication::onWheel(const Event::MouseWheelEvent &wheelEvent)
{
    irr::SEvent event;
    event.EventType = irr::EET_MOUSE_INPUT_EVENT;
    event.MouseInput.Event = irr::EMIE_MOUSE_WHEEL;
    
    event.MouseInput.X = m_mouseX;
    event.MouseInput.Y = m_mouseY;
    event.MouseInput.Shift = m_shiftHold;
    event.MouseInput.Control = m_controlHold;
    event.MouseInput.ID = 0;
    
    if (wheelEvent.Delta < 0)
        event.MouseInput.Wheel = 1.0f;
    else
        event.MouseInput.Wheel = -1.0f;
    
    IrrlichtDevice *dev  = getIrrlichtDevice();
    if (dev)
        dev->postEventFromUser(event);
}

void SkylichtApplication::onResized(int width, int height)
{
    irr::SEvent event;
    event.EventType = irr::EET_GAME_RESIZE;
    event.UserEvent.UserData1 = (s32)width;
    event.UserEvent.UserData2 = (s32)height;
    
    IrrlichtDevice *dev  = getIrrlichtDevice();
    if (dev)
        dev->postEventFromUser(event);
}

void SkylichtApplication::createKeyMap()
{
    // I don't know if this is the best method  to create
    // the lookuptable, but I'll leave it like that until
    // I find a better version.

    m_keyMap.reallocate(105);

    // buttons missing

    m_keyMap.push_back(SKeyMap(Angle::KEY_BACK, irr::KEY_BACK));
    m_keyMap.push_back(SKeyMap(Angle::KEY_TAB, irr::KEY_TAB));
    //m_keyMap.push_back(SKeyMap(Angle::KEY_CLEAR, irr::KEY_CLEAR));
    m_keyMap.push_back(SKeyMap(Angle::KEY_RETURN, irr::KEY_RETURN));

    // combined modifiers missing

    m_keyMap.push_back(SKeyMap(Angle::KEY_PAUSE, irr::KEY_PAUSE));
    //m_keyMap.push_back(SKeyMap(Angle::KEY_CAPSLOCK, irr::KEY_CAPITAL));

    // asian letter keys missing

    m_keyMap.push_back(SKeyMap(Angle::KEY_ESCAPE, irr::KEY_ESCAPE));

    // asian letter keys missing

    m_keyMap.push_back(SKeyMap(Angle::KEY_SPACE, irr::KEY_SPACE));
    m_keyMap.push_back(SKeyMap(Angle::KEY_PAGEUP, irr::KEY_PRIOR));
    m_keyMap.push_back(SKeyMap(Angle::KEY_PAGEDOWN, irr::KEY_NEXT));
    m_keyMap.push_back(SKeyMap(Angle::KEY_END, irr::KEY_END));
    m_keyMap.push_back(SKeyMap(Angle::KEY_HOME, irr::KEY_HOME));
    m_keyMap.push_back(SKeyMap(Angle::KEY_LEFT, irr::KEY_LEFT));
    m_keyMap.push_back(SKeyMap(Angle::KEY_UP, irr::KEY_UP));
    m_keyMap.push_back(SKeyMap(Angle::KEY_RIGHT, irr::KEY_RIGHT));
    m_keyMap.push_back(SKeyMap(Angle::KEY_DOWN, irr::KEY_DOWN));

    // select missing
    // m_keyMap.push_back(SKeyMap(Angle::KEY_PRINTSCREEN, irr::KEY_PRINT));
    // execute missing
    // m_keyMap.push_back(SKeyMap(Angle::KEY_PRINTSCREEN, irr::KEY_SNAPSHOT));

    m_keyMap.push_back(SKeyMap(Angle::KEY_INSERT, irr::KEY_INSERT));
    m_keyMap.push_back(SKeyMap(Angle::KEY_DELETE, irr::KEY_DELETE));
    // m_keyMap.push_back(SKeyMap(Angle::KEY_HELP, irr::KEY_HELP));

    m_keyMap.push_back(SKeyMap(Angle::KEY_NUM0, irr::KEY_KEY_0));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUM1, irr::KEY_KEY_1));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUM2, irr::KEY_KEY_2));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUM3, irr::KEY_KEY_3));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUM4, irr::KEY_KEY_4));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUM5, irr::KEY_KEY_5));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUM6, irr::KEY_KEY_6));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUM7, irr::KEY_KEY_7));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUM8, irr::KEY_KEY_8));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUM9, irr::KEY_KEY_9));

    m_keyMap.push_back(SKeyMap(Angle::KEY_A, irr::KEY_KEY_A));
    m_keyMap.push_back(SKeyMap(Angle::KEY_B, irr::KEY_KEY_B));
    m_keyMap.push_back(SKeyMap(Angle::KEY_C, irr::KEY_KEY_C));
    m_keyMap.push_back(SKeyMap(Angle::KEY_D, irr::KEY_KEY_D));
    m_keyMap.push_back(SKeyMap(Angle::KEY_E, irr::KEY_KEY_E));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F, irr::KEY_KEY_F));
    m_keyMap.push_back(SKeyMap(Angle::KEY_G, irr::KEY_KEY_G));
    m_keyMap.push_back(SKeyMap(Angle::KEY_H, irr::KEY_KEY_H));
    m_keyMap.push_back(SKeyMap(Angle::KEY_I, irr::KEY_KEY_I));
    m_keyMap.push_back(SKeyMap(Angle::KEY_J, irr::KEY_KEY_J));
    m_keyMap.push_back(SKeyMap(Angle::KEY_K, irr::KEY_KEY_K));
    m_keyMap.push_back(SKeyMap(Angle::KEY_L, irr::KEY_KEY_L));
    m_keyMap.push_back(SKeyMap(Angle::KEY_M, irr::KEY_KEY_M));
    m_keyMap.push_back(SKeyMap(Angle::KEY_N, irr::KEY_KEY_N));
    m_keyMap.push_back(SKeyMap(Angle::KEY_O, irr::KEY_KEY_O));
    m_keyMap.push_back(SKeyMap(Angle::KEY_P, irr::KEY_KEY_P));
    m_keyMap.push_back(SKeyMap(Angle::KEY_Q, irr::KEY_KEY_Q));
    m_keyMap.push_back(SKeyMap(Angle::KEY_R, irr::KEY_KEY_R));
    m_keyMap.push_back(SKeyMap(Angle::KEY_S, irr::KEY_KEY_S));
    m_keyMap.push_back(SKeyMap(Angle::KEY_T, irr::KEY_KEY_T));
    m_keyMap.push_back(SKeyMap(Angle::KEY_U, irr::KEY_KEY_U));
    m_keyMap.push_back(SKeyMap(Angle::KEY_V, irr::KEY_KEY_V));
    m_keyMap.push_back(SKeyMap(Angle::KEY_W, irr::KEY_KEY_W));
    m_keyMap.push_back(SKeyMap(Angle::KEY_X, irr::KEY_KEY_X));
    m_keyMap.push_back(SKeyMap(Angle::KEY_Y, irr::KEY_KEY_Y));
    m_keyMap.push_back(SKeyMap(Angle::KEY_Z, irr::KEY_KEY_Z));

    // TODO:
    //m_keyMap.push_back(SKeyMap(Angle::KEY_LSUPER, irr::KEY_LWIN));
    // TODO:
    //m_keyMap.push_back(SKeyMap(Angle::KEY_RSUPER, irr::KEY_RWIN));
    // apps missing
    //m_keyMap.push_back(SKeyMap(Angle::KEY_POWER, irr::KEY_SLEEP)); //??

    m_keyMap.push_back(SKeyMap(Angle::KEY_NUMPAD0, irr::KEY_NUMPAD0));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUMPAD1, irr::KEY_NUMPAD1));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUMPAD2, irr::KEY_NUMPAD2));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUMPAD3, irr::KEY_NUMPAD3));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUMPAD4, irr::KEY_NUMPAD4));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUMPAD5, irr::KEY_NUMPAD5));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUMPAD6, irr::KEY_NUMPAD6));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUMPAD7, irr::KEY_NUMPAD7));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUMPAD8, irr::KEY_NUMPAD8));
    m_keyMap.push_back(SKeyMap(Angle::KEY_NUMPAD9, irr::KEY_NUMPAD9));
    m_keyMap.push_back(SKeyMap(Angle::KEY_MULTIPLY, irr::KEY_MULTIPLY));
    m_keyMap.push_back(SKeyMap(Angle::KEY_ADD, irr::KEY_ADD));
    m_keyMap.push_back(SKeyMap(Angle::KEY_SUBTRACT, irr::KEY_SUBTRACT));
    m_keyMap.push_back(SKeyMap(Angle::KEY_PERIOD, irr::KEY_DECIMAL));
    m_keyMap.push_back(SKeyMap(Angle::KEY_DIVIDE, irr::KEY_DIVIDE));

    m_keyMap.push_back(SKeyMap(Angle::KEY_F1, irr::KEY_F1));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F2, irr::KEY_F2));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F3, irr::KEY_F3));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F4, irr::KEY_F4));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F5, irr::KEY_F5));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F6, irr::KEY_F6));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F7, irr::KEY_F7));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F8, irr::KEY_F8));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F9, irr::KEY_F9));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F10, irr::KEY_F10));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F11, irr::KEY_F11));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F12, irr::KEY_F12));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F13, irr::KEY_F13));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F14, irr::KEY_F14));
    m_keyMap.push_back(SKeyMap(Angle::KEY_F15, irr::KEY_F15));
    // no higher F-keys

    // TODO:
    //m_keyMap.push_back(SKeyMap(Angle::KEY_NUMLOCK, irr::KEY_NUMLOCK));
    //m_keyMap.push_back(SKeyMap(Angle::KEY_SCROLLLOCK, irr::KEY_SCROLL));
    m_keyMap.push_back(SKeyMap(Angle::KEY_LSHIFT, irr::KEY_LSHIFT));
    m_keyMap.push_back(SKeyMap(Angle::KEY_RSHIFT, irr::KEY_RSHIFT));
    m_keyMap.push_back(SKeyMap(Angle::KEY_LCONTROL, irr::KEY_LCONTROL));
    m_keyMap.push_back(SKeyMap(Angle::KEY_RCONTROL, irr::KEY_RCONTROL));
    m_keyMap.push_back(SKeyMap(Angle::KEY_LALT, irr::KEY_LMENU));
    m_keyMap.push_back(SKeyMap(Angle::KEY_RALT, irr::KEY_RMENU));

    //m_keyMap.push_back(SKeyMap(Angle::KEY_PLUS, irr::KEY_PLUS));
    m_keyMap.push_back(SKeyMap(Angle::KEY_COMMA, irr::KEY_COMMA));
    //m_keyMap.push_back(SKeyMap(Angle::KEY_MINUS, irr::KEY_MINUS));
    m_keyMap.push_back(SKeyMap(Angle::KEY_PERIOD, irr::KEY_PERIOD));

    // some special keys missing

    m_keyMap.sort();
}

#else

void exitOSXApp()
{
    
}

#endif
