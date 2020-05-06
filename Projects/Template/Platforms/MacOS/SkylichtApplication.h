#pragma once

#include "pch.h"
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
    
    struct SKeyMap
    {
        SKeyMap() {}
        SKeyMap(Angle::Key macos, irr::EKEY_CODE win32)
            : MacOSKey(macos), Win32Key(win32)
        {
        }

        Angle::Key MacOSKey;
        irr::EKEY_CODE  Win32Key;

        bool operator<(const SKeyMap& o) const
        {
            return (irr::s32)MacOSKey<(irr::s32)o.MacOSKey;
        }
    };

    core::array<SKeyMap> m_keyMap;
    
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
    
    void createKeyMap();
};
