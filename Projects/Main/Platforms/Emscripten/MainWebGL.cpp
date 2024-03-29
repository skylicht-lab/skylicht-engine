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

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/fetch.h>

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
	g_device->run();

	g_mainApp->mainLoop();
}

extern "C" void main_resize(int w, int h)
{
	if (g_mainApp != NULL)
	{
		g_mainApp->notifyResizeWin(w, h);
	}
}

void installApplication(const std::vector<std::string>& argv);

int main(int argc, char *argv[])
{
	// default params
	u32 width = 800;
	u32 height = 600;
	
	// parse params from Javascript (arguments_.push)
	if (argc == 3)
	{
		width = (u32)atoi(argv[1]);
		height = (u32)atoi(argv[2]);
	}
	
	g_mainApp = new CApplication();

	g_device = createDevice(video::EDT_OPENGLES, dimension2d<u32>(width, height), 32, false, false, false, g_mainApp);

	if (!g_device)
		return 1;

	g_mainApp->initApplication(g_device);

	installApplication(g_mainApp->getParams());

	g_mainApp->onInit();

	g_device->setWindowCaption(L"Skylicht Engine - Demo");

	emscripten_set_main_loop(main_loop, 0, 1);

	g_mainApp->destroyApplication();

	g_device->drop();

	delete g_mainApp;
	g_mainApp = NULL;

	return 0;
}
	
#endif