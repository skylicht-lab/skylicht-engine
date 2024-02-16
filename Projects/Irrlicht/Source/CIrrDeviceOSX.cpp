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

void OSXSetWindow(void* window);
void OSXSetDefaultCaption();
void OSXSetWindowCaption(const wchar_t* caption);
void OSXClose();
void OSXSetResizable(bool resize);
void OSXMinimizeWindow();
void OSXMaximizeWindow();
void OSXRestoreWindow();
void OSXGetWindowPosition(int& x, int &y);
void OSXSetCursorVisible(bool visible);
void OSXSetMouseLocation(int x, int y);
void OSXSetCursorIcon(const char *icon);

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
    
    CursorControl = new CCursorControl(CreationParams.WindowSize, this);
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
    struct timespec ts = {0,1};
    nanosleep(&ts, NULL);
}

void CIrrDeviceOSX::sleep(u32 timeMs, bool pauseTimer)
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

void CIrrDeviceOSX::setMouseLocation(int x, int y)
{
    OSXSetMouseLocation(x, y);
}

void CIrrDeviceOSX::setCursorVisible(bool visible)
{
    OSXSetCursorVisible(visible);
}

void CIrrDeviceOSX::setCursorIcon(gui::ECURSOR_ICON iconId)
{
    OSXSetCursorIcon(gui::GUICursorIconNames[iconId]);
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

