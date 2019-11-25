// Copyright (C) 2002-2007 Nikolaus Gebhardt
// Copyright (C) 2007-2011 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "CIrrDevicePhone.h"

#ifdef _IRR_COMPILE_WITH_PHONE_DEVICE_

#include "irrOS.h"
#include "CFileSystem.h"
#include "ISceneManager.h"

#ifdef _IRR_WINDOW_UNIVERSAL_PLATFORM_
#include <thread>
#endif

namespace irr
{
	namespace video
	{
		#if defined(_IRR_COMPILE_WITH_DIRECT3D_11_)
		IVideoDriver* createDirectX11Driver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io);
		#endif

		#if defined(_IRR_COMPILE_WITH_OGLES3_)
		IVideoDriver* createOGLES3Driver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io);
		#endif
	}
}

namespace irr
{

CIrrDevicePhone::CIrrDevicePhone(const SIrrlichtCreationParameters& param)
	: CIrrDeviceStub(param), Focused(false), Initialized(false), Paused(true)
{
#ifdef _DEBUG
	setDebugName("CIrrDevicePhone");
#endif

	createDriver();

	if (VideoDriver)
		createGUIAndScene();

	Initialized = true;
}


CIrrDevicePhone::~CIrrDevicePhone()
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

bool CIrrDevicePhone::run()
{
	if (!Initialized)
		return false;

	os::Timer::tick();

	return Initialized;
}

void CIrrDevicePhone::yield()
{
#if defined(_IRR_WINDOW_UNIVERSAL_PLATFORM_)
	std::this_thread::yield();
#else
	struct timespec ts = {0,1};
	nanosleep(&ts, NULL);
#endif
}

void CIrrDevicePhone::sleep(u32 timeMs, bool pauseTimer)
{
#if defined(_IRR_WINDOW_UNIVERSAL_PLATFORM_)
	const bool wasStopped = Timer ? Timer->isStopped() : true;

	if (pauseTimer && !wasStopped)
		Timer->stop();

	// sleep
	std::this_thread::sleep_for(std::chrono::milliseconds(timeMs));

	if (pauseTimer && !wasStopped)
		Timer->start();
#else
	const bool wasStopped = Timer ? Timer->isStopped() : true;

	struct timespec ts;
	ts.tv_sec = (time_t) (timeMs / 1000);
	ts.tv_nsec = (long) (timeMs % 1000) * 1000000;

	if (pauseTimer && !wasStopped)
		Timer->stop();

	nanosleep(&ts, NULL);

	if (pauseTimer && !wasStopped)
		Timer->start();
#endif
}

void CIrrDevicePhone::setWindowCaption(const wchar_t* text)
{
}

bool CIrrDevicePhone::present(video::IImage* surface, void* windowId, core::rect<s32>* srcClip)
{
	return true;
}

bool CIrrDevicePhone::isWindowActive() const
{
	return (Focused && !Paused);
}

bool CIrrDevicePhone::isWindowFocused() const
{
	return Focused;
}

bool CIrrDevicePhone::isWindowMinimized() const
{
	return !Focused;
}

void CIrrDevicePhone::closeDevice()
{
}

void CIrrDevicePhone::setResizable(bool resize)
{
}

void CIrrDevicePhone::minimizeWindow()
{
}

void CIrrDevicePhone::maximizeWindow()
{
}

void CIrrDevicePhone::restoreWindow()
{
}

core::position2di CIrrDevicePhone::getWindowPosition()
{
	return core::position2di(0, 0);
}

E_DEVICE_TYPE CIrrDevicePhone::getType() const
{
	return EIDT_PHONE;
}

void CIrrDevicePhone::createDriver()
{
	switch(CreationParams.DriverType)
	{	
	case video::EDT_OPENGLES:
		#if defined(_IRR_COMPILE_WITH_OGLES3_)
			VideoDriver = video::createOGLES3Driver(CreationParams, FileSystem);
		#else
			os::Printer::log("No OpenGL ES 3 support compiled in.", ELL_ERROR);
		#endif
		break;
	case video::EDT_NULL:
		VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
		break;	
	case video::EDT_DIRECT3D11:
		#if defined(_IRR_COMPILE_WITH_DIRECT3D_11_)
			VideoDriver = video::createDirectX11Driver(CreationParams, FileSystem);
		#else
			os::Printer::log("No DirectX11 support compiled in.", ELL_ERROR);
		#endif
		break;
	default:
		os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
		break;
	}
}

video::SExposedVideoData& CIrrDevicePhone::getExposedVideoData()
{
	return ExposedVideoData;
}

} // end namespace irr

#endif

