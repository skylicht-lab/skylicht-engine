/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
#include "CUIBase.h"
#include "CUIContainer.h"

namespace Skylicht
{
	namespace UI
	{
		CUIBase::CUIBase(CUIContainer* container, CGUIElement* element) :
			m_container(container),
			m_element(element),
			m_enable(true),
			m_visible(true),
			m_isPointerHover(false),
			m_isPointerDown(false)
		{
			m_container->addChild(this);
		}

		CUIBase::~CUIBase()
		{
			m_container->removeChild(this);

			for (int i = 0, n = (int)EMotionEvent::NumEvent; i < n; i++)
				removeMotions((EMotionEvent)i);
		}

		void CUIBase::setEnable(bool b)
		{
			m_enable = b;
		}

		void CUIBase::setVisible(bool b)
		{
			m_visible = b;
			if (m_element)
				m_element->setVisible(b);
		}

		core::vector3df* CUIBase::getRectTransform()
		{
			if (!m_element)
				return NULL;

			// get current canvas
			CCanvas* canvas = m_element->getCanvas();

			// get last camera, that render this button
			CCamera* camera = canvas->getRenderCamera();
			if (camera == NULL)
				return m_rectTransform;

			const core::matrix4& world = canvas->getRenderWorldTransform();
			const core::matrix4& elementTranform = m_element->getAbsoluteTransform();

			// real world transform
			core::matrix4 worldElementTransform = world * elementTranform;

			core::rectf r = m_element->getRect();

			m_rectTransform[0].set(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, 0.0f);
			m_rectTransform[1].set(r.LowerRightCorner.X, r.UpperLeftCorner.Y, 0.0f);
			m_rectTransform[2].set(r.UpperLeftCorner.X, r.LowerRightCorner.Y, 0.0f);
			m_rectTransform[3].set(r.LowerRightCorner.X, r.LowerRightCorner.Y, 0.0f);

			for (int i = 0; i < 4; i++)
			{
				// get real 3d position
				worldElementTransform.transformVect(m_rectTransform[i]);
			}

			return m_rectTransform;
		}

		void CUIBase::onPointerHover(float pointerX, float pointerY)
		{
			m_isPointerHover = true;
			startMotion(EMotionEvent::PointerHover);

			if (OnPointerHover != nullptr)
				OnPointerHover(pointerX, pointerY);
		}

		void CUIBase::onPointerOut(float pointerX, float pointerY)
		{
			m_isPointerHover = false;
			startMotion(EMotionEvent::PointerOut);

			if (OnPointerOut != nullptr)
				OnPointerOut(pointerX, pointerY);
		}

		void CUIBase::onPointerDown(float pointerX, float pointerY)
		{
			m_isPointerDown = true;
			startMotion(EMotionEvent::PointerDown);

			if (OnPointerDown != nullptr)
				OnPointerDown(pointerX, pointerY);
		}

		void CUIBase::onPointerUp(float pointerX, float pointerY)
		{
			m_isPointerDown = false;
			startMotion(EMotionEvent::PointerUp);

			if (OnPointerUp != nullptr)
				OnPointerUp(pointerX, pointerY);
		}

		void CUIBase::onPressed()
		{
			if (OnPressed != nullptr)
				OnPressed();
		}

		void CUIBase::startMotion(EMotionEvent event)
		{
			if (!m_element)
				return;

			if (m_motions[(int)event].size() == 0)
				return;

			for (int i = 0, n = (int)EMotionEvent::NumEvent; i < n; i++)
			{
				EMotionEvent e = (EMotionEvent)i;

				std::vector<CMotion*>& motions = m_motions[i];
				for (CMotion* m : motions)
				{
					if (e == event)
						m->start();
					else
					{
						if (e != EMotionEvent::In &&
							e != EMotionEvent::Out)
						{
							m->stop();
						}
					}
				}
			}
		}

		bool CUIBase::isMotionPlaying(EMotionEvent event)
		{
			if (!m_element)
				return false;

			std::vector<CMotion*>& motions = m_motions[(int)event];
			if (motions.size() == 0)
				return false;

			for (CMotion* m : motions)
			{
				if (m->isPlaying())
					return true;
			}

			return false;
		}

		CMotion* CUIBase::addMotion(EMotionEvent event, CMotion* motion)
		{
			if (m_element)
			{
				std::vector<CMotion*>& motions = m_motions[(int)event];
				motions.push_back(motion);

				motion->setEvent(event);
				motion->init(m_element);
			}
			else
			{
				os::Printer::log("[CUIBase] addMotion with m_element = Null");
			}
			return motion;
		}

		CMotion* CUIBase::addMotion(EMotionEvent event, CGUIElement* element, CMotion* motion)
		{
			if (element)
			{
				std::vector<CMotion*>& motions = m_motions[(int)event];
				motions.push_back(motion);

				motion->setEvent(event);
				motion->init(element);
			}
			else
			{
				os::Printer::log("[CUIBase] addMotion with element = Null");
			}
			return motion;
		}

		bool CUIBase::removeMotion(EMotionEvent event, CMotion* motion)
		{
			std::vector<CMotion*>& motions = m_motions[(int)event];
			auto it = std::find(motions.begin(), motions.end(), motion);
			if (it != motions.end())
			{
				motions.erase(it);
				delete motion;
				return true;
			}
			return false;
		}

		void CUIBase::removeMotions(EMotionEvent event)
		{
			std::vector<CMotion*>& motions = m_motions[(int)event];
			for (CMotion* m : motions)
				delete m;
			motions.clear();
		}
	}
}