// Copyright (C) 2002-2007 Nikolaus Gebhardt
// Copyright (C) 2007-2011 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "CIrrDeviceLinux.h"

#ifdef _IRR_COMPILE_WITH_LINUX_DEVICE_

#include "irrOS.h"
#include "CFileSystem.h"
#include "ISceneManager.h"

#include <time.h>

namespace irr
{

CIrrDeviceLinux::CIrrDeviceLinux(const SIrrlichtCreationParameters& param)
	: CIrrDeviceStub(param), Focused(false), Initialized(false), Paused(true)
{
#ifdef _DEBUG
	setDebugName("CIrrDeviceLinux");
#endif

	createDriver();

	if (VideoDriver)
		createGUIAndScene();

	Initialized = true;
}


CIrrDeviceLinux::~CIrrDeviceLinux()
{
	if (SceneManager)
	{
		SceneManager->drop();
		SceneManager = 0;
	}

	if (VideoDriver)
	{
		VideoDriver->drop();
		VideoDriver = 0;
	}
}

bool CIrrDeviceLinux::run()
{
	if (!Initialized)
		return false;

	os::Timer::tick();

	return Initialized;
}

void CIrrDeviceLinux::yield()
{
	struct timespec ts = {0,1};
	nanosleep(&ts, NULL);
}

void CIrrDeviceLinux::sleep(u32 timeMs, bool pauseTimer)
{
	const bool wasStopped = Timer ? Timer->isStopped() : true;

	struct timespec ts;
	ts.tv_sec = (time_t) (timeMs / 1000);
	ts.tv_nsec = (long) (timeMs % 1000) * 1000000;

	if (pauseTimer && !wasStopped)
		Timer->stop();

	nanosleep(&ts, NULL);

	if (pauseTimer && !wasStopped)
		Timer->start();
}

void CIrrDeviceLinux::setWindowCaption(const wchar_t* text)
{
}

bool CIrrDeviceLinux::present(video::IImage* surface, void* windowId, core::rect<s32>* srcClip)
{
	return true;
}

bool CIrrDeviceLinux::isWindowActive() const
{
	return (Focused && !Paused);
}

bool CIrrDeviceLinux::isWindowFocused() const
{
	return Focused;
}

bool CIrrDeviceLinux::isWindowMinimized() const
{
	return !Focused;
}

void CIrrDeviceLinux::closeDevice()
{
}

void CIrrDeviceLinux::setResizable(bool resize)
{
}

void CIrrDeviceLinux::minimizeWindow()
{
}

void CIrrDeviceLinux::maximizeWindow()
{
}

void CIrrDeviceLinux::restoreWindow()
{
}

core::position2di CIrrDeviceLinux::getWindowPosition()
{
	return core::position2di(0, 0);
}

E_DEVICE_TYPE CIrrDeviceLinux::getType() const
{
	return EIDT_LINUX;
}

void CIrrDeviceLinux::createDriver()
{
	switch(CreationParams.DriverType)
	{
	case video::EDT_NULL:
		VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
		break;
	default:
		os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
		break;
	}
}

video::SExposedVideoData& CIrrDeviceLinux::getExposedVideoData()
{
	return ExposedVideoData;
}

} // end namespace irr

#endif

