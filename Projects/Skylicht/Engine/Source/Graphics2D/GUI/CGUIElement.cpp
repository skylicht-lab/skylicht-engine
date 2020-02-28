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
#include "CGUIElement.h"

#include "Graphics2D/CCanvas.h"
#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	CGUIElement::CGUIElement(CCanvas *canvas, const core::rectf& rect) :
		m_canvas(canvas),
		m_level(0),
		m_vertical(Top),
		m_horizontal(Left),
		m_transformChanged(true),
		m_rect(rect),
		m_visible(true),
		m_parent(NULL),
		m_scale(1.0f, 1.0f, 1.0f),
		m_cullingVisible(true),
		m_color(255, 255, 255, 255)
	{
		m_shaderID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");
	}

	CGUIElement::CGUIElement(CCanvas *canvas, CGUIElement *parent) :
		m_canvas(canvas),
		m_vertical(Top),
		m_horizontal(Left),
		m_transformChanged(true),
		m_visible(true),
		m_parent(parent),
		m_scale(1.0f, 1.0f, 1.0f),
		m_cullingVisible(true),
		m_color(255, 255, 255, 255)
	{
		m_level = parent->getLevel() + 1;
		m_rect = parent->getRect();
		m_shaderID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");
	}

	CGUIElement::CGUIElement(CCanvas *canvas, CGUIElement *parent, const core::rectf& rect) :
		m_canvas(canvas),
		m_rect(rect),
		m_vertical(Top),
		m_horizontal(Left),
		m_transformChanged(true),
		m_visible(true),
		m_parent(parent),
		m_scale(1.0f, 1.0f, 1.0f),
		m_cullingVisible(true),
		m_color(255, 255, 255, 255)
	{
		m_level = parent->getLevel() + 1;
		m_shaderID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");
	}

	CGUIElement::~CGUIElement()
	{

	}

	void CGUIElement::remove()
	{
		m_canvas->remove(this);
	}

	void CGUIElement::render(CCamera *camera)
	{

	}

	const core::matrix4& CGUIElement::getRelativeTransform()
	{
		if (m_transformChanged == true)
		{
			m_relativeTransform.makeIdentity();
			m_relativeTransform.setRotationDegrees(m_rotation);

			f32 *m = m_relativeTransform.pointer();

			m[0] *= m_scale.X;
			m[1] *= m_scale.X;
			m[2] *= m_scale.X;
			m[3] *= m_scale.X;

			m[4] *= m_scale.Y;
			m[5] *= m_scale.Y;
			m[6] *= m_scale.Y;
			m[7] *= m_scale.Y;

			m[8] *= m_scale.Z;
			m[9] *= m_scale.Z;
			m[10] *= m_scale.Z;
			m[11] *= m_scale.Z;

			m[12] = m_position.X;
			m[13] = m_position.Y;
			m[14] = m_position.Z;

			m_transformChanged = false;
		}

		return m_relativeTransform;
	}

	void CGUIElement::calcAbsoluteTransform()
	{
		if (m_parent == NULL)
			m_absoluteTransform = getRelativeTransform();
		else
			m_absoluteTransform.setbyproduct_nocheck(m_parent->getAbsoluteTransform(), getRelativeTransform());
	}
}