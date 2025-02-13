// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// This device code is based on the original SDL device implementation
// contributed by Shane Parker (sirshane).

#ifndef __C_IRR_DEVICE_SDL2_H_INCLUDED__
#define __C_IRR_DEVICE_SDL2_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_

#include "IrrlichtDevice.h"
#include "CIrrDeviceStub.h"
#include "ICursorControl.h"

#include <SDL2/SDL.h>

namespace irr
{

	class CIrrDeviceSDL2 : public CIrrDeviceStub
	{
	public:

		//! constructor
		CIrrDeviceSDL2(const SIrrlichtCreationParameters& param);

		//! destructor
		virtual ~CIrrDeviceSDL2();

		//! runs the device. Returns false if device wants to be deleted
		virtual bool run() _IRR_OVERRIDE_;

		//! pause execution temporarily
		virtual void yield() _IRR_OVERRIDE_;

		//! pause execution for a specified time
		virtual void sleep(u32 timeMs, bool pauseTimer) _IRR_OVERRIDE_;

		//! sets the caption of the window
		virtual void setWindowCaption(const wchar_t* text) _IRR_OVERRIDE_;

		//! returns if window is active. if not, nothing need to be drawn
		virtual bool isWindowActive() const _IRR_OVERRIDE_;

		//! returns if window has focus.
		bool isWindowFocused() const _IRR_OVERRIDE_;

		//! returns if window is minimized.
		bool isWindowMinimized() const _IRR_OVERRIDE_;

		//! returns color format of the window.
		video::ECOLOR_FORMAT getColorFormat() const _IRR_OVERRIDE_;

		//! notifies the device that it should close itself
		virtual void closeDevice() _IRR_OVERRIDE_;

		//! \return Returns a pointer to a list with all video modes supported
		virtual video::IVideoModeList* getVideoModeList() _IRR_OVERRIDE_;

		//! Sets if the window should be resizable in windowed mode.
		virtual void setResizable(bool resize = false) _IRR_OVERRIDE_;

		//! Minimizes the window.
		virtual void minimizeWindow() _IRR_OVERRIDE_;

		//! Maximizes the window.
		virtual void maximizeWindow() _IRR_OVERRIDE_;

		//! Restores the window size.
		virtual void restoreWindow() _IRR_OVERRIDE_;

		//! Checks if the Irrlicht window is running in fullscreen mode
		/** \return True if window is fullscreen. */
		virtual bool isFullscreen() const _IRR_OVERRIDE_;

		//! Get the position of this window on screen
		virtual core::position2di getWindowPosition() _IRR_OVERRIDE_;

		//! Activate any joysticks, and generate events for them.
		virtual bool activateJoysticks(core::array<SJoystickInfo> & joystickInfo) _IRR_OVERRIDE_;

		//! Set the current Gamma Value for the Display
		virtual bool setGammaRamp(f32 red, f32 green, f32 blue, f32 brightness, f32 contrast) _IRR_OVERRIDE_;

		//! Get the current Gamma Value for the Display
		virtual bool getGammaRamp(f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast) _IRR_OVERRIDE_;

		//! Get the device type
		virtual E_DEVICE_TYPE getType() const _IRR_OVERRIDE_
		{
			return EIDT_SDL;
		}

		virtual void onWindowResize(u32 w, u32 h) _IRR_OVERRIDE_;

		//! Implementation of the linux cursor control
		class CCursorControl : public gui::ICursorControl
		{
		public:

			CCursorControl(CIrrDeviceSDL2* dev);

			~CCursorControl();

			//! Changes the visible state of the mouse cursor.
			virtual void setVisible(bool visible) _IRR_OVERRIDE_
			{
				IsVisible = visible;
				if (visible)
					SDL_ShowCursor(SDL_ENABLE);
				else
				{
					SDL_ShowCursor(SDL_DISABLE);
				}
			}

			//! Returns if the cursor is currently visible.
			virtual bool isVisible() const _IRR_OVERRIDE_
			{
				return IsVisible;
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(const core::position2d<f32> &pos) _IRR_OVERRIDE_
			{
				setPosition(pos.X, pos.Y);
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(f32 x, f32 y) _IRR_OVERRIDE_
			{
				setPosition((s32)(x*Device->Width), (s32)(y*Device->Height));
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(const core::position2d<s32> &pos) _IRR_OVERRIDE_
			{
				setPosition(pos.X, pos.Y);
			}

			//! Sets the new position of the cursor.
			virtual void setPosition(s32 x, s32 y) _IRR_OVERRIDE_
			{
				SDL_WarpMouseInWindow(Device->ScreenWindow, x, y);
			}

			//! Returns the current position of the mouse cursor.
			virtual const core::position2d<s32>& getPosition() _IRR_OVERRIDE_
			{
				updateCursorPos();
				return CursorPos;
			}

			//! Returns the current position of the mouse cursor.
			virtual core::position2d<f32> getRelativePosition() _IRR_OVERRIDE_
			{
				updateCursorPos();
				return core::position2d<f32>(CursorPos.X / (f32)Device->Width,
					CursorPos.Y / (f32)Device->Height);
			}

			virtual void setReferenceRect(core::rect<s32>* rect = 0) _IRR_OVERRIDE_
			{
			}

			virtual void setActiveIcon(gui::ECURSOR_ICON iconId) _IRR_OVERRIDE_;

			//! Gets the currently active icon
			virtual gui::ECURSOR_ICON getActiveIcon() const _IRR_OVERRIDE_
			{
				return CurrentIcon;
			}

		private:

			void updateCursorPos()
			{
				CursorPos.X = Device->MouseX;
				CursorPos.Y = Device->MouseY;

				if (CursorPos.X < 0)
					CursorPos.X = 0;
				if (CursorPos.X > (s32)Device->Width)
					CursorPos.X = Device->Width;
				if (CursorPos.Y < 0)
					CursorPos.Y = 0;
				if (CursorPos.Y > (s32)Device->Height)
					CursorPos.Y = Device->Height;
			}

			CIrrDeviceSDL2* Device;
			core::position2d<s32> CursorPos;
			bool IsVisible;

			gui::ECURSOR_ICON CurrentIcon;

			SDL_Cursor* Cursor[gui::ECI_COUNT];
		};

	private:

		//! create the driver
		void createDriver();

		bool createWindow();

		void createKeyMap();

		void logAttributes();

		void resizeWindow(u32 x, u32 y);

		SDL_Surface* ScreenSurface;
		SDL_Window* ScreenWindow;
		SDL_Texture* ScreenTexture;
		SDL_Renderer* ScreenRenderer;

		SDL_GLContext Context;
		SDL_Window* Window;

		bool Fullscreen;

#if defined(_IRR_COMPILE_WITH_JOYSTICK_EVENTS_)
		core::array<SDL_Joystick*> Joysticks;
#endif

		s32 MouseX, MouseY;
		s32 MouseXRel, MouseYRel;
		u32 MouseButtonStates;

		u32 Width, Height;

		bool Resizable;
		bool WindowHasFocus;
		bool WindowMinimized;

		struct SKeyMap
		{
			SKeyMap() {}
			SKeyMap(s32 x11, s32 win32)
				: SDLKey(x11), Win32Key(win32)
			{
			}

			s32 SDLKey;
			s32 Win32Key;

			bool operator<(const SKeyMap& o) const
			{
				return SDLKey < o.SDLKey;
			}
		};

		core::array<SKeyMap> KeyMap;
	};

	void SDLCopyToClipboard(const c8* text);

	const c8* SDLGetTextFromClipboard();

} // end namespace irr

#endif // _IRR_COMPILE_WITH_SDL2_DEVICE_
#endif // __C_IRR_DEVICE_SDL2_H_INCLUDED__

