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
#include "Skylicht.h"

#ifdef LINUX_SERVER

// Shader Manager
#include "Material/CShaderManager.h"

#else

// Skylicht Control
#include "Control/CTouchManager.h"
#include "Control/CAccelerometer.h"
#include "Control/CJoystick.h"

// Shader Manager
#include "Material/CShaderManager.h"
#include "Graphics2D/CGraphics2D.h"

#endif

namespace Skylicht
{

	IrrlichtDevice* g_device = NULL;
	IVideoDriver*	g_video = NULL;
	float g_timestep = 0.0f;

	void initSkylicht(IrrlichtDevice *device, bool server)
	{
		g_device = device;
		g_video = device->getVideoDriver();

#ifdef LINUX_SERVER
		// init server
		os::Printer::log("Init linux server");
#else
		os::Printer::log("Init skylicht core");

		CTouchManager::createGetInstance();
		CAccelerometer::createGetInstance();
		CJoystick::createGetInstance();

		CShaderManager::createGetInstance();
		CGraphics2D::createGetInstance();



		// alway use HW
		g_video->setMinHardwareBufferVertexCount(0);
#endif

		// reset random time
		os::Randomizer::reset(os::Timer::getRealTime());
	}

	void releaseSkylicht()
	{
#ifdef LINUX_SERVER
		os::Printer::log("Close linux server");
#else
		os::Printer::log("Close skylicht core");



		CGraphics2D::releaseInstance();
		CShaderManager::releaseInstance();

		CTouchManager::releaseInstance();
		CAccelerometer::releaseInstance();
		CJoystick::releaseInstance();
#endif
	}

	void updateSkylicht()
	{
#ifdef LINUX_SERVER

#else	

#endif
	}

	IrrlichtDevice* getIrrlichtDevice()
	{
		return g_device;
	}

	IVideoDriver* getVideoDriver()
	{
		return g_video;
	}

	float getTimeStep()
	{
		return g_timestep;
	}

	void setTimeStep(float timestep)
	{
		g_timestep = timestep;
	}

}