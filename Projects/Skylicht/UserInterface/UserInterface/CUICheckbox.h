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
		 * @brief Checkbox control built on top of `CUIBase`.
		 *
		 * @ingroup UI
		 * 
		 * `CUICheckbox` represents a two-state checkbox control with an optional simple animation
		 * when the checked state changes. The control contains a background element and a checked
		 * mark element (`m_checked`). Use `OnChanged` to react to user-initiated or programmatic
		 * changes.
		 *
		 * Behaviour:
		 * - `onPressed()` toggles the checked state (unless overridden).
		 * - `setToggle()` changes the state programmatically; `invokeEvent` controls whether
		 *   the `OnChanged` callback is invoked; `doAnimation` controls whether the checkbox plays
		 *   its internal tween animation.
		 *
		 * Typical usage:
		 * - Construct with a parent `CUIContainer` and an element node.
		 * - Optionally provide explicit background and checked elements via the alternate constructor.
		 * - Register a listener on `OnChanged` to be notified when the state changes.
		 */
		class CUICheckbox : public CUIBase
		{
		protected:
			CGUIElement* m_background;
			CGUIElement* m_checked;

			bool m_toggleStatus;

			CTween* m_tween;

		public:

			/**
			 * @brief Callback invoked when the checkbox state changes.
			 *
			 * Signature: `void(CUICheckbox* sender, bool checked)`
			 * - `sender`: pointer to this checkbox.
			 * - `checked`: new checked state (true = checked).
			 */
			std::function<void(CUICheckbox*, bool)> OnChanged;

		public:
			/**
			 * @brief Create a checkbox bound to a container and element.
			 * @param container Parent `CUIContainer` that owns this control.
			 * @param element Root `CGUIElement` node representing this control.
			 *
			 * The control will use internal children of `element` (if present) or explicit
			 * elements can be supplied with the alternate constructor.
			 */
			CUICheckbox(CUIContainer* container, CGUIElement* element);

			/**
			 * @brief Create a checkbox with explicit background and checked elements.
			 * @param container Parent `CUIContainer`.
			 * @param element Root element for the control.
			 * @param bg Background element used for visuals.
			 * @param checked Element shown when checkbox is in checked state.
			 */
			CUICheckbox(CUIContainer* container, CGUIElement* element, CGUIElement* bg, CGUIElement* checked);

			virtual ~CUICheckbox();

			/**
			 * @brief Called when the control is pressed.
			 *
			 * Default implementation toggles the checkbox state and will invoke `OnChanged`.
			 * Subclasses may override to customize behaviour; if overriding and wanting the
			 * default toggle semantics, they should call the base implementation.
			 */
			virtual void onPressed();

			/**
			 * @brief Set the checked state programmatically.
			 * @param b Desired checked state (true = checked).
			 * @param invokeEvent If true, the `OnChanged` callback is invoked.
			 * @param doAnimation If true, play checkbox animation (if present) when changing.
			 */
			void setToggle(bool b, bool invokeEvent = true, bool doAnimation = false);

			/**
			 * @brief Query current checked state.
			 * @return true if checkbox is checked.
			 */
			inline bool isToggle()
			{
				return m_toggleStatus;
			}

			/**
			 * @brief Get the background element.
			 * @return Pointer to `CGUIElement` used as background, or nullptr.
			 */
			inline CGUIElement* getBackground()
			{
				return m_background;
			}

			/**
			 * @brief Get the checked element.
			 * @return Pointer to `CGUIElement` representing the checked mark, or nullptr.
			 */
			inline CGUIElement* getChecked()
			{
				return m_checked;
			}

		protected:

			/**
			 * @brief Stop and clear any active tween animation.
			 *
			 * Called internally to abort animations when the control is destroyed or when
			 * a new animation should replace the current one.
			 */
			void stopTween();

			/**
			 * @brief Start the tween animation for the checked/unchecked transition.
			 *
			 * If no `m_tween` exists, no animation is played. This method is internal.
			 */
			void playTween();
		};
	}
}