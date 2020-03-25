/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
	class CGUIImage : public CGUIElement
	{
		friend class CCanvas;

	protected:
		ITexture *m_image;

		core::rectf m_sourceRect;
		core::position2df m_pivot;

	protected:
		CGUIImage(CCanvas *canvas, const core::rectf& rect);
		CGUIImage(CCanvas *canvas, CGUIElement *parent);
		CGUIImage(CCanvas *canvas, CGUIElement *parent, const core::rectf& rect);

	public:
		virtual ~CGUIImage();

		virtual void render(CCamera *camera);

		void setImage(ITexture *texture);

		inline ITexture* getImage()
		{
			return m_image;
		}

		inline void setSourceRect(float x, float y, float w, float h)
		{
			m_sourceRect.UpperLeftCorner.X = x;
			m_sourceRect.UpperLeftCorner.Y = y;
			m_sourceRect.LowerRightCorner.X = x + w;
			m_sourceRect.LowerRightCorner.Y = y + h;
		}

		void setPivot(float x, float y)
		{
			m_pivot.set(x, y);
		}

		const core::position2df& getPivot()
		{
			return m_pivot;
		}
	};
}