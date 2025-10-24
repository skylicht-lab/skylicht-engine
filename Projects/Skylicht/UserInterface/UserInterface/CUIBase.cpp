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
			m_multiTouch(false),
			m_enable(true),
			m_visible(true),
			m_isPointerHover(false),
			m_isPointerDown(false),
			m_continueGameEvent(false),
			m_skipPointerEventWhenDrag(false),
			m_pointerId(-1),
			m_pointerDownX(0.0f),
			m_pointerDownY(0.0f)
		{
			m_container->addChild(this);
		}

		CUIBase::~CUIBase()
		{
			CUIEventManager* eventMgr = CUIEventManager::getInstance();
			if (eventMgr && eventMgr->getFocus() == this)
				eventMgr->setFocus(NULL);

			for (int i = 0, n = (int)EMotionEvent::NumEvent; i < n; i++)
				removeMotions((EMotionEvent)i);
		}

		void CUIBase::remove()
		{
			m_container->removeChild(this);
			delete this;
		}

		void CUIBase::update()
		{
			for (int i = 0, n = (int)EMotionEvent::NumEvent; i < n; i++)
			{
				if (OnMotionFinish[i] != nullptr)
				{
					EMotionEvent eventId = (EMotionEvent)i;

					if (!isMotionPlaying(eventId))
					{
						OnMotionFinish[i](this, eventId);
						OnMotionFinish[i] = nullptr;
					}
				}
			}
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

		void CUIBase::onPointerHover(int pointerId, float pointerX, float pointerY)
		{
			m_isPointerHover = true;
			startMotion(EMotionEvent::PointerHover);

			if (OnPointerHover != nullptr)
				OnPointerHover(pointerX, pointerY);
		}

		void CUIBase::onPointerOut(int pointerId, float pointerX, float pointerY)
		{
			m_isPointerHover = false;
			startMotion(EMotionEvent::PointerOut);

			if (OnPointerOut != nullptr)
				OnPointerOut(pointerX, pointerY);
		}

		void CUIBase::onPointerDown(int pointerId, float pointerX, float pointerY)
		{
			if (m_pointerId == -1)
			{
				m_isPointerDown = true;
				m_pointerId = pointerId;
				m_pointerDownX = pointerX;
				m_pointerDownY = pointerY;

				startMotion(EMotionEvent::PointerDown);

				if (OnPointerDown != nullptr)
					OnPointerDown(pointerX, pointerY);
			}
		}

		void CUIBase::onPointerUp(int pointerId, float pointerX, float pointerY)
		{
			if (m_pointerId == pointerId)
			{
				m_isPointerDown = false;
				m_pointerId = -1;
				startMotion(EMotionEvent::PointerUp);

				if (OnPointerUp != nullptr)
					OnPointerUp(pointerX, pointerY);
			}
		}

		void CUIBase::onPointerMove(int pointerId, float pointerX, float pointerY)
		{
			if (OnPointerMove != nullptr)
				OnPointerMove(pointerX, pointerY, m_isPointerDown);

			if (m_skipPointerEventWhenDrag && m_isPointerDown && m_pointerId == pointerId)
			{
				if (fabsf(pointerX - m_pointerDownX) > 10.0f ||
					fabsf(pointerY - m_pointerDownY) > 10.0f)
				{
					m_container->cancelPointerDown(this, pointerId, pointerX, pointerY);
				}
			}
		}

		void CUIBase::onPressed()
		{
			if (OnPressed != nullptr)
				OnPressed(this);
		}

		void CUIBase::onFocus()
		{
			if (OnFocus != nullptr)
				OnFocus(this);
		}

		void CUIBase::onLostFocus()
		{
			if (OnLostFocus != nullptr)
				OnLostFocus(this);
		}

		void CUIBase::onKeyEvent(const SEvent& event)
		{
			if (OnKeyEvent != nullptr)
				OnKeyEvent(this, event);
		}

		void CUIBase::startMotion(EMotionEvent event)
		{
			if (!m_element)
				return;

			if (m_motions[(int)event].size() == 0)
				return;

			std::vector<CMotion*> startMotions = m_motions[(int)event];

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
							for (CMotion* st : startMotions)
							{
								if (typeid(m) == typeid(st))
								{
									// just stop same motion type
									m->stop();
									break;
								}
							}
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

		void CUIBase::resetPointer()
		{
			m_isPointerHover = false;
			m_isPointerDown = false;
		}

		void CUIBase::convertToUICoordinate(float& pointerX, float& pointerY)
		{
			CCanvas* canvas = getCanvas();
			if (canvas == NULL)
			{
				pointerX = 0.0f;
				pointerY = 0.0f;
				return;
			}

			CCamera* camera = canvas->getRenderCamera();
			if (camera == NULL)
			{
				pointerX = 0.0f;
				pointerY = 0.0f;
				return;
			}

			// we convert pointer (pixel xy) to render viewport like this,
			// that will fix for 3d canvas
			// 
			// (-1,1)              (1,1)
			//
			//
			//          (0,0)
			//
			//
			// (-1,-1)             (1,-1)

			const core::recti& vp = getVideoDriver()->getViewPort();
			pointerX = pointerX / (vp.getWidth() * 0.5f) - 1.0f;
			pointerY = 1.0f - pointerY / (vp.getHeight() * 0.5f);

			core::matrix4 trans = camera->getProjectionMatrix();
			trans *= camera->getViewMatrix();
			trans.makeInverse();

			// convert Screen to UI coordinate axis
			core::vector3df pointer(pointerX, pointerY, 0.0f);
			trans.transformVect(pointer);

			pointerX = pointer.X;
			pointerY = pointer.Y;
		}

		void CUIBase::convertWorldToLocal(CGUIElement* element, float& x, float& y)
		{
			CCanvas* canvas = getCanvas();
			if (canvas == NULL)
			{
				x = 0.0f;
				y = 0.0f;
				return;
			}

			const core::matrix4& canvasTransform = canvas->getRenderWorldTransform();
			const core::matrix4& elementTransform = element->getAbsoluteTransform();

			core::matrix4 world = canvasTransform * elementTransform;
			core::matrix4 worldInv(world, core::matrix4::EM4CONST_INVERSE);

			core::vector3df pointer(x, y, 0.0f);
			worldInv.transformVect(pointer);

			x = pointer.X;
			y = pointer.Y;
		}

		void CUIBase::convertLocalToWorld(CGUIElement* element, float& x, float& y)
		{
			CCanvas* canvas = getCanvas();
			if (canvas == NULL)
			{
				x = 0.0f;
				y = 0.0f;
				return;
			}

			const core::matrix4& canvasTransform = canvas->getRenderWorldTransform();
			const core::matrix4& elementTransform = element->getAbsoluteTransform();

			core::matrix4 world = canvasTransform * elementTransform;

			core::vector3df pointer(x, y, 0.0f);
			world.transformVect(pointer);

			x = pointer.X;
			y = pointer.Y;
		}
	}
}