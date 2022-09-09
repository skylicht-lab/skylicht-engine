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
		m_vertical(Top),
		m_horizontal(Left),
		m_dock(NoDock),
		m_visible(true),
		m_parent(NULL),
		m_cullingVisible(true),
		m_color(255, 255, 255, 255),
		m_mask(NULL),
		m_applyCurrentMask(NULL),
		m_material(NULL)
	{
		CEntityPrefab* entityPrefab = m_canvas->getEntityManager();
		m_entity = entityPrefab->createEntity();
		entityPrefab->addTransformData(m_entity, NULL, core::IdentityMatrix, "");
		m_guiTransform = m_entity->addData<CGUITransformData>();
		m_guiAlign = m_entity->addData<CGUIAlignData>();

		m_transform = GET_ENTITY_DATA(m_entity, CWorldTransformData);

		setParent(parent);
		setRect(parent->getRect());

		m_shaderID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");
	}

	CGUIElement::CGUIElement(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect) :
		m_canvas(canvas),
		m_vertical(Top),
		m_horizontal(Left),
		m_dock(NoDock),
		m_visible(true),
		m_parent(NULL),
		m_cullingVisible(true),
		m_color(255, 255, 255, 255),
		m_mask(NULL),
		m_applyCurrentMask(NULL),
		m_material(NULL)
	{
		CEntityPrefab* entityPrefab = m_canvas->getEntityManager();
		m_entity = entityPrefab->createEntity();
		entityPrefab->addTransformData(m_entity, NULL, core::IdentityMatrix, "");
		m_guiTransform = m_entity->addData<CGUITransformData>();
		m_guiAlign = m_entity->addData<CGUIAlignData>();

		m_transform = GET_ENTITY_DATA(m_entity, CWorldTransformData);

		setParent(parent);
		setRect(rect);

		m_shaderID = CShaderManager::getInstance()->getShaderIDByName("TextureColorAlpha");
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
		}
		else
		{
			entityMgr->changeParent(m_entity, NULL);
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

	/*
	void CGUIElement::layout(const core::rectf& parentRect)
	{
		switch (m_dock)
		{
		case CGUIElement::NoDock:
			layoutNoDock(parentRect);
			break;
		case CGUIElement::DockLeft:
		case CGUIElement::DockRight:
		case CGUIElement::DockTop:
		case CGUIElement::DockBottom:
		case CGUIElement::DockFill:
			layoutDock(parentRect);
		default:
			break;
		}

		calcAbsoluteTransform();

		for (int i = 0, n = (int)m_childs.size(); i < n; i++)
		{
			m_childs[i]->layout(getRect());
		}
	}

	void CGUIElement::layoutNoDock(const core::rectf& parentRect)
	{
		m_transformPosition.Z = m_position.Z;

		switch (m_vertical)
		{
		case CGUIElement::Top:
			m_transformPosition.Y = m_position.Y;
			break;
		case CGUIElement::Middle:
			m_transformPosition.Y = (parentRect.getHeight() - m_rect.getHeight()) * 0.5f + m_position.Y;
			break;
		case CGUIElement::Bottom:
			m_transformPosition.Y = parentRect.LowerRightCorner.Y - m_rect.getHeight() - m_position.Y;
			break;
		default:
			break;
		}

		switch (m_horizontal)
		{
		case CGUIElement::Left:
			m_transformPosition.X = m_position.X;
			break;
		case CGUIElement::Center:
			m_transformPosition.X = (parentRect.getWidth() - m_rect.getWidth()) * 0.5f + m_position.X;
			break;
		case CGUIElement::Right:
			m_transformPosition.X = parentRect.LowerRightCorner.X - m_rect.getWidth() - m_position.Y;
			break;
		default:
			break;
		}
	}

	void CGUIElement::layoutDock(const core::rectf& parentRect)
	{
		if (m_dock == DockFill)
		{
			m_rect = parentRect;

			m_transformPosition.set(0.0f, 0.0f, 0.0f);

			m_rect.UpperLeftCorner.X = m_rect.UpperLeftCorner.X + m_margin.Left;
			m_rect.UpperLeftCorner.Y = m_rect.UpperLeftCorner.Y + m_margin.Top;
			m_rect.LowerRightCorner.X = m_rect.LowerRightCorner.X - m_margin.Right;
			m_rect.LowerRightCorner.Y = m_rect.LowerRightCorner.Y - m_margin.Bottom;
		}
		else if (m_dock == DockLeft)
		{
			m_transformPosition.set(m_margin.Left, m_margin.Top, 0.0f);

			m_rect.UpperLeftCorner.Y = parentRect.UpperLeftCorner.Y;
			m_rect.LowerRightCorner.Y = parentRect.LowerRightCorner.Y - m_margin.Bottom - m_margin.Top;
		}
		else if (m_dock == DockRight)
		{
			m_transformPosition.set(0.0f, m_margin.Top, 0.0f);
			m_transformPosition.X = parentRect.getWidth() - m_rect.getWidth() - m_margin.Left;

			m_rect.UpperLeftCorner.Y = parentRect.UpperLeftCorner.Y;
			m_rect.LowerRightCorner.Y = parentRect.LowerRightCorner.Y - m_margin.Bottom - m_margin.Top;
		}
		else if (m_dock == DockTop)
		{
			m_transformPosition.set(m_margin.Left, m_margin.Top, 0.0f);

			m_rect.UpperLeftCorner.X = parentRect.UpperLeftCorner.X;
			m_rect.LowerRightCorner.X = parentRect.LowerRightCorner.X - m_margin.Right - m_margin.Left;
		}
		else if (m_dock == DockBottom)
		{
			m_transformPosition.set(m_margin.Left, 0.0f, 0.0f);
			m_transformPosition.Y = parentRect.getHeight() - m_rect.getHeight() - m_margin.Bottom;

			m_rect.UpperLeftCorner.X = parentRect.UpperLeftCorner.X;
			m_rect.LowerRightCorner.X = parentRect.LowerRightCorner.X - m_margin.Right - m_margin.Left;
		}

		m_position = m_transformPosition;
	}
	*/

	const core::matrix4& CGUIElement::getRelativeTransform()
	{
		m_transform->Relative.makeIdentity();
		/*
		m_transform->Relative.setRotationDegrees(m_rotation);

		f32* m = m_transform->Relative.pointer();

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

		m[12] = m_transformPosition.X;
		m[13] = m_transformPosition.Y;
		m[14] = m_transformPosition.Z;

		*/
		return m_transform->Relative;
	}

	void CGUIElement::calcAbsoluteTransform()
	{
		if (m_parent == NULL)
		{
			// alway identity at root transform
			// that fix for canvas billboard
			m_transform->World = core::IdentityMatrix;
		}
		else
		{
			m_transform->World.setbyproduct_nocheck(m_parent->getAbsoluteTransform(), getRelativeTransform());
		}
	}
}