#include "pch.h"

#ifdef SDL

#if defined(WIN32) || defined(CYGWIN) || defined(MINGW)
// for HINSTANCE
#include <Windows.h>
#endif

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

#if defined(CYGWIN) || defined(MINGW)
int CALLBACK WinMain(
	HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow
)
#elif defined(WIN32)
// Visual Studio Main Function
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{	
	g_mainApp = new CApplication();

	g_device = createDevice(video::EDT_OPENGL, dimension2d<u32>(640, 480), 32, false, false, false, g_mainApp);

	if (!g_device)
		return 1;

	g_device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");

	g_mainApp->initApplication(g_device);

	main_loop();	

	g_mainApp->destroyApplication();
	
	g_device->drop();

	delete g_mainApp;
	g_mainApp = NULL;

	return 0;
}

#endif