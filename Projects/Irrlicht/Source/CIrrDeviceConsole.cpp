// Copyright (C) 2002-2007 Nikolaus Gebhardt
// Copyright (C) 2007-2011 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "CIrrDeviceConsole.h"

#ifdef _IRR_COMPILE_WITH_CONSOLE_DEVICE_

#include "irrOS.h"
#include "CFileSystem.h"
#include "ISceneManager.h"

#include <time.h>

#ifdef _IRR_WINDOWS_API_
#include <Windows.h>
#endif

namespace irr
{

CIrrDeviceConsole::CIrrDeviceConsole(const SIrrlichtCreationParameters& param)
	: CIrrDeviceStub(param), Focused(false), Initialized(false), Paused(true), Close(false)
{
#ifdef _DEBUG
	setDebugName("CIrrDeviceConsole");
#endif

	createDriver();

	if (VideoDriver)
		createGUIAndScene();

	Initialized = true;
}


CIrrDeviceConsole::~CIrrDeviceConsole()
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

bool CIrrDeviceConsole::run()
{
	if (!Initialized)
		return false;

	os::Timer::tick();

	return !Close;
}

void CIrrDeviceConsole::yield()
{
#ifdef _IRR_WINDOWS_API_
	Sleep(1);
#else
	struct timespec ts = {0,0};
	nanosleep(&ts, NULL);
#endif
}

void CIrrDeviceConsole::sleep(u32 timeMs, bool pauseTimer)
{
	const bool wasStopped = Timer ? Timer->isStopped() : true;

#ifdef _IRR_WINDOWS_API_
	Sleep(timeMs);
#else
	struct timespec ts;
	ts.tv_sec = (time_t) (timeMs / 1000);
	ts.tv_nsec = (long) (timeMs % 1000) * 1000000;

	if (pauseTimer && !wasStopped)
		Timer->stop();

	nanosleep(&ts, NULL);
#endif

	if (pauseTimer && !wasStopped)
		Timer->start();
}

void CIrrDeviceConsole::setWindowCaption(const wchar_t* text)
{
}

bool CIrrDeviceConsole::present(video::IImage* surface, void* windowId, core::rect<s32>* srcClip)
{
	return true;
}

bool CIrrDeviceConsole::isWindowActive() const
{
	return (Focused && !Paused);
}

bool CIrrDeviceConsole::isWindowFocused() const
{
	return Focused;
}

bool CIrrDeviceConsole::isWindowMinimized() const
{
	return !Focused;
}

void CIrrDeviceConsole::closeDevice()
{
	Close = true;
}

void CIrrDeviceConsole::setResizable(bool resize)
{
}

void CIrrDeviceConsole::minimizeWindow()
{
}

void CIrrDeviceConsole::maximizeWindow()
{
}

void CIrrDeviceConsole::restoreWindow()
{
}

core::position2di CIrrDeviceConsole::getWindowPosition()
{
	return core::position2di(0, 0);
}

E_DEVICE_TYPE CIrrDeviceConsole::getType() const
{
	return EIDT_LINUX;
}

void CIrrDeviceConsole::createDriver()
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

video::SExposedVideoData& CIrrDeviceConsole::getExposedVideoData()
{
	return ExposedVideoData;
}

} // end namespace irr

#endif

