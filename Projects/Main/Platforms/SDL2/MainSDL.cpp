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

#include "pch.h"

#ifdef SDL

#if defined(WIN32) || defined(CYGWIN) || defined(MINGW)
#include <Windows.h> // for HINSTANCE
#endif

#include "CWindowConfig.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

CApplication* g_mainApp = NULL;
irr::IrrlichtDevice* g_device = NULL;

void main_loop()
{
	IVideoDriver* driver = g_device->getVideoDriver();
	while (g_device->run())
	{
		g_mainApp->mainLoop();
	}
}

void installApplication(const std::vector<std::string>& argv);

int main(int argc, char* argv[])
{
	g_mainApp = new CApplication();

	std::vector<std::string> params;
	for (int i = 1; i < argc; i++)
		params.push_back(std::string(argv[i]));
	g_mainApp->setParams(params);

	dimension2du size(1280, 800);

#if defined(__EMSCRIPTEN__)
	size = dimension2du(640, 480);
#endif

#if defined(SKYLICHT_EDITOR)
	// read size from last session
	u32 x, y, w, h;
	bool maximize = false;
	bool haveConfig = false;
	if (CWindowConfig::loadConfig(x, y, w, h, maximize))
	{
		size.Width = w;
		size.Height = h;
	}

	g_mainApp->enableWriteLog(true);
#endif

	g_device = createDevice(video::EDT_OPENGL, size, 32, false, false, false, g_mainApp);
	if (!g_device)
		return 1;

#if defined(SKYLICHT_EDITOR)
	g_device->setResizable(true);

	if (maximize)
		g_device->maximizeWindow();
#endif

	g_device->setWindowCaption(L"Skylicht Engine");

	g_mainApp->initApplication(g_device);

	g_mainApp->setLimitFPS(60);

	installApplication(g_mainApp->getParams());

	g_mainApp->onInit();

	main_loop();

	g_mainApp->destroyApplication();

	g_device->drop();

	delete g_mainApp;
	g_mainApp = NULL;

	return 0;
}

#endif