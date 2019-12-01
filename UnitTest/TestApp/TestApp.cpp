/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "Base.hh"
#include "pch.h"
#include "MainTest.h"
#include "CApplication.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

CApplication *g_mainApp = NULL;
irr::IrrlichtDevice *g_device = NULL;

void main_loop()
{
	IVideoDriver* driver = g_device->getVideoDriver();
	while (g_device->run())
	{
		g_mainApp->mainLoop();
	}
}

int main(int, char**)
{
	g_mainApp = new CApplication();

	// create irrlicht device console
	SIrrlichtCreationParameters p;
	p.DeviceType = EIDT_CONSOLE;
	p.DriverType = video::EDT_NULL;
	p.Bits = (u8)32;
	p.Fullscreen = false;
	p.Stencilbuffer = false;
	p.Vsync = true;
	p.ZBufferBits = 32;
	p.AntiAlias = 1;
	p.WindowId = NULL;
	p.EventReceiver = g_mainApp;

	g_device = createDeviceEx(p);

	if (!g_device)
		return 1;

	g_device->setWindowCaption(L"Skylicht Test Demo");

	MainTest *mainTest = new MainTest();

	g_mainApp->registerAppEvent("MainTest", mainTest);

	g_mainApp->initApplication(g_device);

	main_loop();

	g_mainApp->destroyApplication();

	g_device->drop();

	delete g_mainApp;
	g_mainApp = NULL;

	TEST_ASSERT_THROW(mainTest->isPassInit());
	TEST_ASSERT_THROW(mainTest->isPassUpdate());
	TEST_ASSERT_THROW(mainTest->isPassRender());
	TEST_ASSERT_THROW(mainTest->isPassPostRender());
	TEST_ASSERT_THROW(mainTest->isPassQuitApp());

	delete mainTest;

	return 0;
}