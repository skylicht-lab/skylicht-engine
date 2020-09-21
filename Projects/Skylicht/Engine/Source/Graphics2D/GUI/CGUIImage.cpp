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

#include "pch.h"
#include "CGUIImage.h"
#include "Graphics2D/CGraphics2D.h"

namespace Skylicht
{
	CGUIImage::CGUIImage(CCanvas *canvas, const core::rectf& rect) :
		CGUIElement(canvas, rect),
		m_image(NULL)
	{

	}

	CGUIImage::CGUIImage(CCanvas *canvas, CGUIElement *parent) :
		CGUIElement(canvas, parent),
		m_image(NULL)
	{

	}

	CGUIImage::CGUIImage(CCanvas *canvas, CGUIElement *parent, const core::rectf& rect) :
		CGUIElement(canvas, parent, rect),
		m_image(NULL)
	{

	}

	CGUIImage::~CGUIImage()
	{

	}

	void CGUIImage::render(CCamera *camera)
	{
		if (m_image != NULL)
		{
			CGraphics2D::getInstance()->addImageBatch(m_image, m_rect, m_sourceRect, m_color, m_absoluteTransform, m_shaderID, m_material, m_pivot.X, m_pivot.Y);
		}
	}

	void CGUIImage::setImage(ITexture *texture)
	{
		m_image = texture;

		if (m_image)
			setSourceRect(0, 0, (float)m_image->getSize().Width, (float)m_image->getSize().Height);
	}
}