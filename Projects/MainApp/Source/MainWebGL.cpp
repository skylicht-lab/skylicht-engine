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
	IVideoDriver* driver = g_device->getVideoDriver();
	g_device->run();

	g_mainApp->mainLoop();
}

/*
void wget_load(unsigned int s, void* userData, const char *path)
{
	const char *fileName = "Hero.dae";
	FILE *f = fopen(fileName, "rb");
	if (f != NULL)
	{
		char buffer[1024] = { 0 };
		printf("Open %s success - %d\n", fileName, s);
		for (int i = 0; i < 10; i++)
		{
			fgets(buffer, 1024, f);
			printf("%s\n", buffer);
			
			if (feof(f) == true)			
				break;
		}		
		fclose(f);
	}
	else
	{
		printf("Open %s failed - %d\n", fileName, s);
	}
}

void wget_status(unsigned int s, void* userData, int status)
{
	printf("%s - status: %d - %d\n", (const char*)userData, status, s);
}

void wget_process(unsigned int s, void* userData, int percent)
{
	printf("%s - process: %d - %d\n", (const char*)userData, percent, s);
}
*/
	
int main()
{		
	// Begin test --------
	MAIN_THREAD_EM_ASM(
		FS.mkdir('offline');
	    FS.mount(IDBFS, {}, 'offline');		
	);
	
	/*
	const char *url = "Data/3D/Character/Locomotion/Hero.dae";
	const char *fileName = "Hero.dae";
	const char *params = "key1=value1&key2=value2";
	void *userData = (void*)fileName;
	
	emscripten_async_wget2(url , fileName, "GET", params, userData, wget_load, wget_status, wget_process);
	*/
		
	MAIN_THREAD_EM_ASM(
		FS.syncfs(function (err) {
			// todo sync fs
		});
	);	
	// End test --------
	 
	g_mainApp = new CApplication();
	
	g_device = createDevice(video::EDT_OPENGLES, dimension2d<u32>(640, 480), 32, false, false, false, g_mainApp);

	if (!g_device)
		return 1;

	g_mainApp->initApplication(g_device);
	
	g_device->setWindowCaption(L"Skylicht Engine - Demo");

	printf("Run Main Loop\n");
	
	emscripten_set_main_loop(main_loop, 0, 1);
	
	printf("End Main Loop\n");
	
	g_mainApp->destroyApplication();
	
	g_device->drop();
	
	delete g_mainApp;
	g_mainApp = NULL;
	
	return 0;
}

#endif