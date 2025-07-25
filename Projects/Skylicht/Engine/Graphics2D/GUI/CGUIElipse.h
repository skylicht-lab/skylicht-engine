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
	/// @brief This is the object class for displaying an ellipse.
	/// @ingroup GUI
	/// 
	/// @code
	/// CCanvas *canvas = gameobject->addComponent<CCanvas>();
	/// core::rectf r(0.0f, 0.0f, 100.0f, 100.0f);
	/// CGUIElipse* gui = canvas->createElipse(r, SColor(255,255,0,0));
	/// @endcode
	class SKYLICHT_API CGUIElipse : public CGUIElement
	{
		friend class CCanvas;
	public:

	protected:
		float m_a;
		float m_b;

		CGUIElipse(CCanvas* canvas, CGUIElement* parent);
		CGUIElipse(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect);

	public:
		virtual ~CGUIElipse();

		virtual void render(CCamera* camera);

		inline void setFillAngle(float a, float b)
		{
			m_a = a;
			m_b = b;
		}

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		DECLARE_GETTYPENAME(CGUIElipse)
	};
}