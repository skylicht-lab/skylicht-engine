// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_PHONE_H_INCLUDED__
#define __C_IRR_DEVICE_PHONE_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if !defined(CYGWIN) && !defined(MINGW)
#include <timeapi.h>
#endif

#include "CIrrDeviceStub.h"
#include "IrrlichtDevice.h"
#include "ICursorControl.h"

namespace irr
{
	class CIrrDeviceWin32 : public CIrrDeviceStub
	{
	public:
		CIrrDeviceWin32(const SIrrlichtCreationParameters& param);

		virtual ~CIrrDeviceWin32();

		virtual bool run();

		virtual void yield();

		virtual void sleep(u32 timeMs, bool pauseTimer = false);

		virtual void setWindowCaption(const wchar_t* text);

		virtual bool present(video::IImage* surface, void* windowId, core::rect<s32>* srcClip);

		virtual bool isWindowActive() const;

		virtual bool isWindowFocused() const;

		virtual bool isWindowMinimized() const;

		virtual void closeDevice();

		virtual void setResizable(bool resize = false);

		virtual void minimizeWindow();

		virtual void maximizeWindow();

		virtual void restoreWindow();

		virtual core::position2di getWindowPosition();
		
		virtual E_DEVICE_TYPE getType() const;

	private:		

		void createDriver();

		video::SExposedVideoData& getExposedVideoData();

		bool Focused;
		bool Initialized;
		bool Paused;

		video::SExposedVideoData ExposedVideoData;
	};

} // end namespace irr

#endif // _IRR_COMPILE_WITH_ANDROID_DEVICE_
#endif // __C_IRR_DEVICE_ANDROID_H_INCLUDED__
