/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)

#ifdef BUILD_SKYLICHT_NETWORK
#include <Ws2tcpip.h>
#include <Winsock2.h>
#endif

#include <memory.h>
#include <stdlib.h>

#include <Windows.h>

#else

#include <memory.h>
#include <unistd.h>

#ifdef BUILD_SKYLICHT_NETWORK
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netdb.h>

#define SOCKET  int
#define INVALID_SOCKET  -1
#define SOCKET_ERROR -1
#endif

#endif

// IRRLICHT 3D ENGINE
#include "irrlicht.h"
#include "IRandomizer.h"
#include "ILogger.h"
#include "irrOS.h"

using namespace irr;
using namespace irr::scene;
using namespace irr::video;

// STL C++
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <fstream>
#include <string>

#ifdef ANDROID
#include <android/log.h>
#include <jni.h>
#endif

namespace Skylicht
{
	/**
	 * @brief Get the currently active Irrlicht device.
	 * @return Pointer to the IrrlichtDevice instance used by Skylicht Engine.
	 */
	extern IrrlichtDevice* getIrrlichtDevice();

	/**
	 * @brief Get the video driver used for rendering.
	 * @return Pointer to the IVideoDriver instance.
	 */
	extern IVideoDriver* getVideoDriver();

	/**
	 * @brief Get the current time step in milliseconds, scaled by the time scale.
	 *
	 * If fixed time step is enabled, returns the fixed time step instead.
	 * @return The time step value in milliseconds.
	 */
	extern float getTimeStep();

	/**
	 * @brief Get the current time step in milliseconds, without time scale applied.
	 * @return The raw (unscaled) time step value in milliseconds.
	 */
	extern float getNonScaledTimestep();

	/**
	 * @brief Get the total elapsed application time in milliseconds.
	 * @return The total time since application start.
	 */
	extern float getTotalTime();

	/**
	 * @brief Enable or disable the use of fixed time step for updates.
	 *
	 * When enabled, the engine will use a fixed time step for all updates instead of the dynamically calculated time step.
	 * @param b True to enable fixed time step, false to disable.
	 */
	extern void enableFixedTimeStep(bool b);

	/**
	 * @brief Set the value of the fixed time step in milliseconds.
	 *
	 * @param s The fixed time step to use.
	 */
	extern void setFixedTimeStep(float s);

	/**
	 * @brief Set the time scale multiplier for time-based updates.
	 *
	 * This allows for slow-motion or speed-up effects by scaling the time step value.
	 * @param scale The time scale multiplier.
	 */
	extern void setTimeScale(float scale);

	/**
	 * @brief Get the current time scale multiplier.
	 *
	 * @return The time scale value.
	 */
	extern float getTimeScale();

#ifdef ANDROID
	/*
	* @brief Get the JNI environment for Android platform.
	*
	* @return Pointer to the JNIEnv instance.
	*/
	extern JNIEnv* getJniEnv();

	/*
	* @brief Set the JNI environment for Android platform.
	*
	* @param env Pointer to the JNIEnv instance.
	*/
	extern void setJniEnv(JNIEnv* env);

	/*
	* @brief Get the main activity object for Android platform.
	*
	* @return The jobject representing the main activity.
	*/
	extern jobject getMainActivity();

	/*
	* @brief Set the main activity object for Android platform.
	*
	* @param activity The jobject representing the main activity.
	*/
	extern void setMainActivity(jobject activity);
#endif
}