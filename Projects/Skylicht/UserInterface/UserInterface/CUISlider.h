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
		* @class CUISlider
		* @brief A horizontal slider control with background, fill bar and draggable handle.
		*
		* @ingroup UI
		* 
		* `CUISlider` is a UI wrapper around GUI elements that implements a simple
		* one-dimensional slider. The class tracks a normalized value in range [0, 1]
		* and updates the fill and handle positions accordingly. It receives pointer
		* events from the containing `CUIContainer` via `CUIBase` overrides and handles
		* dragging logic internally.
		*
		* Typical usage:
		* - Create slider with underlying `CGUIElement` (and optional child elements
		*   for background, fillbar and handle).
		* - Subscribe to `OnChanged` to receive value updates.
		* - Call `setValue()` to programmatically update slider value.
		*
		* Responsibilities:
		* - Maintain the current slider value (`getValue()` / `setValue()`).
		* - Move `m_fillBar` and `m_handle` to represent the value visually.
		* - Provide simple drag lifecycle hooks: `onBeginDrag()`, `updateDrag()`,
		*   `onEndDrag()`.
		*
		* Threading: UI code runs on the main/game thread; callbacks are invoked
		* synchronously from the slider when value changes.
		*/
		class CUISlider : public CUIBase
		{
		protected:
			CGUIElement* m_background;
			CGUIElement* m_fillBar;
			CGUIElement* m_handle;
			CGUIMask* m_mask;

			CUIBase* m_bgHander;
			CUIBase* m_buttonHander;

			float m_value;
			float m_handleWidth;

			core::vector2df m_offset;

		public:
			/**
			* @brief Event invoked when slider value changes.
			*
			* Signature: `void(CUISlider* sender, float newValue)`
			* - `sender` is the slider instance that produced the event.
			* - `newValue` is the new normalized value in [0, 1].
			*
			* The event is invoked by `setValue()` and during dragging (when the
			* underlying value changes). Listeners should not assume reentrancy
			* protection; if they mutate the slider, behaviour is the caller's
			* responsibility.
			*/
			std::function<void(CUISlider*, float)> OnChanged;

		public:
			/**
			* @brief Construct a slider that wraps an existing `CGUIElement`.
			* @param container Parent `CUIContainer` that owns this UI object.
			* @param element Underlying `CGUIElement` used as the root element for the slider.
			*
			* The constructor will attempt to locate child GUI elements (fill, handle)
			* inside `element` or rely on the explicit constructor overload which
			* accepts those child elements directly.
			*/
			CUISlider(CUIContainer* container, CGUIElement* element);

			/**
			* @brief Construct a slider with explicit sub-elements.
			* @param container Parent `CUIContainer`.
			* @param element Root GUI element.
			* @param bg Background/track element.
			* @param fillbar Fill portion element.
			* @param handle Draggable handle element.
			*
			* Use this overload when you have separate GUI elements for the
			* background, fill and handle to ensure the slider uses them directly.
			*/
			CUISlider(CUIContainer* container, CGUIElement* element, CGUIElement* bg, CGUIElement* fillbar, CGUIElement* handle);

			virtual ~CUISlider();

			/**
			* @brief Set the slider value and optionally invoke the `OnChanged` event.
			* @param value New value to set. Values are clamped to [0, 1].
			* @param invokeEvent When true (default) the `OnChanged` callback will be executed if value changed.
			*
			* This method updates the internal `m_value`, moves the fill and handle
			* GUI elements to reflect the change and triggers the change callback.
			*/
			void setValue(float value, bool invokeEvent = true);

			/** @brief Get the current normalized slider value in range [0,1]. */
			inline float getValue()
			{
				return m_value;
			}

		protected:

			/**
			* @brief Initialize internal state and locate child elements.
			*
			* Called by constructors to perform common setup:
			* - cache handle width,
			* - configure mask if available,
			* - wire pointer callbacks on `m_handle`/`m_buttonHander` if present.
			*/
			void init();

			/**
			* @brief Called when a drag begins (pointer down on handle).
			*
			* This captures the initial pointer offset and prepares slider for
			* `updateDrag()` calls while the pointer moves.
			*/
			virtual void onBeginDrag();

			/**
			* @brief Update the slider value according to current pointer position.
			*
			* This method should compute the new normalized value based on pointer
			* coordinates relative to the slider track, update visuals and optionally
			* fire `OnChanged`.
			*/
			virtual void updateDrag();

			/**
			* @brief Called when a drag ends (pointer up).
			*
			* Finalizes the value and releases any capture state.
			*/
			virtual void onEndDrag();
		};
	}
}