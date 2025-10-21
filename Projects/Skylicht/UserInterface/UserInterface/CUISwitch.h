/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

#include "CUIBase.h"

namespace Skylicht
{
	namespace UI
	{
		/**
		* @brief Toggle switch control.
		*
		* @ingroup UI
		* 
		* `CUISwitch` is a UI control that represents a two-state toggle (on/off).
		* It composes three GUI elements:
		*  - `m_background` : optional background track
		*  - `m_handle`     : the movable handle that represents current state
		*  - `m_on`         : an alternate handle (or visual) used for the "on" state
		*
		* Visual transitions between states can be animated using an internal
		* tween (`m_tween`). Colors for the handle in each state are configurable
		* via `setHandleColor()`.
		*
		* The control exposes the `OnChanged` callback which is invoked whenever
		* the toggle value changes (either from `setToggle()` when `invokeEvent`
		* is true, or from user interaction via `onPressed()`).
		*
		* @see CUIBase
		*/
		class CUISwitch : public CUIBase
		{
		protected:
			CGUIElement* m_background;
			CGUIElement* m_on;
			CGUIElement* m_handle;

			bool m_toggleStatus;

			core::vector3df m_onPosition;
			core::vector3df m_offPosition;

			CTween* m_tween;

			SColor m_onColor;
			SColor m_offColor;

		public:

			/**
			* @brief Callback invoked when the toggle state changes.
			* @param sender Pointer to the sender `CUISwitch`.
			* @param state  New toggle state: true = on, false = off.
			*
			* This callback is invoked from `setToggle()` when `invokeEvent` is true
			* and from `onPressed()` after the user toggles the switch.
			*/
			std::function<void(CUISwitch*, bool)> OnChanged;

		public:
			/**
			* @brief Construct a switch from a root `CGUIElement`.
			* @param container Parent UI container that owns this control.
			* @param element  Root `CGUIElement` which may contain children:
			*                 "Background", "HandleOff", "HandleOn".
			*
			* The constructor finds sub-elements by path and initializes
			* `m_offPosition` / `m_onPosition`. `m_on` is hidden by default.
			*/
			CUISwitch(CUIContainer* container, CGUIElement* element);

			/**
			* @brief Construct a switch using explicit sub-elements.
			* @param container Parent UI container that owns this control.
			* @param element  Root `CGUIElement` for the control.
			* @param bg       Background element to use for the track.
			* @param handle   Handle element representing the movable knob.
			* @param on       Optional alternate handle/visual for "on" state.
			*/
			CUISwitch(CUIContainer* container, CGUIElement* element, CGUIElement* bg, CGUIElement* handle, CGUIElement* on);

			virtual ~CUISwitch();

			virtual void onPressed();

			/**
			* @brief Set the toggle state programmatically.
			* @param b            New state: true = on, false = off.
			* @param invokeEvent  If true, `OnChanged` will be invoked after state change.
			* @param doAnimation  If true, state change will be animated using tweening;
			*                     otherwise the handle position and color are set immediately.
			*
			* The function updates `m_toggleStatus`, moves the handle to the correct
			* position and applies the corresponding handle color. If animation is
			* requested, the internal tween will animate the handle between positions.
			*/
			void setToggle(bool b, bool invokeEvent = true, bool doAnimation = false);

			/** @brief Returns current toggle state. */
			inline bool isToggle()
			{
				return m_toggleStatus;
			}

			/**
			* @brief Set colors used for the handle in on/off states.
			* @param on  Color to use when the switch is on.
			* @param off Color to use when the switch is off.
			*/
			void setHandleColor(const SColor& on, const SColor& off)
			{
				m_onColor = on;
				m_offColor = off;
			}

			/** @brief Get the background GUI element. */
			inline CGUIElement* getBackground()
			{
				return m_background;
			}

			/** @brief Get the handle GUI element. */
			inline CGUIElement* getHandle()
			{
				return m_handle;
			}

		protected:
			/** Stop and release the current tween if any. */
			void stopTween();

			/** Create and start a tween to animate handle between positions. */
			void playTween();
		};
	}
}