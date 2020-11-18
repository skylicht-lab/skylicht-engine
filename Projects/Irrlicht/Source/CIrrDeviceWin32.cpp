// Copyright (C) 2002-2007 Nikolaus Gebhardt
// Copyright (C) 2007-2011 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
#include "pch.h"
#include "CIrrDeviceWin32.h"

#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_

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

		#if defined(_IRR_COMPILE_WITH_OPENGL_)
		IVideoDriver* createOpenGLDriver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io);
		#endif

		#if defined(_IRR_COMPILE_WITH_OGLES3_)
		IVideoDriver* createOGLES3Driver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io);
		#endif
	}
}

namespace irr
{	

CIrrDeviceWin32::CIrrDeviceWin32(const SIrrlichtCreationParameters& param)
	: CIrrDeviceStub(param), Focused(false), Initialized(false), Paused(true)
{
#ifdef _DEBUG
	setDebugName("CIrrDeviceWin32");
#endif

	core::stringc winversion("Windows");
	Operator = new COSOperator(winversion);

	CursorControl = new CCursorControl(this, CreationParams.WindowSize, (HWND)CreationParams.WindowId, CreationParams.Fullscreen);

	createDriver();

	if (VideoDriver)
		createGUIAndScene();
}


CIrrDeviceWin32::~CIrrDeviceWin32()
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

bool CIrrDeviceWin32::run()
{
	if (!Initialized)
		return false;

	os::Timer::tick();

	static_cast<CCursorControl*>(CursorControl)->update();

	return Initialized;
}

void CIrrDeviceWin32::yield()
{
	Sleep(1);
}

void CIrrDeviceWin32::sleep(u32 timeMs, bool pauseTimer)
{
	const bool wasStopped = Timer ? Timer->isStopped() : true;
	if (pauseTimer && !wasStopped)
		Timer->stop();

#if !defined(CYGWIN) && !defined(MINGW)
	// add to fix sleep longer than timeMS
	timeBeginPeriod(1);
#endif

	Sleep(timeMs);

#if !defined(CYGWIN) && !defined(MINGW)
	timeEndPeriod(1);
#endif

	if (pauseTimer && !wasStopped)
		Timer->start();
}

void CIrrDeviceWin32::setWindowCaption(const wchar_t* text)
{
	HWND hwnd = (HWND)CreationParams.WindowId;
	SetWindowTextW(hwnd, text);
}

bool CIrrDeviceWin32::present(video::IImage* surface, void* windowId, core::rect<s32>* srcClip)
{
	return true;
}

bool CIrrDeviceWin32::isWindowActive() const
{
	return (Focused && !Paused);
}

bool CIrrDeviceWin32::isWindowFocused() const
{
	return Focused;
}

bool CIrrDeviceWin32::isWindowMinimized() const
{
	return !Focused;
}

void CIrrDeviceWin32::closeDevice()
{
	HWND hwnd = (HWND)CreationParams.WindowId;
	PostMessage(hwnd, WM_CLOSE, 0, 0);
}

void CIrrDeviceWin32::setResizable(bool resize)
{
	if (!getVideoDriver() || CreationParams.Fullscreen)
		return;

	LONG_PTR style = WS_POPUP;
	HWND hwnd = (HWND)CreationParams.WindowId;

	if (!resize)
		style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	else
		style = WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	if (!SetWindowLongPtr(hwnd, GWL_STYLE, style))
		os::Printer::log("Could not change window style.");

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = getVideoDriver()->getScreenSize().Width;
	clientSize.bottom = getVideoDriver()->getScreenSize().Height;

	AdjustWindowRect(&clientSize, style, FALSE);

	const s32 realWidth = clientSize.right - clientSize.left;
	const s32 realHeight = clientSize.bottom - clientSize.top;

	const s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	const s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	SetWindowPos(hwnd, HWND_TOP, windowLeft, windowTop, realWidth, realHeight,
		SWP_FRAMECHANGED | SWP_NOMOVE | SWP_SHOWWINDOW);
}

void CIrrDeviceWin32::minimizeWindow()
{
	HWND hwnd = (HWND)CreationParams.WindowId;

	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwnd, &wndpl);
	wndpl.showCmd = SW_SHOWMINNOACTIVE;
	SetWindowPlacement(hwnd, &wndpl);
}

void CIrrDeviceWin32::maximizeWindow()
{
	HWND hwnd = (HWND)CreationParams.WindowId;

	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwnd, &wndpl);
	wndpl.showCmd = SW_SHOWMAXIMIZED;
	SetWindowPlacement(hwnd, &wndpl);
}

void CIrrDeviceWin32::restoreWindow()
{
	HWND hwnd = (HWND)CreationParams.WindowId;

	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwnd, &wndpl);
	wndpl.showCmd = SW_SHOWNORMAL;
	SetWindowPlacement(hwnd, &wndpl);
}

core::position2di CIrrDeviceWin32::getWindowPosition()
{
	return core::position2di(0, 0);
}

E_DEVICE_TYPE CIrrDeviceWin32::getType() const
{
	return EIDT_WIN32;
}

void CIrrDeviceWin32::createDriver()
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
	case video::EDT_OPENGL:
		#if defined(_IRR_COMPILE_WITH_OPENGL_)
			VideoDriver = video::createOpenGLDriver(CreationParams, FileSystem);
		#else
			os::Printer::log("No OpenGL support compiled in.", ELL_ERROR);
		#endif
		break;
	default:
		os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
		break;
	}
}

video::SExposedVideoData& CIrrDeviceWin32::getExposedVideoData()
{
	return ExposedVideoData;
}

CIrrDeviceWin32::CCursorControl::CCursorControl(CIrrDeviceWin32* device, const core::dimension2d<u32>& wsize, HWND hwnd, bool fullscreen)
	: Device(device), WindowSize(wsize), InvWindowSize(0.0f, 0.0f),
	HWnd(hwnd), BorderX(0), BorderY(0),
	UseReferenceRect(false), IsVisible(true)
	, ActiveIcon(gui::ECI_NORMAL), ActiveIconStartTime(0)
{
	if (WindowSize.Width != 0)
		InvWindowSize.Width = 1.0f / WindowSize.Width;

	if (WindowSize.Height != 0)
		InvWindowSize.Height = 1.0f / WindowSize.Height;

	updateBorderSize(fullscreen, false);
	initCursors();
}

CIrrDeviceWin32::CCursorControl::~CCursorControl()
{
	for (u32 i = 0; i < Cursors.size(); ++i)
	{
		for (u32 f = 0; f < Cursors[i].Frames.size(); ++f)
		{
			DestroyCursor(Cursors[i].Frames[f].IconHW);
		}
	}
}


void CIrrDeviceWin32::CCursorControl::initCursors()
{
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_ARROW)));
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_CROSS)));
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_HAND)));
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_HELP)));
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_IBEAM)));
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_NO)));
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_WAIT)));
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_SIZEALL)));
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_SIZENESW)));
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_SIZENWSE)));
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_SIZENS)));
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_SIZEWE)));
	Cursors.push_back(CursorW32(LoadCursor(NULL, IDC_UPARROW)));
}


void CIrrDeviceWin32::CCursorControl::update()
{
	if (!Cursors[ActiveIcon].Frames.empty() && Cursors[ActiveIcon].FrameTime)
	{
		// update animated cursors. This could also be done by X11 in case someone wants to figure that out (this way was just easier to implement)
		u32 now = Device->getTimer()->getRealTime();
		u32 frame = ((now - ActiveIconStartTime) / Cursors[ActiveIcon].FrameTime) % Cursors[ActiveIcon].Frames.size();
		SetCursor(Cursors[ActiveIcon].Frames[frame].IconHW);
	}
}

//! Sets the active cursor icon
void CIrrDeviceWin32::CCursorControl::setActiveIcon(gui::ECURSOR_ICON iconId)
{
	if (iconId >= (s32)Cursors.size())
		return;

	ActiveIcon = iconId;
	ActiveIconStartTime = Device->getTimer()->getRealTime();
	if (Cursors[ActiveIcon].Frames.size())
		SetCursor(Cursors[ActiveIcon].Frames[0].IconHW);
}


//! Add a custom sprite as cursor icon.
gui::ECURSOR_ICON CIrrDeviceWin32::CCursorControl::addIcon(const gui::SCursorSprite& icon)
{	
	return gui::ECI_NORMAL;
}


//! replace the given cursor icon.
void CIrrDeviceWin32::CCursorControl::changeIcon(gui::ECURSOR_ICON iconId, const gui::SCursorSprite& icon)
{

}


//! Return a system-specific size which is supported for cursors. Larger icons will fail, smaller icons might work.
core::dimension2di CIrrDeviceWin32::CCursorControl::getSupportedIconSize() const
{
	core::dimension2di result;

	result.Width = GetSystemMetrics(SM_CXCURSOR);
	result.Height = GetSystemMetrics(SM_CYCURSOR);

	return result;
}

} // end namespace irr

#endif

