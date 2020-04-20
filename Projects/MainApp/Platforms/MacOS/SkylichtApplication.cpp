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
    :AngleApplication(std::string("Skylicht Engine - DEMO"), argc, argv, 3, 0)
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
    
    g_device->run();
    g_mainApp->mainLoop();
}

void SkylichtApplication::draw()
{
    AngleApplication::draw();
}
