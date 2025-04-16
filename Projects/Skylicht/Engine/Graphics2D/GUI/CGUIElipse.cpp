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

#include "pch.h"
#include "CGUIElipse.h"
#include "Material/Shader/CShaderManager.h"
#include "Graphics2D/CGraphics2D.h"

namespace Skylicht
{
	CGUIElipse::CGUIElipse(CCanvas* canvas, CGUIElement* parent) :
		CGUIElement(canvas, parent),
		m_a(0.0f),
		m_b(360.0f)
	{
		m_enableMaterial = true;
		m_renderData->ShaderID = CShaderManager::getInstance()->getShaderIDByName("VertexColorAlpha");
	}

	CGUIElipse::CGUIElipse(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect) :
		CGUIElement(canvas, parent, rect),
		m_a(0.0f),
		m_b(360.0f)
	{
		m_enableMaterial = true;
		m_renderData->ShaderID = CShaderManager::getInstance()->getShaderIDByName("VertexColorAlpha");
	}

	CGUIElipse::~CGUIElipse()
	{

	}

	void CGUIElipse::render(CCamera* camera)
	{
		const SColor& color = getColor();
		if (color.getAlpha() > 0)
		{
			core::rectf uv(0.0f, 0.0f, 1.0f, 1.0f);
			CGraphics2D::getInstance()->addEclipseBatch(getRect(), uv, color, m_transform->World, getShaderID(), m_a, m_b, getMaterial());
		}
		CGUIElement::render(camera);
	}

	CObjectSerializable* CGUIElipse::createSerializable()
	{
		CObjectSerializable* object = CGUIElement::createSerializable();
		object->autoRelease(new CFloatProperty(object, "angleA", m_a, -360.0f, 360.0f));
		object->autoRelease(new CFloatProperty(object, "angleB", m_b, -360.0f, 360.0f));
		return object;
	}

	void CGUIElipse::loadSerializable(CObjectSerializable* object)
	{
		CGUIElement::loadSerializable(object);
		m_a = object->get<float>("angleA", 0.0f);
		m_b = object->get<float>("angleB", 360.0f);
	}
}