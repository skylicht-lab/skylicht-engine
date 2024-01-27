// Copyright (C) 2002-2007 Nikolaus Gebhardt
// Copyright (C) 2007-2011 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "CIrrDeviceOSX.h"

#ifdef _IRR_COMPILE_WITH_OSX_DEVICE_

#include "irrOS.h"
#include "CFileSystem.h"
#include "ISceneManager.h"

#include "COSOperator.h"

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

extern void OSXSetWindow(void* window);
extern void OSXSetDefaultCaption();
extern void OSXSetWindowCaption(const wchar_t* caption);
extern void OSXClose();
extern void OSXSetResizable(bool resize);
extern void OSXMinimizeWindow();
extern void OSXMaximizeWindow();
extern void OSXRestoreWindow();
extern void OSXGetWindowPosition(int& x, int &y);

namespace irr
{

CIrrDeviceOSX::CIrrDeviceOSX(const SIrrlichtCreationParameters& param)
	: CIrrDeviceStub(param), Focused(false), Initialized(false), Paused(true)
{
#ifdef _DEBUG
	setDebugName("CIrrDeviceOSX");
#endif

	Operator = new COSOperator("Device Phone");

	createDriver();

	if (VideoDriver)
		createGUIAndScene();

	Initialized = true;
    
    OSXSetWindow(param.WindowId);
    OSXSetDefaultCaption();
}


CIrrDeviceOSX::~CIrrDeviceOSX()
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

bool CIrrDeviceOSX::run()
{
	if (!Initialized)
		return false;

	os::Timer::tick();

	return Initialized;
}

void CIrrDeviceOSX::yield()
{
#if defined(_IRR_WINDOW_UNIVERSAL_PLATFORM_)
	std::this_thread::yield();
#else
	struct timespec ts = {0,1};
	nanosleep(&ts, NULL);
#endif
}

void CIrrDeviceOSX::sleep(u32 timeMs, bool pauseTimer)
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

void CIrrDeviceOSX::setWindowCaption(const wchar_t* text)
{
    OSXSetWindowCaption(text);
}

bool CIrrDeviceOSX::present(video::IImage* surface, void* windowId, core::rect<s32>* srcClip)
{
	return true;
}

bool CIrrDeviceOSX::isWindowActive() const
{
	return (Focused && !Paused);
}

bool CIrrDeviceOSX::isWindowFocused() const
{
	return Focused;
}

bool CIrrDeviceOSX::isWindowMinimized() const
{
	return !Focused;
}

void CIrrDeviceOSX::closeDevice()
{
    OSXClose();
}

void CIrrDeviceOSX::setResizable(bool resize)
{
    OSXSetResizable(resize);
}

void CIrrDeviceOSX::minimizeWindow()
{
    OSXMinimizeWindow();
}

void CIrrDeviceOSX::maximizeWindow()
{
    OSXMaximizeWindow();
}

void CIrrDeviceOSX::restoreWindow()
{
    OSXRestoreWindow();
}

core::position2di CIrrDeviceOSX::getWindowPosition()
{
    int x, y;
    OSXGetWindowPosition(x, y);
	return core::position2di(x, y);
}

E_DEVICE_TYPE CIrrDeviceOSX::getType() const
{
	return EIDT_OSX;
}

void CIrrDeviceOSX::createDriver()
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

video::SExposedVideoData& CIrrDeviceOSX::getExposedVideoData()
{
	return ExposedVideoData;
}

} // end namespace irr

#endif

