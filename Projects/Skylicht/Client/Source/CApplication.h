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
#include "CBaseApp.h"

namespace Skylicht
{
	class CApplication :
		public IEventReceiver,
		public CBaseApp
	{
	public:
		unsigned long m_lastUpdateTime;
		int m_fps;

		unsigned long m_pauseTime;

	protected:
		int m_width;
		int m_height;

		bool m_runGame;		

		std::vector<std::string> m_argv;

	public:
		CApplication();

		virtual ~CApplication();

		virtual bool OnEvent(const SEvent& event);

		// getBaseApp
		// for multi-inherit
		CBaseApp* getBaseApp()
		{
			return static_cast<CBaseApp*>(this);
		}

		void setParams(const std::vector<std::string>& argv);

		const std::vector<std::string>& getParams();

		void initApplication(IrrlichtDevice* device);

		void destroyApplication();

		void mainLoop();

		bool onClose();

		int back();

		void pause();

		void resume(int showConnecting = 1);

		void resetTouch();

		void notifyResizeWin(int w, int h);

		// updateTouch
		// action:
		//	+ 0: TouchDown
		//	+ 1: TouchUp
		//	+ 2: TouchMove
		void updateTouch(long touchID, int x, int y, int action);

		// updateJoystick
		// action:
		// + 0: Press
		// + 1: Release
		void updateJoystick(int deviceID, int key, int action);

		void updateAnalog(int deviceID, int id, float x, float y);

		void updateAccelerometer(float x, float y, float z);

		void setAccelerometerSupport(bool b);

		void setAccelerometerEnable(bool b);

		bool isAccelerometerEnable();

		void setDeviceID(const wchar_t* string);

		void setDeviceID(const char* string);

		void enableWriteLog(bool b);		
	};

}