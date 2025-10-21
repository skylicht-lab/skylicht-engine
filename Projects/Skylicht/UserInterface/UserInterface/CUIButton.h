/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
		 * @brief Button control built on top of `CUIBase`.
		 *
		 * @ingroup UI
		 * 
		 * `CUIButton` provides a simple button implementation with optional toggle behavior,
		 * a background element and a `CGUIText` label. It handles pointer hover/out and
		 * pressed events and exposes callbacks for toggle changes.
		 *
		 * Typical usage:
		 * - Construct with a parent `CUIContainer` and a `CGUIElement` representing the element node.
		 * - Optionally provide a background element and a text element to control visual parts separately.
		 * - Register to `OnToggle` or override `onPressed()` to react to user interaction.
		 */
		class CUIButton : public CUIBase
		{
		protected:
			CGUIElement* m_background;
			CGUIText* m_text;

			bool m_isToggle;
			bool m_toggleStatus;

		public:

			/**
			 * @brief Callback invoked when the button toggle state changes.
			 * @param sender Pointer to the button (`CUIBase*` / `CUIButton*` castable).
			 * @param toggled New toggle state (true if toggled on).
			 */
			std::function<void(CUIBase*, bool)> OnToggle;

		public:
			/**
			 * @brief Construct a button bound to a container and element.
			 * @param container Parent `CUIContainer` that owns this control.
			 * @param element Underlying `CGUIElement` representing this control node.
			 *
			 * The background and text elements remain null; use setters or the alternate constructor
			 * to provide them.
			 */
			CUIButton(CUIContainer* container, CGUIElement* element);

			/**
			 * @brief Construct a button with explicit background and text elements.
			 * @param container Parent `CUIContainer`.
			 * @param element Root element of the button.
			 * @param bg Background `CGUIElement` used for visual state changes.
			 * @param text `CGUIText` used as the button label.
			 */
			CUIButton(CUIContainer* container, CGUIElement* element, CGUIElement* bg, CGUIText* text);

			virtual ~CUIButton();

			/**
			 * @brief Set the button label using a UTF-8 `char` string.
			 * @param string Null-terminated UTF-8 string to set as label.
			 */
			void setLabel(const char* string);

			/**
			 * @brief Set the button label using a wide character string.
			 * @param string Null-terminated wide string to set as label.
			 */
			void setLabel(const wchar_t* string);

			/**
			 * @brief Set the label and adjust the button width by `addition` units.
			 * @param string UTF-8 label text.
			 * @param addition Extra width to add after sizing to the text.
			 *
			 * Useful when label size must drive layout while keeping padding.
			 */
			void setLabelAndChangeWidth(const char* string, float addition);

			/**
			 * @brief Set the label (wide chars) and adjust the button width by `addition` units.
			 * @param string Wide label text.
			 * @param addition Extra width to add after sizing to the text.
			 */
			void setLabelAndChangeWidth(const wchar_t* string, float addition);

			virtual void onPointerHover(float pointerX, float pointerY);

			virtual void onPointerOut(float pointerX, float pointerY);

			virtual void onPressed();

			/**
			 * @brief Enable or disable toggle mode for this button.
			 * @param b true to make the button a toggle (stateful), false for momentary button.
			 */
			inline void setToggleButton(bool b)
			{
				m_isToggle = b;
			}

			/**
			 * @brief Query whether the button is configured as a toggle.
			 * @return true if the button is a toggle button.
			 */
			inline bool isToggleButton()
			{
				return m_isToggle;
			}

			/**
			 * @brief Set the toggle state programmatically.
			 * @param b New toggle state (true = on).
			 * @param invokeEvent If true, `OnToggle` will be invoked; otherwise it will not.
			 */
			virtual void setToggle(bool b, bool invokeEvent = true);

			/** @brief Returns current toggle state. */
			inline bool isToggle()
			{
				return m_toggleStatus;
			}

			/** @brief Get the background element (may be nullptr). */
			inline CGUIElement* getBackground()
			{
				return m_background;
			}

			/** @brief Get the text element used as label (may be nullptr). */
			inline CGUIText* getText()
			{
				return m_text;
			}
		};
	}
}