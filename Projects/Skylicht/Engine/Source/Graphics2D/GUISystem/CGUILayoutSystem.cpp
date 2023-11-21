/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CGUILayoutSystem.h"

namespace Skylicht
{
	CGUILayoutSystem::CGUILayoutSystem()
	{

	}

	CGUILayoutSystem::~CGUILayoutSystem()
	{

	}

	void CGUILayoutSystem::beginQuery(CEntityManager* entityManager)
	{
		m_worldTransforms.reset();
		m_guiTransform.reset();
		m_guiAlign.reset();
		m_guiLayout.reset();
		m_guiLayoutTransform.reset();
	}

	void CGUILayoutSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int count)
	{
		for (int i = 0; i < count; i++)
		{
			CEntity* e = entities[i];

			CWorldTransformData* w = GET_ENTITY_DATA(e, CWorldTransformData);
			CGUITransformData* t = GET_ENTITY_DATA(e, CGUITransformData);
			CGUIAlignData* a = GET_ENTITY_DATA(e, CGUIAlignData);
			CGUIChildLayoutData* childLayout = GET_ENTITY_DATA(e, CGUIChildLayoutData);
			CGUILayoutData* l = GET_ENTITY_DATA(e, CGUILayoutData);

			if (t->HasChanged || (w->Parent && w->Parent->HasChanged))
				w->HasChanged = true;

			if (l)
			{
				l->CurrentX = 0.0f;
				l->CurrentY = 0.0f;
			}

			m_worldTransforms.push(w);
			m_guiTransform.push(t);
			m_guiAlign.push(a);

			if (childLayout)
			{
				// reset to no dock align
				a->Dock = EGUIDock::NoDock;
				a->Vertical = EGUIVerticalAlign::Top;
				a->Horizontal = EGUIHorizontalAlign::Left;

				// reset rect transform
				float width = t->Rect.getWidth();
				float height = t->Rect.getHeight();
				t->Rect.UpperLeftCorner.set(0.0f, 0.0f);
				t->Rect.LowerRightCorner.set(width, height);

				// add for layout system
				m_guiLayout.push(childLayout);
				m_guiLayoutTransform.push(t);
			}
		}
	}

	void CGUILayoutSystem::init(CEntityManager* entityManager)
	{

	}

	void CGUILayoutSystem::update(CEntityManager* entityManager)
	{
		updateLayout();
		updateAlign();
		updateTransform();
	}

	void CGUILayoutSystem::updateLayout()
	{
		CGUIChildLayoutData** layouts = m_guiLayout.pointer();
		CGUITransformData** transforms = m_guiLayoutTransform.pointer();

		int numEntity = m_guiLayout.count();
		for (int i = 0; i < numEntity; i++)
		{
			CGUIChildLayoutData* layout = layouts[i];
			CGUITransformData* transform = transforms[i];
			CGUILayoutData* parentLayoutInfo = layout->Parent;

			if (!parentLayoutInfo)
				continue;

			float h = transform->Rect.getHeight();
			float w = transform->Rect.getWidth();

			switch (parentLayoutInfo->AlignType)
			{
			case CGUILayoutData::Vertical:
			{
				float y = parentLayoutInfo->CurrentY;

				transform->setPosition(core::vector3df(0.0f, y, 0.0f));

				y = y + h;
				y = y + parentLayoutInfo->Spacing;

				parentLayoutInfo->CurrentY = y;
				parentLayoutInfo->MaxW = core::max_(parentLayoutInfo->MaxW, w);
				parentLayoutInfo->MaxH = y - parentLayoutInfo->Spacing;
			}
			break;
			case CGUILayoutData::Horizontal:
			{
				float x = parentLayoutInfo->CurrentX;

				transform->setPosition(core::vector3df(x, 0.0f, 0.0f));

				x = x + w;
				x = x + parentLayoutInfo->Spacing;

				parentLayoutInfo->CurrentX = x;
				parentLayoutInfo->MaxH = core::max_(parentLayoutInfo->MaxH, h);
				parentLayoutInfo->MaxW = x - parentLayoutInfo->Spacing;
			}
			break;
			}
		}
	}

	void CGUILayoutSystem::updateAlign()
	{
		int numEntity = m_worldTransforms.count();

		CGUITransformData** guiTransforms = m_guiTransform.pointer();
		CGUIAlignData** guiAlign = m_guiAlign.pointer();

		for (int i = 0; i < numEntity; i++)
		{
			CGUITransformData* t = guiTransforms[i];
			CGUIAlignData* a = guiAlign[i];

			core::rectf parentRect = t->Parent ? t->Parent->Rect : t->Rect;

			switch (a->Dock)
			{
			case EGUIDock::NoDock:
				layoutNoDock(t, a, parentRect);
				break;
			case EGUIDock::DockLeft:
			case EGUIDock::DockRight:
			case EGUIDock::DockTop:
			case EGUIDock::DockBottom:
			case EGUIDock::DockFill:
				layoutDock(t, a, parentRect);
				break;
			default:
				break;
			}
		}
	}

	void CGUILayoutSystem::layoutNoDock(CGUITransformData* t, CGUIAlignData* a, const core::rectf& parentRect)
	{
		t->m_transformPosition.Z = t->m_position.Z;

		switch (a->Vertical)
		{
		case EGUIVerticalAlign::Top:
			t->m_transformPosition.Y = parentRect.UpperLeftCorner.Y + t->m_position.Y;
			break;
		case EGUIVerticalAlign::Middle:
			t->m_transformPosition.Y = parentRect.UpperLeftCorner.Y + (parentRect.getHeight() - t->Rect.getHeight()) * 0.5f + t->m_position.Y;
			break;
		case EGUIVerticalAlign::Bottom:
			t->m_transformPosition.Y = parentRect.LowerRightCorner.Y - t->Rect.getHeight() - t->m_position.Y;
			break;
		default:
			break;
		}

		switch (a->Horizontal)
		{
		case EGUIHorizontalAlign::Left:
			t->m_transformPosition.X = parentRect.UpperLeftCorner.X + t->m_position.X;
			break;
		case EGUIHorizontalAlign::Center:
			t->m_transformPosition.X = parentRect.UpperLeftCorner.X + (parentRect.getWidth() - t->Rect.getWidth()) * 0.5f + t->m_position.X;
			break;
		case EGUIHorizontalAlign::Right:
			t->m_transformPosition.X = parentRect.LowerRightCorner.X - t->Rect.getWidth() - t->m_position.Y;
			break;
		default:
			break;
		}
	}

	void CGUILayoutSystem::layoutDock(CGUITransformData* t, CGUIAlignData* a, const core::rectf& parentRect)
	{
		if (a->Dock == EGUIDock::DockFill)
		{
			t->m_transformPosition.set(0.0f, 0.0f, 0.0f);

			t->Rect = parentRect;
			t->Rect.UpperLeftCorner.X = t->Rect.UpperLeftCorner.X + a->Margin.Left;
			t->Rect.UpperLeftCorner.Y = t->Rect.UpperLeftCorner.Y + a->Margin.Top;
			t->Rect.LowerRightCorner.X = t->Rect.LowerRightCorner.X - a->Margin.Right;
			t->Rect.LowerRightCorner.Y = t->Rect.LowerRightCorner.Y - a->Margin.Bottom;
		}
		else if (a->Dock == EGUIDock::DockLeft)
		{
			t->m_transformPosition.set(parentRect.UpperLeftCorner.X + a->Margin.Left, a->Margin.Top, 0.0f);

			t->Rect.UpperLeftCorner.Y = parentRect.UpperLeftCorner.Y;
			t->Rect.LowerRightCorner.Y = parentRect.LowerRightCorner.Y - a->Margin.Bottom - a->Margin.Top;
		}
		else if (a->Dock == EGUIDock::DockRight)
		{
			t->m_transformPosition.set(0.0f, a->Margin.Top, 0.0f);
			t->m_transformPosition.X = parentRect.UpperLeftCorner.X + parentRect.getWidth() - t->Rect.getWidth() - a->Margin.Right;

			t->Rect.UpperLeftCorner.Y = parentRect.UpperLeftCorner.Y;
			t->Rect.LowerRightCorner.Y = parentRect.LowerRightCorner.Y - a->Margin.Bottom - a->Margin.Top;
		}
		else if (a->Dock == EGUIDock::DockTop)
		{
			t->m_transformPosition.set(
				a->Margin.Left,
				parentRect.UpperLeftCorner.Y + a->Margin.Top,
				0.0f);

			t->Rect.UpperLeftCorner.X = parentRect.UpperLeftCorner.X;
			t->Rect.LowerRightCorner.X = parentRect.LowerRightCorner.X - a->Margin.Right - a->Margin.Left;
		}
		else if (a->Dock == EGUIDock::DockBottom)
		{
			t->m_transformPosition.set(a->Margin.Left, 0.0f, 0.0f);
			t->m_transformPosition.Y = parentRect.UpperLeftCorner.Y + parentRect.getHeight() - t->Rect.getHeight() - a->Margin.Bottom;

			t->Rect.UpperLeftCorner.X = parentRect.UpperLeftCorner.X;
			t->Rect.LowerRightCorner.X = parentRect.LowerRightCorner.X - a->Margin.Right - a->Margin.Left;
		}
		else
		{
			t->m_transformPosition = t->m_position;
		}

		if (t->Parent != NULL)
			t->m_position = t->m_transformPosition;
	}

	void CGUILayoutSystem::updateTransform()
	{
		int numEntity = m_worldTransforms.count();

		CWorldTransformData** transforms = m_worldTransforms.pointer();
		CGUITransformData** guiTransforms = m_guiTransform.pointer();

		for (int i = 0; i < numEntity; i++)
		{
			CWorldTransformData* w = transforms[i];
			CGUITransformData* t = guiTransforms[i];

			if (w->HasChanged)
			{
				// update relative matrix
				w->Relative.makeIdentity();
				w->Relative.setRotationDegrees(t->m_rotation);

				f32* m = w->Relative.pointer();

				m[0] *= t->m_scale.X;
				m[1] *= t->m_scale.X;
				m[2] *= t->m_scale.X;
				m[3] *= t->m_scale.X;

				m[4] *= t->m_scale.Y;
				m[5] *= t->m_scale.Y;
				m[6] *= t->m_scale.Y;
				m[7] *= t->m_scale.Y;

				m[8] *= t->m_scale.Z;
				m[9] *= t->m_scale.Z;
				m[10] *= t->m_scale.Z;
				m[11] *= t->m_scale.Z;

				m[12] = t->m_transformPosition.X;
				m[13] = t->m_transformPosition.Y;
				m[14] = t->m_transformPosition.Z;

				// update world matrix
				if (w->Parent)
					w->World.setbyproduct_nocheck(w->Parent->World, w->Relative);
				else
				{
					// The relative will set on render (driver->setTransform)
					w->World = core::IdentityMatrix;
				}

				// notify transform updated
				t->HasChanged = false;
				w->HasChanged = false;
			}
		}
	}
}