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
#include "CBaseApp.h"

#include "Graphics2D/CGraphics2D.h"
#include "RenderPipeline/CBaseRP.h"
#include "BuildConfig/CBuildConfig.h"

#ifdef _DEBUG
#ifdef USE_VISUAL_LEAK_DETECTOR
#include "vld.h"
#endif
#endif

#ifdef IOS
extern "C" void application_setFPS(int fps);
#endif

#ifdef ANDROID
extern "C" void nativeInterface_openURL(const char* url);
extern "C" void nativeInterface_openApplicationSetting();
extern "C" int nativeInterface_isNetworkAvailable();
#elif defined(IOS) || defined(MACOS)
extern "C" void application_openURL(const char* url);
extern "C" void application_openApplicationSetting();
extern "C" bool application_isNetworkAvailable();
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <shellapi.h>
#elif LINUX
#include <cstdlib>
#include <string>
#endif

namespace Skylicht
{
	CBaseApp::CBaseApp() :
		m_device(NULL),
		m_driver(NULL),
		m_fileSystem(NULL),
		m_timeStep(1.0f),
		m_totalTime(0.0f),
		m_limitFPS(-1),
		m_showFPS(false),
		m_clearColor(255, 0, 0, 0),
		m_clearScreenTime(0.0f),
		m_renderEnabled(true),
		m_enableRunWhenPause(false)
	{
#ifdef USE_VISUAL_LEAK_DETECTOR
		VLDEnable();
#endif

#if defined(_WIN32)
		m_enableRunWhenPause = true;
#endif

		m_appName = "Skylicht Engine";
	}

	CBaseApp::~CBaseApp()
	{
	}

	void CBaseApp::reportLeakMemory()
	{
#ifdef USE_VISUAL_LEAK_DETECTOR
		VLDReportLeaks();
		VLDDisable();
#endif
	}

	void CBaseApp::showDebugConsole()
	{
#if defined(_WIN32) && !defined(SDL)
#if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
		AllocConsole();
		freopen("con", "w", stdout);
		freopen("con", "w", stderr);
#endif
#endif
	}

	void CBaseApp::registerAppEvent(const std::string& name, IApplicationEventReceiver* pEvent)
	{
		std::vector<appEventType>::iterator i = m_appEventReceivers.begin(), end = m_appEventReceivers.end();
		while (i != end)
		{
			if ((*i).second == pEvent)
				return;
			++i;
		}

		m_appEventReceivers.push_back(appEventType(name, pEvent));
		m_appName = name;
	}

	void CBaseApp::unRegisterAppEvent(IApplicationEventReceiver* pEvent)
	{
		std::vector<appEventType>::iterator i = m_appEventReceivers.begin(), end = m_appEventReceivers.end();
		while (i != end)
		{
			if ((*i).second == pEvent)
			{
				m_appEventReceivers.erase(i);
				return;
			}
			++i;
		}
	}

	void CBaseApp::sendEventToAppReceiver(int eventID, int param1, int param2)
	{
		std::vector<appEventType>::iterator i = m_appEventReceivers.begin(), end = m_appEventReceivers.end();
		while (i != end)
		{
			switch (eventID)
			{
			case AppEventUpdate:
				i->second->onUpdate();
				break;
			case AppEventRender:
				i->second->onRender();
				break;
			case AppEventPostRender:
				i->second->onPostRender();
				break;
			case AppEventPause:
				i->second->onPause();
				break;
			case AppEventResume:
				i->second->onResume();
				break;
			case AppEventInit:
				i->second->onInitApp();
				break;
			case AppEventQuit:
				i->second->onQuitApp();
				break;
			case AppEventResize:
				i->second->onResize(param1, param2);
			default:
				break;
			}
			++i;
		}
	}

	io::path CBaseApp::getBuiltInPath(const char* name)
	{
#ifdef __EMSCRIPTEN__
		return io::path(name);
#elif defined(WINDOWS_STORE)
		std::string assetPath = std::string("Assets\\") + std::string(name);
		return io::path(assetPath.c_str());
#elif defined(MACOS)
		std::string assetPath = CBuildConfig::getInstance()->DataFolder + std::string("//") + std::string(name);
		return io::path(assetPath.c_str());
#elif defined(ANDROID)
		// see the file FullscreenActivity.java in Android Project
		return io::path(name);
#else
		return io::path(name);
#endif
	}

	std::string CBaseApp::getTexturePackageName(const char* name, const char* ext)
	{
		std::string result = name;
#ifdef __EMSCRIPTEN__
		IVideoDriver* driver = getVideoDriver();
		if (driver->queryFeature(EVDF_TEXTURE_COMPRESSED_DXT) == true)
			result += "DDS";
		else
			result += "ETC";
#elif defined(WINDOWS_STORE)
		result += "DDS";
#elif defined(MACOS)
		result += "DDS";
#elif defined(LINUX)
		result += "DDS";
#elif defined(ANDROID)
		result += "ETC";
#elif defined(IOS)
#if defined(USE_ETC_TEXTURE)
		result += "ETC";
#else
		result += "PVR";
#endif
#else
		result += "DDS";
#endif
		result += ext;
		return result;
	}

	void CBaseApp::setClearColor(const video::SColor& c)
	{
		m_clearColor = c;
		CBaseRP::setClearColor(c);
	}

	int CBaseApp::getWidth()
	{
		return (int)m_driver->getScreenSize().Width;
	}

	int CBaseApp::getHeight()
	{
		return (int)m_driver->getScreenSize().Height;
	}

	float CBaseApp::getSizeRatio()
	{
		core::dimension2d<u32> size = m_driver->getScreenSize();

		float ratio = (float)size.Width / (float)size.Height;
		return ratio;
	}

	bool CBaseApp::isWideScreen()
	{
#ifdef LINUX_SERVER
		return false;
#else
		return CGraphics2D::getInstance()->isWideScreen();
#endif
	}

	void CBaseApp::setLimitFPS(int fps)
	{
		m_limitFPS = fps;

#ifdef IOS
		application_setFPS(fps);
#endif
	}

	void CBaseApp::openURL(const char* url)
	{
#ifdef ANDROID
		nativeInterface_openURL(url);
#elif defined(IOS) || defined(MACOS)
		application_openURL(url);
#elif defined(_WIN32) || defined(WIN32)
		ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
#elif LINUX
		std::string cmd = "xdg-open ";
		cmd += url;
		int result = system(cmd.c_str());
		(void)result;
#else
		os::Printer::log("[openWebURL] not yet implement");
#endif
	}

	bool CBaseApp::isNetworkAvailable()
	{
#ifdef ANDROID
		return nativeInterface_isNetworkAvailable() == 1;
#elif defined(IOS)
		return application_isNetworkAvailable();
#else
		return true;
#endif
	}

	void CBaseApp::openApplicationSetting()
	{
#ifdef ANDROID
		nativeInterface_openApplicationSetting();
#elif defined(IOS)
		application_openApplicationSetting();
#else
		os::Printer::log("[openApplicationSetting] not yet implement");
#endif
	}
}
