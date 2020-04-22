#include "pch.h"
#include "SkylichtApplication.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

CApplication *g_mainApp = NULL;
IrrlichtDevice *g_device = NULL;

SkylichtApplication::SkylichtApplication(int argc, char **argv)
    :AngleApplication(std::string("Skylicht Engine - DEMO"), argc, argv, 3, 0),
    m_shiftHold(false),
    m_controlHold(false),
    m_leftMouseDown(false),
    m_midMouseDown(false),
    m_rightMouseDown(false),
    m_mouseX(0),
    m_mouseY(0)
{
    g_mainApp = new CApplication();
    
    std::vector<std::string> params;
    for (int i = 1; i < argc; i++)
        params.push_back(std::string(argv[i]));
    g_mainApp->setParams(params);
}

SkylichtApplication::~SkylichtApplication()
{

}

bool SkylichtApplication::initialize()
{
    int w = getWindow()->getWidth();
    int h = getWindow()->getHeight();

    g_device = createDevice(video::EDT_OPENGLES, dimension2d<u32>(w, h), 32, false, false, false, g_mainApp);
    
    g_mainApp->initApplication(g_device);
    return true;
}

void SkylichtApplication::destroy()
{
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
}

void SkylichtApplication::onKeyDown(const Event::KeyEvent &keyEvent)
{
    m_shiftHold = keyEvent.Shift;
    m_controlHold = keyEvent.Control;
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
        dev->postEventFromUser(event);
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
    
    if (wheelEvent.Delta < 0)
        event.MouseInput.Wheel = 1.0f;
    else
        event.MouseInput.Wheel = -1.0f;
    
    IrrlichtDevice *dev  = getIrrlichtDevice();
    if (dev)
        dev->postEventFromUser(event);
}
