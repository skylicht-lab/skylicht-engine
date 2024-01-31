// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_OSX_H_INCLUDED__
#define __C_IRR_DEVICE_OSX_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OSX_DEVICE_

#include "CIrrDeviceStub.h"
#include "IrrlichtDevice.h"
#include "ICursorControl.h"

namespace irr
{
	class CIrrDeviceOSX : public CIrrDeviceStub
	{
	public:
		CIrrDeviceOSX(const SIrrlichtCreationParameters& param);

		virtual ~CIrrDeviceOSX();

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

        void setMouseLocation(int x, int y);
        
        void setCursorVisible(bool visible);
        
        void setCursorIcon(gui::ECURSOR_ICON iconId);
        
	private:

		void createDriver();

		video::SExposedVideoData& getExposedVideoData();

		bool Focused;
		bool Initialized;
		bool Paused;

		video::SExposedVideoData ExposedVideoData;
        
        //! Implementation of the macos x cursor control
        class CCursorControl : public gui::ICursorControl
        {
        public:

            CCursorControl(const core::dimension2d<u32>& wsize, CIrrDeviceOSX *device)
                : WindowSize(wsize), InvWindowSize(0.0f, 0.0f), Device(device), IsVisible(true), UseReferenceRect(false)
            {
                CursorPos.X = CursorPos.Y = 0;
                if (WindowSize.Width!=0)
                    InvWindowSize.Width = 1.0f / WindowSize.Width;
                if (WindowSize.Height!=0)
                    InvWindowSize.Height = 1.0f / WindowSize.Height;
            }

            //! Changes the visible state of the mouse cursor.
            virtual void setVisible(bool visible)
            {
                IsVisible = visible;
                Device->setCursorVisible(visible);
            }

            //! Returns if the cursor is currently visible.
            virtual bool isVisible() const
            {
                return IsVisible;
            }

            //! Sets the new position of the cursor.
            virtual void setPosition(const core::position2d<f32> &pos)
            {
                setPosition(pos.X, pos.Y);
            }

            //! Sets the new position of the cursor.
            virtual void setPosition(f32 x, f32 y)
            {
                setPosition((s32)(x*WindowSize.Width), (s32)(y*WindowSize.Height));
            }

            //! Sets the new position of the cursor.
            virtual void setPosition(const core::position2d<s32> &pos)
            {
                if (CursorPos.X != pos.X || CursorPos.Y != pos.Y)
                    setPosition(pos.X, pos.Y);
            }

            //! Sets the new position of the cursor.
            virtual void setPosition(s32 x, s32 y)
            {
                if (UseReferenceRect)
                {
                    Device->setMouseLocation(ReferenceRect.UpperLeftCorner.X + x, ReferenceRect.UpperLeftCorner.Y + y);
                }
                else
                {
                    Device->setMouseLocation(x,y);
                }
            }

            //! Returns the current position of the mouse cursor.
            virtual const core::position2d<s32>& getPosition()
            {
                return CursorPos;
            }

            //! Returns the current position of the mouse cursor.
            virtual core::position2d<f32> getRelativePosition()
            {
                if (!UseReferenceRect)
                {
                    return core::position2d<f32>(CursorPos.X * InvWindowSize.Width,
                        CursorPos.Y * InvWindowSize.Height);
                }

                return core::position2d<f32>(CursorPos.X / (f32)ReferenceRect.getWidth(),
                        CursorPos.Y / (f32)ReferenceRect.getHeight());
            }

            //! Sets an absolute reference rect for calculating the cursor position.
            virtual void setReferenceRect(core::rect<s32>* rect=0)
            {
                if (rect)
                {
                    ReferenceRect = *rect;
                    UseReferenceRect = true;

                    // prevent division through zero and uneven sizes

                    if (!ReferenceRect.getHeight() || ReferenceRect.getHeight()%2)
                        ReferenceRect.LowerRightCorner.Y += 1;

                    if (!ReferenceRect.getWidth() || ReferenceRect.getWidth()%2)
                        ReferenceRect.LowerRightCorner.X += 1;
                }
                else
                    UseReferenceRect = false;
            }

            //! Updates the internal cursor position
            virtual void updateInternalCursorPosition(int x,int y)
            {
                CursorPos.X = x;
                CursorPos.Y = y;
            }

            virtual void setActiveIcon(gui::ECURSOR_ICON iconId)
            {
                Device->setCursorIcon(iconId);
            }
            
        private:

            core::position2d<s32> CursorPos;
            core::dimension2d<s32> WindowSize;
            core::dimension2d<float> InvWindowSize;
            core::rect<s32> ReferenceRect;
            CIrrDeviceOSX *Device;
            bool IsVisible;
            bool UseReferenceRect;
        };
	};

} // end namespace irr

#endif
#endif
