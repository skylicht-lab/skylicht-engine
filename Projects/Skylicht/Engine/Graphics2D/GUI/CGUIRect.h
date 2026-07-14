/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "CGUIElement.h"

namespace Skylicht
{
	/**
	 * @brief This is the object class for displaying a rectangle.
	 * @ingroup GUI
	 *
	 * @code
	 * CCanvas *canvas = gameobject->addComponent<CCanvas>();
	 * core::rectf r(0.0f, 0.0f, 100.0f, 100.0f);
	 * CGUIRect* gui = canvas->createRect(r, SColor(255,255,0,0));
	 * @endcode
	 */
	class SKYLICHT_API CGUIRect : public CGUIElement
	{
		friend class CCanvas;
	public:

	protected:
		/**
		 * @brief Construct a rectangle element for a canvas and optional parent.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 */
		CGUIRect(CCanvas* canvas, CGUIElement* parent);

		/**
		 * @brief Construct a rectangle element with an initial rectangle.
		 * @param canvas Canvas that owns the element.
		 * @param parent Parent element, or NULL for a root element.
		 * @param rect Initial local GUI rectangle.
		 */
		CGUIRect(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect);

	public:
		/**
		 * @brief Destructor.
		 */
		virtual ~CGUIRect();

		/**
		 * @brief Render the rectangle element.
		 * @param camera Camera used for GUI rendering.
		 */
		virtual void render(CCamera* camera);

		DECLARE_GETTYPENAME(CGUIRect)
	};
}
