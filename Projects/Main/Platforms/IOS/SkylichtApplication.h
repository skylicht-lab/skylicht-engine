#pragma once

#include "pch.h"
#include "AngleApplication.h"

class SkylichtApplication: public AngleApplication
{
protected:
    
public:
    SkylichtApplication(int argc, char **argv, int width, int height);

    virtual ~SkylichtApplication();
    
    virtual bool initialize();
    virtual void destroy();

    virtual void step(float dt, double totalTime);
    virtual void draw();    
    
    virtual void onResized(int width, int height);
    
    void onTouchDown(int touchID, int x, int y);
    void onTouchMove(int touchID, int x, int y);
    void onTouchUp(int touchID, int x, int y);
    
    void setSaveFolder(const char *folder);
    void setBundleId(const char *bundleId);
};
