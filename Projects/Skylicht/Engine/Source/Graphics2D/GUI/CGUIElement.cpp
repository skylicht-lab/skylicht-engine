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
#include "Graphics2D/CGraphics2D.h"
#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	CGUIElement::CGUIElement(CCanvas* canvas, CGUIElement* parent) :
		m_canvas(canvas),
		m_visible(true),
		m_parent(NULL),
		m_mask(NULL),
		m_applyCurrentMask(NULL)
	{
		CEntityPrefab* entityPrefab = m_canvas->getEntityManager();
		m_entity = entityPrefab->createEntity();
		entityPrefab->addTransformData(m_entity, NULL, core::IdentityMatrix, "");

		m_guiTransform = m_entity->addData<CGUITransformData>();
		m_guiAlign = m_entity->addData<CGUIAlignData>();
		m_renderData = m_entity->addData<CGUIRenderData>();
		m_transform = GET_ENTITY_DATA(m_entity, CWorldTransformData);

		setParent(parent);
		setRect(parent->getRect());
	}

	CGUIElement::CGUIElement(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect) :
		m_canvas(canvas),
		m_visible(true),
		m_parent(NULL),
		m_mask(NULL),
		m_applyCurrentMask(NULL)
	{
		CEntityPrefab* entityPrefab = m_canvas->getEntityManager();
		m_entity = entityPrefab->createEntity();
		entityPrefab->addTransformData(m_entity, NULL, core::IdentityMatrix, "");

		m_guiTransform = m_entity->addData<CGUITransformData>();
		m_guiAlign = m_entity->addData<CGUIAlignData>();
		m_renderData = m_entity->addData<CGUIRenderData>();
		m_transform = GET_ENTITY_DATA(m_entity, CWorldTransformData);

		setParent(parent);
		setRect(rect);
	}

	CGUIElement::~CGUIElement()
	{
		removeAllChilds();

		CEntityPrefab* entityPrefab = m_canvas->getEntityManager();
		entityPrefab->removeEntity(m_entity);
	}

	void CGUIElement::setParent(CGUIElement* parent)
	{
		if (m_parent)
			m_parent->removeChild(this);

		m_parent = parent;

		CEntityPrefab* entityMgr = m_canvas->getEntityManager();

		if (m_parent)
		{
			m_parent->m_childs.push_back(this);

			entityMgr->changeParent(m_entity, m_parent->m_entity);

			CGUITransformData* t = GET_ENTITY_DATA(m_entity, CGUITransformData);
			t->Parent = GET_ENTITY_DATA(m_parent->m_entity, CGUITransformData);
		}
		else
		{
			entityMgr->changeParent(m_entity, NULL);

			CGUITransformData* t = GET_ENTITY_DATA(m_entity, CGUITransformData);
			t->Parent = NULL;
		}
	}

	bool CGUIElement::removeChild(CGUIElement* child)
	{
		for (int i = 0, n = (int)m_childs.size(); i < n; i++)
		{
			if (m_childs[i] == child)
			{
				m_childs.erase(m_childs.begin() + i);
				return true;
			}
		}
		return false;
	}

	void CGUIElement::remove()
	{
		if (m_parent)
			m_parent->removeChild(this);

		delete this;
	}

	void CGUIElement::removeAllChilds()
	{
		for (CGUIElement* child : m_childs)
			delete child;

		m_childs.clear();
	}

	void CGUIElement::update(CCamera* camera)
	{

	}

	void CGUIElement::render(CCamera* camera)
	{

	}

	CObjectSerializable* CGUIElement::createSerializable()
	{
		CObjectSerializable* object = new CObjectSerializable(getTypeName().c_str());
		object->autoRelease(new CBoolProperty(object, "visible", m_visible));
		return object;
	}
}