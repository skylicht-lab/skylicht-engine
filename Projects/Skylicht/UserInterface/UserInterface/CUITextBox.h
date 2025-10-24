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
		* @brief Single-line / multi-line editable text box control.
		*
		* @ingroup UI
		*
		* `CUITextBox` wraps a `CGUIText` element and provides basic text editing
		* behaviour: caret management, text insertion/deletion, keyboard navigation,
		* and mouse caret placement. The control forwards focus and capture to the
		* global `CUIEventManager` when the user interacts with the text area.
		*
		* Important behaviours implemented by this class:
		* - Shows the I-beam cursor while hovering and resets it on pointer out.
		* - On pointer down: places the caret at the clicked character, shows the caret,
		*   sets focus and capture to this control.
		* - While pointer is down and moved: caret follows the pointer.
		* - On lost focus: hides the caret.
		* - Keyboard handling (in `onKeyEvent`):
		*   - Arrow keys, Home/End, Page Up/Down (skeleton) move the caret.
		*   - Backspace/Delete perform editing when `m_editable` is true and invoke `OnTextChanged`.
		*   - Character input is inserted when current length < `m_maxLength`.
		*
		* By default, the control is editable and the maximum text length is 64.
		*
		* Note: the internal `CGUIText` has `setEnableTextFormnat(false)` applied
		* by the constructors to disable automatic text formatting behaviour.
		*/
		class CUITextBox : public CUIBase
		{
		protected:
			CGUIElement* m_background;
			CGUIText* m_text;

			bool m_editable;

			int m_maxLength;

		public:

			/**
			* @brief Callback fired when the text content changes.
			* @param sender Pointer to the `CUIBase` (this control).
			*
			* Invoked after editing operations that modify the text.
			*/
			std::function<void(CUIBase*)> OnTextChanged;

		public:
			/**
			* @brief Create a `CUITextBox` by locating child elements named "Background" and "Text".
			* @param container Parent UI container that owns this control.
			* @param element  Root `CGUIElement` for the control.
			*
			* The constructor sets `m_editable = true`, `m_maxLength = 64`, and
			* disables text formatting on the underlying `CGUIText`.
			*/
			CUITextBox(CUIContainer* container, CGUIElement* element);

			/**
			* @brief Create a `CUITextBox` with explicitly supplied background and text elements.
			* @param container  Parent UI container that owns this control.
			* @param element    Root `CGUIElement` for the control.
			* @param background Background element to use.
			* @param text       `CGUIText` element used for rendering and editing.
			*/
			CUITextBox(CUIContainer* container, CGUIElement* element, CGUIElement* background, CGUIText* text);

			virtual ~CUITextBox();

			/** @brief Get the background GUI element (may be null). */
			inline CGUIElement* getBackground()
			{
				return m_background;
			}

			/** @brief Get the underlying `CGUIText` used by this text box. */
			inline CGUIText* getTextGUI()
			{
				return m_text;
			}

			/**
			* @brief Set the text content (UTF-8 / narrow string).
			* @param text C string to set as content.
			*/
			void setText(const char* text);

			/**
			* @brief Set the text content (wide string).
			* @param text Wide string to set as content.
			*/
			void setText(const wchar_t* text);

			/** @brief Get the current text (UTF-8 / narrow C string). Returns empty string if no `m_text`. */
			const char* getText();

			/** @brief Get the current text (wide string). Returns empty wide string if no `m_text`. */
			const wchar_t* getTextW();

			/** @brief Get the current text length in characters. */
			int getTextLength();

			/** @brief Enable or disable user editing. */
			inline void setEditable(bool b)
			{
				m_editable = b;
			}

			/** @brief Returns true when the control is editable. */
			inline bool isEditable()
			{
				return m_editable;
			}

			/** @brief Set maximum allowed characters that can be inserted. */
			inline void setMaxLength(int l)
			{
				m_maxLength = l;
			}

			/** @brief Get maximum allowed characters. */
			inline int getMaxLength()
			{
				return m_maxLength;
			}

			virtual void onPointerHover(int pointerId, float pointerX, float pointerY);

			virtual void onPointerOut(int pointerId, float pointerX, float pointerY);

			virtual void onPointerDown(int pointerId, float pointerX, float pointerY);

			virtual void onPointerUp(int pointerId, float pointerX, float pointerY);

			virtual void onPointerMove(int pointerId, float pointerX, float pointerY);

			virtual void onLostFocus();

			/**
			* @brief Keyboard event handler for navigation and editing.
			*
			* Behaviour summary:
			* - Arrow keys / Home / End move the caret across lines and characters.
			* - Backspace/Delete modify text when `m_editable` is true and invoke `OnTextChanged`.
			* - Printable characters are inserted when current length < `m_maxLength`.
			*
			* @param event Original input `SEvent` from the engine.
			*/
			virtual void onKeyEvent(const SEvent& event);
		};
	}
}