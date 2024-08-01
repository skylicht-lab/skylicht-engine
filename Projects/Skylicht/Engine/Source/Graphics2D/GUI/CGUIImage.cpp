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

#include "TextureManager/CTextureManager.h"

namespace Skylicht
{
	CGUIImage::CGUIImage(CCanvas* canvas, CGUIElement* parent) :
		CGUIElement(canvas, parent),
		m_image(NULL)
	{
		m_enableMaterial = true;
	}

	CGUIImage::CGUIImage(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect) :
		CGUIElement(canvas, parent, rect),
		m_image(NULL)
	{
		m_enableMaterial = true;
	}

	CGUIImage::~CGUIImage()
	{

	}

	void CGUIImage::render(CCamera* camera)
	{
		if (m_image != NULL)
		{
			const SColor& color = getColor();
			if (color.getAlpha() > 0)
			{
				CGraphics2D::getInstance()->addImageBatch(
					m_image,
					getRect(),
					m_sourceRect,
					color,
					m_transform->World,
					getShaderID(),
					getMaterial(),
					m_pivot.X, m_pivot.Y);
			}
		}

		CGUIElement::render(camera);
	}

	const core::rectf CGUIImage::getNativeRect()
	{
		core::rectf r = core::rectf(core::vector2df(0.0f, 0.0f), core::dimension2df(getWidth(), getHeight()));
		if (m_image != NULL)
		{
			core::dimension2du size = m_image->getSize();
			r.LowerRightCorner.X = r.UpperLeftCorner.X + size.Width;
			r.LowerRightCorner.Y = r.UpperLeftCorner.Y + size.Height;
		}
		return r;
	}

	void CGUIImage::setImage(ITexture* texture)
	{
		m_image = texture;

		if (m_image)
			setSourceRect(0, 0, (float)m_image->getSize().Width, (float)m_image->getSize().Height);
	}

	CObjectSerializable* CGUIImage::createSerializable()
	{
		CObjectSerializable* object = CGUIElement::createSerializable();
		object->autoRelease(new CImageSourceProperty(object, "imageSrc", m_resource.c_str()));
		return object;
	}


	void CGUIImage::loadSerializable(CObjectSerializable* object)
	{
		std::string src = object->get("imageSrc", std::string(""));
		CGUIElement::loadSerializable(object);

		if (src != m_resource)
		{
			m_resource = src;
			ITexture* t = CTextureManager::getInstance()->getTexture(m_resource.c_str());
			setImage(t);
		}
	}
}