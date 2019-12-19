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
#include "CApplication.h"

#ifdef ANDROID
#include <android/log.h>
#endif

#include "EventManager/CEventManager.h"

// Build config
#include "BuildConfig/CBuildConfig.h"

// Control
#include "Control/CTouchManager.h"
#include "Control/CAccelerometer.h"
#include "Control/CJoystick.h"

// String Imp
#include "Utils/CStringImp.h"

CBaseApp* g_app = NULL;

// external function
void installApplication(const std::vector<std::string>& argv);

namespace Skylicht
{
	CBaseApp* getApplication()
	{
		return g_app;
	}

	CApplication::CApplication() :
		m_pauseTime(0),
		m_resizeWin(true),
		m_width(0),
		m_height(0),
		m_runGame(false),
		m_showFPS(false)
	{
		g_app = this;

#if _DEBUG
		m_showFPS = true;
#endif
	}

	bool CApplication::OnEvent(const SEvent& event)
	{
		CEventManager *eventMgr = CEventManager::getInstance();
		if (eventMgr != NULL)
			eventMgr->OnEvent(event);

#if defined(WIN32) && WINAPI_FAMILY==WINAPI_FAMILY_DESKTOP_APP
		if (event.EventType == EET_KEY_INPUT_EVENT)
		{
			if (event.KeyInput.Key == KEY_F1 && event.KeyInput.PressedDown == false)
			{
				static bool s_isPause = false;

				if (s_isPause == false)
				{
					pause();
					s_isPause = true;
				}
				else
				{
					resume();
					s_isPause = false;
				}
			}
			else if (event.KeyInput.Key == KEY_BACK && event.KeyInput.PressedDown == false)
			{
				back();
			}
		}
#endif

#ifdef ANDROID
		if (event.EventType == EET_LOG_TEXT_EVENT)
		{
			__android_log_print(ANDROID_LOG_INFO, "skylicht_client.so", "%s", event.LogEvent.Text);
		}
#endif

		return false;
	}

	void CApplication::setParams(const std::vector<std::string>& argv)
	{
		m_argv = argv;
	}

	const std::vector<std::string>& CApplication::getParams()
	{
		return m_argv;
	}

	void CApplication::initApplication(IrrlichtDevice* device)
	{
		if (device == NULL)
			return;

		// init irrlicht device
		m_device = device;
		m_driver = device->getVideoDriver();
		m_fileSystem = device->getSceneManager()->getFileSystem();

		// get time
		m_lastUpdateTime = m_device->getTimer()->getTime();
		m_fps = 0;

		core::recti viewport = m_driver->getViewPort();

		m_width = viewport.getWidth();
		m_height = viewport.getHeight();

		// init skylicht component
		initSkylicht(m_device);

		// install application
		installApplication(m_argv);

		// send app init event
		sendEventToAppReceiver(AppEventInit);

		m_runGame = true;
	}

	void CApplication::destroyApplication()
	{
		// stop game
		m_runGame = false;

		// quit event
		sendEventToAppReceiver(AppEventQuit);

		// release skylicht component
		releaseSkylicht();
	}

	void CApplication::mainLoop()
	{
		if (m_runGame == false || m_device == NULL)
			return;

		m_device->getTimer()->tick();
		unsigned long now = m_device->getTimer()->getTime();
		m_timeStep = (f32)(now - m_lastUpdateTime);

		if (m_timeStep == 0.0f)
			m_timeStep = 1.0f;

		const float maxTimeStep = 1000.0f / 15.0f;
		if (m_timeStep > maxTimeStep)
		{
			printf("Warning: Low FPS: %f tpf, %f fps \n", m_timeStep, 1000.0f / m_timeStep);
			m_timeStep = maxTimeStep;
		}

		m_lastUpdateTime = now;

		// update skylicht timestep
		setTimeStep(m_timeStep);

		// skylicht update
		updateSkylicht();

		// application receiver
		sendEventToAppReceiver(AppEventUpdate);

		if (m_enableRender == true)
		{
			// clear screen
			m_driver->setRenderTarget(NULL);
			m_driver->beginScene(true, true, m_clearColor);

			// application receiver
			sendEventToAppReceiver(AppEventRender);

			// clear screen
			if (m_clearScreenTime > 0.0f)
			{
				// clear screen
				m_driver->beginScene(true, true, m_clearColor);

				m_clearScreenTime = m_clearScreenTime - m_timeStep;
				if (m_clearScreenTime < 0)
					m_clearScreenTime = 0.0f;
			}

			// game render
			sendEventToAppReceiver(AppEventPostRender);

			// draw debug fps string
			int fps = m_driver->getFPS();

			if (m_showFPS == true)
			{
				core::stringw tmp(L"Driver name: ");
				tmp += m_driver->getName();

				tmp += L" Fps: ";
				tmp += fps;

				tmp += L" Primitive: ";
				tmp += m_driver->getPrimitiveCountDrawn();

				tmp += L", texBind: ";
				tmp += m_driver->getTextureChange();

				tmp += L", numTextureLoaded: ";
				tmp += m_driver->getTextureCount();

				m_device->setWindowCaption(tmp.c_str());
			}

			m_fps = fps;

			m_driver->endScene();
		}

#if !defined(IOS)
		long sleepTime = 0;
		if (m_limitFPS > 0)
		{
			// get current time on timer
			m_device->getTimer()->tick();

			float limitTime = 1000.0f / (float)m_limitFPS;
			long  frameTime = m_device->getTimer()->getTime() - m_lastUpdateTime;
			sleepTime = (long)limitTime - frameTime;

			if (sleepTime > 0)
				m_device->sleep((u32)sleepTime);
		}
		else
		{
			m_device->yield();
		}
#endif
	}

	int CApplication::back()
	{
		if (m_runGame == true)
		{
			sendEventToAppReceiver(AppEventBack);
			os::Printer::log("CApplication::back");
			return 0;
		}

		return 1;
	}

	void CApplication::pause()
	{
		if (m_runGame == true)
		{
			m_runGame = false;

			// application receiver
			sendEventToAppReceiver(AppEventPause);
			os::Printer::log("CApplication::pause");
		}
	}

	void CApplication::resume(int showConnecting)
	{
		os::Printer::log("CApplication::resume");

		if (m_runGame == false)
		{
			os::Printer::log("Resume gamestate");

			resetTouch();

			// application receiver
			sendEventToAppReceiver(AppEventResume);
			m_runGame = true;
		}

		os::Printer::log("CApplication::finish Resume");
	}

	void CApplication::resetTouch()
	{
		CTouchManager::getInstance()->resetTouch();
	}

	void CApplication::notifyResizeWin(int w, int h)
	{
		if (m_device == NULL)
			return;

		m_width = w;
		m_height = h;

		// resize window
		m_driver->OnResize(core::dimension2du((u32)w, (u32)h));

		// resize mouse
		core::rect<s32> winRect(0, 0, w, h);
		m_device->getCursorControl()->setReferenceRect(&winRect);

		// todo auto scale
		autoScaleUI();

		m_resizeWin = true;
	}

	// updateTouch
	// action:
	//	+ 0:	TouchDown
	//	+ 1:	TouchUp
	//	+ 2:	TouchMove
	void CApplication::updateTouch(long touchID, int x, int y, int action)
	{
		if (m_device == NULL)
			return;

		CTouchManager::ETouchEvent touch;

		if (action == 0)
			touch = CTouchManager::TouchDown;
		else if (action == 1)
			touch = CTouchManager::TouchUp;
		else
			touch = CTouchManager::TouchMove;

		CTouchManager::getInstance()->touchEvent(touch, x, y, touchID);
	}

	// updateJoystick
	// action:
	// + 0: Press
	// + 1: Release
	void CApplication::updateJoystick(int deviceID, int key, int action)
	{
		CJoystick::getInstance()->keyEvent(deviceID, key, action == 0);
	}

	void CApplication::updateAnalog(int deviceID, int id, float x, float y)
	{
		CJoystick::getInstance()->analogEvent(deviceID, id, x, y);
	}

	void CApplication::updateAccelerometer(float x, float y, float z)
	{
		if (CAccelerometer::getInstance() != NULL)
			CAccelerometer::getInstance()->setVector(x, y, z);
	}

	void CApplication::setAccelerometerSupport(bool b)
	{
		CAccelerometer::getInstance()->setSupport(b);
	}

	void CApplication::setAccelerometerEnable(bool b)
	{
		CAccelerometer::getInstance()->setEnable(b);
	}

	bool CApplication::isAccelerometerEnable()
	{
		return CAccelerometer::getInstance()->isEnable();
	}

	void CApplication::setDeviceID(const wchar_t *string)
	{
		if (string == NULL)
		{
			os::Printer::log("Invalid Device ID");
			return;
		}

		char mac[1024] = { 0 };
		CStringImp::convertUnicodeToUTF8(string, mac);

		for (int i = 0, n = strlen(mac); i < n; i++)
		{
			char c = mac[i];
			bool isAnsi = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
			if (!isAnsi)
				mac[i] = '_';
		}

		char logString[1025];
		sprintf(logString, "DeviceID: %s", mac);
		os::Printer::log(logString);

		CBuildConfig::DeviceID = mac;
	}

	void CApplication::setDeviceID(const char* string)
	{
		if (string == NULL)
		{
			os::Printer::log("Invalid Device ID");
			return;
		}

		char mac[1024] = { 0 };
		strcpy(mac, string);

		for (int i = 0, n = strlen(mac); i < n; i++)
		{
			char c = mac[i];
			bool isAnsi = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
			if (!isAnsi)
				mac[i] = '_';
		}

		char logString[1025];
		sprintf(logString, "DeviceID: %s", mac);
		os::Printer::log(logString);

		CBuildConfig::DeviceID = mac;
	}
}