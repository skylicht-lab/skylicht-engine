#pragma once
#include "AngleApplication.h"

class SkylichtApplication: public AngleApplication
{
public:
    SkylichtApplication(int argc, char **argv);

    virtual ~SkylichtApplication();
    
    virtual bool initialize();
    virtual void destroy();

    virtual void step(float dt, double totalTime);
    virtual void draw();
};
