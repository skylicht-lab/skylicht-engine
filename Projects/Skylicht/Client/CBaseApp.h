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
	/**
	 * @enum EAppEvent
	 * @brief Enumeration of application event types for event receiver system.
	 */
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

	/**
	 * @class CBaseApp
	 * @brief Base class for managing the application lifecycle, rendering, and event system in Skylicht Engine.
	 *
	 * CBaseApp handles basic properties such as device, video driver, file system, timing, rendering, and application events.
	 * It provides API to manage event receivers, screen properties, rendering, FPS control, and utility functions for resource management.
	 */
	class CBaseApp
	{
	protected:
		/// Irrlicht device pointer
		IrrlichtDevice* m_device;

		/// Video driver pointer
		IVideoDriver* m_driver;

		/// File system pointer
		io::IFileSystem* m_fileSystem;

		SExposedVideoData m_videoData;

		/// Time step for each frame(ms)
		float m_timeStep;

		/// Total elapsed time since app started(ms)
		float m_totalTime;

		/// FPS limit(-1 for unlimited)
		int m_limitFPS;

		/// Flag to display FPS in window caption
		bool m_showFPS;

		/// Time to clear screen before rendering
		float m_clearScreenTime;

		/// Flag to enable / disable rendering
		bool m_renderEnabled;

		// Flag for running logic when paused
		bool m_enableRunWhenPause;

		/// Color used to clear the screen
		video::SColor m_clearColor;

		/// Application name
		std::string m_appName;

	public:
		/**
		 * @brief Default constructor for CBaseApp.
		 */
		CBaseApp();

		/**
		 * @brief Destructor for CBaseApp.
		 */
		virtual ~CBaseApp();

		/// Type for mapping application event receivers by name.
		typedef std::pair<std::string, IApplicationEventReceiver*> appEventType;

		/// List of registered application event receivers.
		std::vector<appEventType> m_appEventReceivers;

	public:
		/**
		 * @brief Get the name of the application.
		 * @return Application name as a C-style string.
		 */
		inline const char* getAppName()
		{
			return m_appName.c_str();
		}

		/**
		 * @brief Show debug console (Windows only).
		 */
		void showDebugConsole();

		/**
		 * @brief Report memory leaks (Visual Leak Detector, debug only).
		 */
		static void reportLeakMemory();

		/**
		 * @brief Get the Irrlicht device.
		 * @return Pointer to IrrlichtDevice.
		 */
		inline IrrlichtDevice* getDevice()
		{
			return m_device;
		}

		/**
		 * @brief Get the Irrlicht video driver.
		 * @return Pointer to IVideoDriver.
		 */
		inline IVideoDriver* getDriver()
		{
			return m_driver;
		}

		/**
		 * @brief Get the Irrlicht file system.
		 * @return Pointer to IFileSystem.
		 */
		inline io::IFileSystem* getFileSystem()
		{
			return m_fileSystem;
		}

		/**
		 * @brief Set the color used to clear the screen before rendering.
		 * @param c Color value.
		 */
		void setClearColor(const video::SColor& c);

		/**
		 * @brief Get exposed video data for rendering pipeline.
		 * @return Reference to SExposedVideoData.
		 */
		inline SExposedVideoData& getVideoData()
		{
			return m_videoData;
		}

		/**
		 * @brief Get the time step for the current frame.
		 * @return Time step value in milliseconds.
		 */
		inline float getTimeStep()
		{
			return m_timeStep;
		}

		/**
		 * @brief Get built-in asset path based on platform.
		 * @param name Asset name.
		 * @return Platform-specific path.
		 */
		io::path getBuiltInPath(const char* name);

		/**
		 * @brief Get texture package name based on platform and compression support.
		 * @param name Base texture name.
		 * @return Package name (e.g., "DDS.zip", "ETC.zip").
		 */
		std::string getTexturePackageName(const char* name);

		/**
		 * @brief Show or hide FPS in window caption.
		 * @param b True to show FPS, false to hide.
		 */
		inline void showFPS(bool b)
		{
			m_showFPS = b;
		}

		/**
		 * @brief Get the screen width.
		 * @return Screen width in pixels.
		 */
		int getWidth();

		/**
		 * @brief Get the screen height.
		 * @return Screen height in pixels.
		 */
		int getHeight();

		/**
		 * @brief Get the screen aspect ratio (width/height).
		 * @return Aspect ratio as float.
		 */
		float getSizeRatio();

		/**
		 * @brief Check if screen is widescreen format.
		 * @return True if widescreen, false otherwise.
		 */
		bool isWideScreen();

		/**
		 * @brief Set the duration to clear the screen before rendering.
		 * @param t Time in milliseconds.
		 */
		inline void clearScreenTime(float t)
		{
			m_clearScreenTime = t;
		}

		/**
		 * @brief Enable or disable rendering.
		 * @param b True to enable rendering, false to disable.
		 */
		inline void enableRender(bool b)
		{
			m_renderEnabled = b;
		}

		/**
		 * @brief Check if rendering is enabled.
		 * @return True if enabled, false otherwise.
		 */
		inline bool isRenderEnabled()
		{
			return m_renderEnabled;
		}

		/**
		 * @brief Register a new application event receiver.
		 * @param name Name of the receiver.
		 * @param pEvent Pointer to IApplicationEventReceiver.
		 */
		void registerAppEvent(std::string name, IApplicationEventReceiver* pEvent);

		/**
		 * @brief Unregister an application event receiver.
		 * @param pEvent Pointer to IApplicationEventReceiver to remove.
		 */
		void unRegisterAppEvent(IApplicationEventReceiver* pEvent);

		/**
		 * @brief Send an event to all registered application event receivers.
		 * @param eventID Event type identifier.
		 * @param param1 Optional integer parameter (e.g., width for resize).
		 * @param param2 Optional integer parameter (e.g., height for resize).
		 */
		void sendEventToAppReceiver(int eventID, int param1 = 0, int param2 = 0);

		/**
		 * @brief Set the FPS limit for the application.
		 * @param fps Target FPS value, -1 for unlimited
		 */
		void setLimitFPS(int fps);

		/**
		* @brief Get the FPS limit.
		*/
		inline int getLimitFPS()
		{
			return m_limitFPS;
		}

		/**
		 * @brief Enable or disable running logic when the application is paused.
		 * @param b True to enable, false to disable.
		 */
		void enableRunWhenPause(bool b)
		{
			m_enableRunWhenPause = b;
		}
	};

	extern CBaseApp* getApplication();
}