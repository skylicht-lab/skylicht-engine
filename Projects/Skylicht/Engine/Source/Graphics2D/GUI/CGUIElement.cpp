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
		setName("No Name");
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

		// element data
		object->autoRelease(new CStringProperty(object, "name", getName()));
		object->autoRelease(new CBoolProperty(object, "visible", m_visible));

		// transform
		object->autoRelease(new CVector3Property(object, "position", m_guiTransform->getPosition()));
		object->autoRelease(new CVector3Property(object, "scale", m_guiTransform->getScale()));
		object->autoRelease(new CVector3Property(object, "rotation", m_guiTransform->getRotation()));

		// rect
		object->autoRelease(new CFloatProperty(object, "rectX1", m_guiTransform->Rect.UpperLeftCorner.X));
		object->autoRelease(new CFloatProperty(object, "rectY1", m_guiTransform->Rect.UpperLeftCorner.Y));
		object->autoRelease(new CFloatProperty(object, "rectX2", m_guiTransform->Rect.LowerRightCorner.X));
		object->autoRelease(new CFloatProperty(object, "rectY2", m_guiTransform->Rect.LowerRightCorner.Y));

		// align
		CEnumProperty<EGUIDock>* dock = new CEnumProperty<EGUIDock>(object, "dock", m_guiAlign->Dock);
		dock->addEnumString("NoDock", EGUIDock::NoDock);
		dock->addEnumString("DockLeft", EGUIDock::DockLeft);
		dock->addEnumString("DockRight", EGUIDock::DockRight);
		dock->addEnumString("DockTop", EGUIDock::DockTop);
		dock->addEnumString("DockBottom", EGUIDock::DockBottom);
		dock->addEnumString("DockFill", EGUIDock::DockFill);
		object->autoRelease(dock);

		CEnumProperty<EGUIVerticalAlign>* alignVertical = new CEnumProperty<EGUIVerticalAlign>(object, "alignVertical", m_guiAlign->Vertical);
		alignVertical->addEnumString("Top", EGUIVerticalAlign::Top);
		alignVertical->addEnumString("Middle", EGUIVerticalAlign::Middle);
		alignVertical->addEnumString("Bottom", EGUIVerticalAlign::Bottom);
		object->autoRelease(alignVertical);

		CEnumProperty<EGUIHorizontalAlign>* alignHorizontal = new CEnumProperty<EGUIHorizontalAlign>(object, "alignHorizontal", m_guiAlign->Horizontal);
		alignHorizontal->addEnumString("Left", EGUIHorizontalAlign::Left);
		alignHorizontal->addEnumString("Center", EGUIHorizontalAlign::Center);
		alignHorizontal->addEnumString("Right", EGUIHorizontalAlign::Right);
		object->autoRelease(alignHorizontal);

		// padding
		object->autoRelease(new CFloatProperty(object, "paddingLeft", m_guiAlign->Margin.Left));
		object->autoRelease(new CFloatProperty(object, "paddingTop", m_guiAlign->Margin.Top));
		object->autoRelease(new CFloatProperty(object, "paddingRight", m_guiAlign->Margin.Right));
		object->autoRelease(new CFloatProperty(object, "paddingBottom", m_guiAlign->Margin.Bottom));

		// material
		object->autoRelease(new CColorProperty(object, "color", m_renderData->Color));

		return object;
	}

	void CGUIElement::loadSerializable(CObjectSerializable* object)
	{
		// gui data
		setName(object->get("name", "No name"));
		m_visible = object->get("visible", false);

		// transform
		setPosition(object->get<core::vector3df>("position", core::vector3df()));
		setScale(object->get<core::vector3df>("scale", core::vector3df(1.0f, 1.0f, 1.0f)));
		setRotation(object->get<core::vector3df>("rotation", core::vector3df()));

		// rect
		m_guiTransform->Rect.UpperLeftCorner.X = object->get("rectX1", 0.0f);
		m_guiTransform->Rect.UpperLeftCorner.Y = object->get("rectY1", 0.0f);
		m_guiTransform->Rect.LowerRightCorner.X = object->get("rectX2", 0.0f);
		m_guiTransform->Rect.LowerRightCorner.Y = object->get("rectY2", 0.0f);

		// align
		m_guiAlign->Dock = object->get<EGUIDock>("dock", EGUIDock::NoDock);
		m_guiAlign->Vertical = object->get<EGUIVerticalAlign>("alignVertical", EGUIVerticalAlign::Top);
		m_guiAlign->Horizontal = object->get<EGUIHorizontalAlign>("alignHorizontal", EGUIHorizontalAlign::Left);

		// padding
		m_guiAlign->Margin.Left = object->get("paddingLeft", 0.0f);
		m_guiAlign->Margin.Top = object->get("paddingTop", 0.0f);
		m_guiAlign->Margin.Right = object->get("paddingRight", 0.0f);
		m_guiAlign->Margin.Bottom = object->get("paddingBottom", 0.0f);

		// color
		m_renderData->Color = object->get("color", SColor(255, 255, 255, 255));
	}
}