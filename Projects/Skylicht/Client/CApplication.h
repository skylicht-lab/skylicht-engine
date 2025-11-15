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
#include <functional>

namespace Skylicht
{
	/**
	 * @brief Class that manages the lifecycle and events of the application in Skylicht Engine.
	 *
	 * CApplication inherits from IEventReceiver and CBaseApp. It is responsible for initialization, main loop management,
	 * event processing, input device management (touch, joystick, accelerometer), and system operations related to the application.
	 */
	class CApplication :
		public IEventReceiver,
		public CBaseApp
	{
	public:
		/// Last update time(milliseconds)
		unsigned long m_lastUpdateTime;

		/// Current limit FPS value
		int m_fps;

		/// Callback for application exit (use on MacOS)
		std::function<void()> OnExitApplication;

	protected:
		int m_width;
		int m_height;

		/// Flag indicating whether the application is running.
		bool m_runGame;

		/// Command line arguments for the application.
		std::vector<std::string> m_argv;

	public:
		/**
		 * @brief Constructor for CApplication.
		 */
		CApplication();

		/**
		 * @brief Destructor for CApplication, releases resources.
		 */
		virtual ~CApplication();

		/**
		 * @brief Handle input events.
		 * @param event The incoming event.
		 * @return Return true if the event is handled, otherwise false.
		 */
		virtual bool OnEvent(const SEvent& event);

		/**
		 * @brief Returns a pointer to CBaseApp (for multiple inheritance).
		 * @return Pointer to CBaseApp.
		 */
		CBaseApp* getBaseApp()
		{
			return static_cast<CBaseApp*>(this);
		}

		/**
		 * @brief Set command line parameters for the application.
		 * @param argv List of parameters.
		 */
		void setParams(const std::vector<std::string>& argv);

		/**
		 * @brief Get the list of command line parameters passed to the application.
		 * @return List of parameters.
		 */
		const std::vector<std::string>& getParams();

		/**
		 * @brief Initialize the application with an Irrlicht device.
		 * @param device Pointer to IrrlichtDevice.
		 */
		void initApplication(IrrlichtDevice* device);

		/**
		 * @brief Initialize the application external plugin.
		 */
		void initPlugin();

		/**
		 * @brief Destroy the application and release resources.
		 */
		void destroyApplication();

		/**
		 * @brief Main loop of the application. Calls update, render, and handles events.
		 */
		void mainLoop();

		/**
		 * @brief Send initialization event to application receivers.
		 */
		void onInit();

		/**
		* @brief Handle application close event.
		* @return Return true if can close, false if should keep running.
		*/
		bool onClose();

		/**
		 * @brief Handle Back event (commonly used on mobile).
		 * @return Returns 1 if handled, or value from receivers.
		 */
		int back();

		/**
		 * @brief Pause the application.
		 */
		void pause();

		/**
		 * @brief Resume the application after pause.
		 * @param showConnecting Indicates whether to show reconnecting UI.
		 */
		void resume(int showConnecting = 1);

		/**
		 * @brief Reset touch (screen touch) state.
		 */
		void resetTouch();

		/**
		 * @brief Notify window size change.
		 * @param w New width.
		 * @param h New height.
		 */
		void notifyResizeWin(int w, int h);

		/**
		 * @brief Update touch event (screen interaction).
		 * @param touchID Touch ID.
		 * @param x X coordinate.
		 * @param y Y coordinate.
		 * @param action Event type: 0 = TouchDown, 1 = TouchUp, 2 = TouchMove.
		 */
		void updateTouch(long touchID, int x, int y, int action);

		/**
		 * @brief Update joystick event (game controller).
		 * @param deviceID Device ID.
		 * @param key Key pressed/released.
		 * @param action Event type: 0 = Press, 1 = Release.
		 */
		void updateJoystick(int deviceID, int key, int action);

		/**
		* @brief Update analog event from joystick.
		* @param deviceID Device ID.
		* @param id Analog ID.
		* @param x X axis value.
		* @param y Y axis value.
		*/
		void updateAnalog(int deviceID, int id, float x, float y);

		/**
		 * @brief Update accelerometer values (motion sensor).
		 * @param x X axis.
		 * @param y Y axis.
		 * @param z Z axis.
		 */
		void updateAccelerometer(float x, float y, float z);

		/**
		 * @brief Set accelerometer support.
		 * @param b true to support, false otherwise.
		 */
		void setAccelerometerSupport(bool b);

		/**
		 * @brief Enable/disable accelerometer.
		 * @param b true to enable, false to disable.
		 */
		void setAccelerometerEnable(bool b);

		/**
		 * @brief Check if accelerometer is enabled.
		 * @return true if enabled, false if disabled.
		 */
		bool isAccelerometerEnable();

		/**
		* @brief Set device ID (unicode string).
		* @param string Device ID as wchar_t string.
		*/
		void setDeviceID(const wchar_t* string);

		/**
		 * @brief Set device ID (ansi string).
		 * @param string Device ID as char string.
		 */
		void setDeviceID(const char* string);

		/**
		 * @brief Enable/disable console logging.
		 * @param b true to enable, false to disable.
		 */
		void enableWriteLog(bool b);
	};

}
