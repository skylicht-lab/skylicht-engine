#pragma once
#include "AngleApplication.h"

class SkylichtApplication: public AngleApplication
{
protected:
    bool m_shiftHold;
    bool m_controlHold;

    bool m_leftMouseDown;
    bool m_midMouseDown;
    bool m_rightMouseDown;
    
    int m_mouseX;
    int m_mouseY;
public:
    SkylichtApplication(int argc, char **argv);

    virtual ~SkylichtApplication();
    
    virtual bool initialize();
    virtual void destroy();

    virtual void step(float dt, double totalTime);
    virtual void draw();
    
    virtual void onKeyUp(const Event::KeyEvent &keyEvent);
    virtual void onKeyDown(const Event::KeyEvent &keyEvent);
    
    virtual void onMouseMoved(const Event::MouseMoveEvent &mouseEvent);
    virtual void onMouseButtonPressed(const Event::MouseButtonEvent &mouseEvent);
    virtual void onMouseButtonRelease(const Event::MouseButtonEvent &mouseEvent);
    
    virtual void onWheel(const Event::MouseWheelEvent &wheelEvent);
};
