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

#ifdef ANDROID
#include <android/log.h>

#include "stdafx.h"
#include "CApplication.h"
#include "JavaClassDefined.h"

#include "BuildConfig/CBuildConfig.h"

void installApplication(const std::vector<std::string>& argv);

extern "C" {

	bool g_appRun = false;
	bool g_appRelease = false;
	bool g_appRestart = false;

	CApplication* g_myApp = NULL;
	IrrlichtDevice* g_irrDevice = NULL;

	int g_width = -1;
	int g_height = -1;

	void applicationExitApp();

	void applicationInitApp(int width, int height)
	{
		if (g_width == -1 || g_height == -1)
		{
			g_width = width;
			g_height = height;
		}

		if (g_appRelease == true)
		{
			applicationExitApp();
			g_appRelease = false;
		}

		if (g_myApp == NULL)
		{
			if (g_appRun == true)
			{
				__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Restart Application");
			}

			g_myApp = new CApplication();

			// create irrlicht device (fix 32 bit texture)
			g_irrDevice = createDevice(irr::video::EDT_OPENGLES, core::dimension2d<u32>(g_width, g_height), 32, false, false, false, g_myApp);

			if (g_irrDevice == NULL)
			{
				__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Can not create Irrlicht Device");
				return;
			}

			__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Init Application");

			// init application
			g_myApp->initApplication(g_irrDevice);

			// accelerometer
			g_myApp->setAccelerometerSupport(true);
			g_myApp->setAccelerometerEnable(true);

			// run app
			g_appRun = true;
			g_appRelease = false;

			// install app
			installApplication(g_myApp->getParams());
			
			g_myApp->onInit();

			__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Init Application Success");
		}
	}

	void applicationLoop()
	{
		// todo restart application
		if (g_appRestart == true)
		{
			// todo need release app
			g_appRelease = true;
			g_appRestart = false;

			// restart application
			applicationInitApp(g_width, g_height);
		}

		if (g_irrDevice->run())
			g_myApp->mainLoop();
	}

	void applicationRelease()
	{
		// notify release the app when device resume
		g_appRestart = true;
	}

	void applicationExitApp()
	{
		if (g_myApp)
		{
			__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Destroy Application");
			g_myApp->destroyApplication();

			__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Destroy IrrDevice");
			if (g_irrDevice)
				g_irrDevice->drop();

			delete g_myApp;
		}

		g_myApp = NULL;
		g_irrDevice = NULL;

		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "Native Application has exited");
	}

	void applicationPauseApp()
	{
		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "applicationPauseApp");
		if (g_myApp)
			g_myApp->pause();
	}

	void applicationResumeApp(int showConnecting)
	{
		__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "applicationResumeApp");
		if (g_myApp)
			g_myApp->resume(showConnecting);
	}

	void applicationResizeWindow(int w, int h)
	{
		g_width = w;
		g_height = h;

		if (g_myApp)
		{
			__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "notifyResizeWin");
			g_myApp->notifyResizeWin(w, h);
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, JNI_APPNAME, "do not notifyResizeWin (App is not init)");
		}
	}

	int applicationOnBack()
	{
		if (g_myApp)
		{
			return g_myApp->back();
		}
		return 1;
	}

	void applicationUpdateAccelerometer(float x, float y, float z)
	{
		if (g_myApp)
			g_myApp->updateAccelerometer(x, y, z);
	}

	void applicationSetAccelerometer(int b)
	{
		if (g_myApp)
			g_myApp->setAccelerometerSupport(b == 1);
	}

	void applicationTouchDown(int touchID, int x, int y)
	{
		if (g_myApp)
			g_myApp->updateTouch(touchID, x, y, 0);
	}

	void applicationTouchMove(int touchID, int x, int y)
	{
		if (g_myApp)
			g_myApp->updateTouch(touchID, x, y, 2);
	}

	void applicationTouchUp(int touchID, int x, int y)
	{
		if (g_myApp)
			g_myApp->updateTouch(touchID, x, y, 1);
	}

	void applicationSetAPK(const char *apkPath)
	{
		CBuildConfig::APKPath = apkPath;
	}

	void applicationSetSaveFolder(const char *savePath)
	{
		CBuildConfig::SaveFolder = savePath;
		CBuildConfig::ProfileFolder = savePath;
	}

	void applicationSetDownloadFolder(const char *downloadPath)
	{
		CBuildConfig::SaveFolder = downloadPath;
	}

	void applicationSetAppID(const char *id)
	{
		CBuildConfig::AppID = id;
	}

	void applicationSetDeviceID(const char *id)
	{
		char mac[512];
		strcpy(mac, id);
		for (int i = 0, n = strlen(id); i < n; i++)
		{
			if (mac[i] == ':')
				mac[i] = '_';
		}

		CBuildConfig::DeviceID = mac;
	}

	void applicationSetAndroidDeviceInfo(const char *manu, const char *model, const char *os)
	{
		CBuildConfig::Factory = manu;
		CBuildConfig::Model = model;
		CBuildConfig::OSVersion = os;
	}	
}
#endif