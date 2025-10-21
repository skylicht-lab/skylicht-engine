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
		* @brief A simple progress bar control.
		*
		* @ingroup UI
		* 
		* `CUIProgressBar` is a UI control that visually represents progress or loading
		* state. It composes three GUI elements:
		* - a background element (`m_background`),
		* - a mask (`m_mask`) used to clip the loading/foreground element, and
		* - a loading/foreground element (`m_loading`) that is revealed according to
		*   the current progress percentage.
		*
		* Typical usage:
		* - Create the control with a root `CGUIElement` and optionally provide
		*   custom background and loading elements.
		* - Call `setPercent()` to update the displayed progress. The value is
		*   interpreted as a fraction in the range [0.0, 1.0] (0% – 100%).
		*
		* @see CUIBase, CGUIElement, CGUIMask
		*/
		class CUIProgressBar : public CUIBase
		{
		protected:
			CGUIElement* m_background;
			CGUIMask* m_mask;
			CGUIElement* m_loading;

		public:
			/**
			* @brief Construct a progress bar using the given root element.
			* @param container Parent UI container that owns this control.
			* @param element Root `CGUIElement` used for this progress bar.
			*
			* The constructor will typically look for child elements within `element`
			* to initialize `m_background` and `m_loading`. The mask (`m_mask`) is
			* auto-created in the constructor if one is not already present on the
			* provided element.
			*/
			CUIProgressBar(CUIContainer* container, CGUIElement* element);

			/**
			* @brief Construct a progress bar with explicitly provided components.
			* @param container Parent UI container that owns this control.
			* @param element Root `CGUIElement` used for this progress bar.
			* @param bg Background element to use for the bar.
			* @param loading Foreground/loading element to be clipped or resized.
			*
			* Use this constructor when you want to supply specific sub-elements
			* instead of relying on the root element's internal children.
			* The mask (`m_mask`) will still be created automatically if required.
			*/
			CUIProgressBar(CUIContainer* container, CGUIElement* element, CGUIElement* bg, CGUIElement* loading);

			virtual ~CUIProgressBar();

			/**
			* @brief Set the current progress value.
			* @param f Progress value interpreted as a fraction, where 0.0 = empty and 1.0 = full.
			*
			* This function updates the mask rectangle to reveal the corresponding
			* portion of the loading element. The implementation sets the mask width
			* to `element_width * f`. The mask is created in the constructor, so
			* calling this method requires a valid `m_mask`.
			*/
			void setPercent(float f);
		};
	}
}