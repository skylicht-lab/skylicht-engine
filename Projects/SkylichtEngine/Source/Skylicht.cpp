// Copyright (C) 2012 Pham Hong Duc
// This file is part of the "Skylicht Technologies".

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
#include "Graphics/CGraphics.h"

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
	CGraphics::createGetInstance();



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



	CGraphics::releaseInstance();
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

// getIrrlichtDevice
IrrlichtDevice* getIrrlichtDevice()
{
	return g_device;
}

// getVideoDriver
IVideoDriver* getVideoDriver()
{
	return g_video;
}

// getTimeStep
float getTimeStep()
{
	return g_timestep;
}

// setTimeStep
void setTimeStep(float timestep)
{
	g_timestep = timestep;
}

}
