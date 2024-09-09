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
#include "CGUILayout.h"
#include "Material/Shader/CShaderManager.h"
#include "Graphics2D/CGraphics2D.h"


namespace Skylicht
{
	CGUILayout::CGUILayout(CCanvas* canvas, CGUIElement* parent) :
		CGUIElement(canvas, parent)
	{
		m_layoutData = m_entity->addData<CGUILayoutData>();
	}

	CGUILayout::CGUILayout(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect) :
		CGUIElement(canvas, parent, rect)
	{
		m_layoutData = m_entity->addData<CGUILayoutData>();
	}

	CGUILayout::~CGUILayout()
	{

	}

	void CGUILayout::render(CCamera* camera)
	{
		CGUIElement::render(camera);
	}

	CObjectSerializable* CGUILayout::createSerializable()
	{
		CObjectSerializable* object = CGUIElement::createSerializable();

		CEnumProperty<CGUILayoutData::EAlignType>* alginType = new CEnumProperty<CGUILayoutData::EAlignType>(object, "layoutAlignType", m_layoutData->AlignType);
		alginType->addEnumString("Vertical", CGUILayoutData::Vertical);
		alginType->addEnumString("Horizontal", CGUILayoutData::Horizontal);
		object->autoRelease(alginType);

		object->autoRelease(new CFloatProperty(object, "spacing", m_layoutData->Spacing));
		object->autoRelease(new CBoolProperty(object, "fitChildrenSize", m_layoutData->FitChildrenSize));

		return object;
	}

	void CGUILayout::loadSerializable(CObjectSerializable* object)
	{
		CGUIElement::loadSerializable(object);

		m_layoutData->AlignType = object->get<CGUILayoutData::EAlignType>("layoutAlignType", CGUILayoutData::Vertical);
		m_layoutData->Spacing = object->get<float>("spacing", 0);
		m_layoutData->FitChildrenSize = object->get<bool>("fitChildrenSize", 0);
	}
}