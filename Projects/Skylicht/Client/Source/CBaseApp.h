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

#pragma once

#include "pch.h"
#include "IApplicationEventReceiver.h"

namespace Skylicht
{
	enum EAppEvent
	{
		AppEventUpdate,
		AppEventRender,
		AppEventPostRender,
		AppEventPause,
		AppEventResume,
		AppEventBack,
		AppEventQuit,
		AppEventInit,
		AppEventResize
	};

	class CBaseApp
	{
	protected:
		IrrlichtDevice* m_device;
		IVideoDriver* m_driver;
		io::IFileSystem* m_fileSystem;

		SExposedVideoData m_videoData;

		float m_timeStep;
		int m_limitFPS;

		bool m_showFPS;

		float m_clearScreenTime;
		bool m_enableRender;

		video::SColor m_clearColor;
	public:

		CBaseApp();
		virtual ~CBaseApp();

		typedef std::pair<std::string, IApplicationEventReceiver*> appEventType;
		std::vector<appEventType> m_appEventReceivers;

	public:

		void showDebugConsole();

		inline IrrlichtDevice* getDevice()
		{
			return m_device;
		}

		inline IVideoDriver* getDriver()
		{
			return m_driver;
		}

		inline io::IFileSystem* getFileSystem()
		{
			return m_fileSystem;
		}

		void setClearColor(const video::SColor& c);

		inline SExposedVideoData& getVideoData()
		{
			return m_videoData;
		}

		inline float getTimeStep()
		{
			return m_timeStep;
		}

		io::path getBuiltInPath(const char* name);

		inline void showFPS(bool b)
		{
			m_showFPS = b;
		}

		int getWidth();

		int getHeight();

		float getSizeRatio();

		bool isHD();

		bool isWideScreen();

		inline void clearScreenTime(float f)
		{
			m_clearScreenTime = f;
		}

		inline void enableRender(bool b)
		{
			m_enableRender = b;
		}

		inline bool isEnableRender()
		{
			return m_enableRender;
		}

		void registerAppEvent(std::string name, IApplicationEventReceiver* pEvent);

		void unRegisterAppEvent(IApplicationEventReceiver* pEvent);

		void sendEventToAppReceiver(int eventID, int param1 = 0, int param2 = 0);

		void setLimitFPS(int fps)
		{
			m_limitFPS = fps;
		}

		virtual void alertError(wchar_t* lpString);

		virtual bool yesNoQuestion(wchar_t* lpString);

		virtual void setStatusText(int part, wchar_t* lpString);
	};

	extern CBaseApp* getApplication();
}