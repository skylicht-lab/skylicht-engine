#include "Base.hh"
#include "pch.h"
#include "CApplication.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

CApplication *g_mainApp = NULL;
irr::IrrlichtDevice *g_device = NULL;

extern bool g_finalPass;

int main(int, char**)
{
	g_mainApp = new CApplication();

	// create irrlicht device console and null driver
	SIrrlichtCreationParameters p;
	p.DeviceType = EIDT_CONSOLE;
	p.DriverType = video::EDT_NULL;
	p.EventReceiver = g_mainApp;

	g_device = createDeviceEx(p);

	if (!g_device)
		return 1;

	g_device->setWindowCaption(L"Skylicht Test Demo");

	g_mainApp->initApplication(g_device);

	IVideoDriver* driver = g_device->getVideoDriver();
	while (g_device->run())
	{
		g_mainApp->mainLoop();
	}

	g_mainApp->destroyApplication();

	g_device->drop();

	delete g_mainApp;
	g_mainApp = NULL;

	TEST_CASE("Final pass");
	TEST_ASSERT_THROW(g_finalPass);

	return 0;
}