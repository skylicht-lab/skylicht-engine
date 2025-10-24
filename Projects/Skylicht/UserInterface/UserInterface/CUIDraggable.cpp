/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "CUIDraggable.h"

#include "CUIEventManager.h"

namespace Skylicht
{
	namespace UI
	{
		CUIDraggable::CUIDraggable(CUIContainer* container, CGUIElement* element) :
			CUIBase(container, element),
			m_lockX(false),
			m_lockY(false),
			m_limitDragToRect(false)
		{

		}

		CUIDraggable::~CUIDraggable()
		{

		}

		void CUIDraggable::onPointerDown(int pointerId, float pointerX, float pointerY)
		{
			if (m_pointerId == -1)
			{
				CUIBase::onPointerDown(pointerId, pointerX, pointerY);

				CUIEventManager::getInstance()->setCapture(this);
				onBeginDrag();
			}
		}

		void CUIDraggable::onPointerUp(int pointerId, float pointerX, float pointerY)
		{
			if (m_pointerId == pointerId)
			{
				CUIBase::onPointerUp(pointerId, pointerX, pointerY);

				CUIEventManager::getInstance()->setCapture(NULL);
				onEndDrag();
			}
		}

		void CUIDraggable::onPointerMove(int pointerId, float pointerX, float pointerY)
		{
			CUIBase::onPointerMove(pointerId, pointerX, pointerY);

			if (m_isPointerDown && m_pointerId == pointerId)
				updateDrag();
		}

		void CUIDraggable::onBeginDrag()
		{
			CCanvas* canvas = getCanvas();
			CCamera* camera = canvas->getRenderCamera();
			if (camera == NULL)
				return;

			CUIEventManager* eventMgr = CUIEventManager::getInstance();
			float pointerX = (float)eventMgr->getPointerX();
			float pointerY = (float)eventMgr->getPointerY();

			convertToUICoordinate(pointerX, pointerY);
			convertWorldToLocal(m_element, pointerX, pointerY);
			m_offset.set(pointerX, pointerY);

			m_oldPosition = m_element->getPosition();
			if (OnBeginDrag != nullptr)
				OnBeginDrag(this, m_oldPosition.X, m_oldPosition.Y);
		}

		void CUIDraggable::updateDrag()
		{
			CCanvas* canvas = getCanvas();
			CCamera* camera = canvas->getRenderCamera();
			if (camera == NULL)
				return;

			CUIEventManager* eventMgr = CUIEventManager::getInstance();
			float pointerX = (float)eventMgr->getPointerX();
			float pointerY = (float)eventMgr->getPointerY();

			convertToUICoordinate(pointerX, pointerY);
			convertWorldToLocal(m_element->getParent(), pointerX, pointerY);

			float currentX = pointerX - m_offset.X;
			float currentY = pointerY - m_offset.Y;

			if (m_lockX)
				currentX = m_oldPosition.X;
			if (m_lockY)
				currentY = m_oldPosition.Y;

			if (m_limitDragToRect)
			{
				currentX = core::max_(currentX, m_bounds.UpperLeftCorner.X);
				currentY = core::max_(currentY, m_bounds.UpperLeftCorner.Y);
				currentX = core::min_(currentX, m_bounds.LowerRightCorner.X);
				currentY = core::min_(currentY, m_bounds.LowerRightCorner.Y);
			}

			m_element->setPosition(core::vector3df(currentX, currentY, m_oldPosition.Z));

			if (OnDrag != nullptr)
				OnDrag(this, currentX, currentY);
		}

		void CUIDraggable::onEndDrag()
		{
			m_oldPosition = m_element->getPosition();
			if (OnEndDrag != nullptr)
				OnEndDrag(this, m_oldPosition.X, m_oldPosition.Y);
		}
	}
}