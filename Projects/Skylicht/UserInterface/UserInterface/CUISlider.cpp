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
#include "CUISlider.h"

#include "CUIEventManager.h"

namespace Skylicht
{
	namespace UI
	{
		CUISlider::CUISlider(CUIContainer* container, CGUIElement* element) :
			CUIBase(container, element),
			m_background(NULL),
			m_fillBar(NULL),
			m_handle(NULL),
			m_mask(NULL),
			m_buttonHander(NULL),
			m_value(1.0f),
			m_handleWidth(0.0f)
		{
			if (m_element)
			{
				CCanvas* canvas = getCanvas();
				m_background = canvas->getGUIByPath(element, "Background");
				m_fillBar = canvas->getGUIByPath(element, "FillBar");
				m_handle = canvas->getGUIByPath(element, "Handle");

				m_mask = canvas->createMask(m_element, m_element->getRect());

				if (m_background)
					element->bringToNext(m_mask, m_background, true);

				if (m_fillBar)
					m_fillBar->setMask(m_mask);

				if (m_handle)
				{
					m_handleWidth = m_handle->getWidth();

					m_buttonHander = new UI::CUIBase(container, m_handle);
					m_buttonHander->OnPointerDown = [&](float pointerX, float pointerY)
						{
							CUIEventManager::getInstance()->setCapture(m_buttonHander);
							onBeginDrag();
						};
					m_buttonHander->OnPointerUp = [&](float pointerX, float pointerY)
						{
							CUIEventManager::getInstance()->setCapture(NULL);
							onEndDrag();
						};
					m_buttonHander->OnPointerMove = [&](float pointerX, float pointerY, bool pointerDown)
						{
							if (pointerDown)
								updateDrag();
						};
				}
			}
		}

		CUISlider::~CUISlider()
		{
			if (m_buttonHander)
				m_buttonHander->remove();
		}

		void CUISlider::onBeginDrag()
		{
			CCanvas* canvas = getCanvas();
			CCamera* camera = canvas->getRenderCamera();
			if (camera == NULL)
				return;

			CUIEventManager* eventMgr = CUIEventManager::getInstance();
			float pointerX = (float)eventMgr->getPointerX();
			float pointerY = (float)eventMgr->getPointerY();

			convertToUICoordinate(pointerX, pointerY);
			convertWorldToLocal(m_handle, pointerX, pointerY);
			m_offset.set(pointerX, pointerY);
		}

		void CUISlider::updateDrag()
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

			float sliderWidth = m_element->getWidth();
			float sliderBegin = -m_handleWidth * 0.5f;
			float sliderEnd = sliderWidth - m_handleWidth * 0.5f;

			float currentX = pointerX - m_offset.X;
			float currentY = m_handle->getPosition().Y;
			float currentZ = m_handle->getPosition().Z;

			currentX = core::clamp(currentX, sliderBegin, sliderEnd);

			float sliderBar = currentX - sliderBegin;

			if (m_mask)
			{
				core::rectf r = m_mask->getRect();
				r.LowerRightCorner.X = r.UpperLeftCorner.X + sliderBar;
				m_mask->setRect(r);
			}

			if (sliderWidth == 0.0f)
				m_value = 0.0f;
			else
			{
				m_value = sliderBar / sliderWidth;
				m_value = core::clamp(m_value, 0.0f, 1.0f);
			}

			m_handle->setPosition(core::vector3df(currentX, currentY, currentZ));

			if (OnChanged != nullptr)
				OnChanged(this, m_value);
		}

		void CUISlider::onEndDrag()
		{

		}

		void CUISlider::setValue(float value, bool invokeEvent)
		{
			m_value = value;
			m_value = core::clamp(m_value, 0.0f, 1.0f);

			float sliderWidth = m_element->getWidth();
			float sliderBegin = -m_handleWidth * 0.5f;
			float sliderEnd = sliderWidth - m_handleWidth * 0.5f;

			float sliderBar = sliderWidth * value;

			float currentX = sliderBegin + sliderBar;
			float currentY = m_handle->getPosition().Y;

			currentX = core::clamp(currentX, sliderBegin, sliderEnd);

			if (m_mask)
			{
				core::rectf r = m_mask->getRect();
				r.LowerRightCorner.X = r.UpperLeftCorner.X + sliderBar;
				m_mask->setRect(r);
			}

			m_handle->setPosition(core::vector3df(currentX, currentY, 0.0f));

			if (invokeEvent)
			{
				if (OnChanged != nullptr)
					OnChanged(this, m_value);
			}
		}
	}
}