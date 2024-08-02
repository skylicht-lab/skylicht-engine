#include "pch.h"
#include "CUIContainer.h"

#include "Projective/CProjective.h"
#include "Control/CTouchManager.h"

namespace Skylicht
{
	namespace UI
	{
		CUIContainer::CUIContainer() :
			m_enable(true),
			m_hover(NULL)
		{

		}

		CUIContainer::~CUIContainer()
		{
			CEventManager::getInstance()->unRegisterProcessorEvent(this);

			// delete all ui
			std::vector<CUIBase*> objects = m_arrayUIObjects;
			for (CUIBase* base : objects)
				delete base;
		}

		void CUIContainer::initComponent()
		{
			CEventManager::getInstance()->registerProcessorEvent(m_gameObject->getNameA(), this);
		}

		void CUIContainer::updateComponent()
		{

		}

		void CUIContainer::addChild(CUIBase* base)
		{
			m_arrayUIObjects.push_back(base);
		}

		bool CUIContainer::removeChild(CUIBase* base)
		{
			std::vector<CUIBase*>::iterator i = m_arrayUIObjects.begin(), end = m_arrayUIObjects.end();
			while (i != end)
			{
				if (*i == base)
				{
					m_arrayUIObjects.erase(i);
					return true;
				}
				++i;
			}
			return false;
		}

		bool CUIContainer::OnProcessEvent(const SEvent& event)
		{
			if (!m_enable)
				return true;

			if (event.EventType == EET_MOUSE_INPUT_EVENT)
			{
				const core::recti& vp = getVideoDriver()->getViewPort();

				core::vector3df p2d[4];
				m_raycastUIObjects.clear();

				f32 mouseX = (f32)event.MouseInput.X;
				f32 mouseY = (f32)event.MouseInput.Y;
				core::vector3df mousePos(mouseX, mouseY, 0.0f);

				for (CUIBase* base : m_arrayUIObjects)
				{
					if (base->getElement() == NULL)
						continue;

					CCanvas* canvas = base->getElement()->getCanvas();
					CCamera* camera = canvas->getRenderCamera();
					if (camera == NULL)
						continue;

					core::vector3df* rectTransform = base->getRectTransform();

					for (int i = 0; i < 4; i++)
					{
						// project to 2d screen
						CProjective::getScreenCoordinatesFrom3DPosition(
							camera, rectTransform[i],
							p2d[i].X,
							p2d[i].Y,
							vp.getWidth(),
							vp.getHeight());
						p2d[i].Z = 0.0f;
					}

					// hit test in 3D - apply for 3D UI
					core::triangle3df t1(p2d[0], p2d[1], p2d[2]);
					core::triangle3df t2(p2d[2], p2d[1], p2d[3]);

					if (t1.isPointInside(mousePos) == true || t2.isPointInside(mousePos) == true)
					{
						m_raycastUIObjects.push_back(base);
					}
				}

				if (m_raycastUIObjects.size() == 0)
				{
					if (m_hover)
					{
						m_hover->onPointerOut(mouseX, mouseY);
						m_hover = NULL;
					}
					return true;
				}

				std::sort(m_raycastUIObjects.begin(), m_raycastUIObjects.end(), [](CUIBase*& a, CUIBase*& b)
					{
						return a->getElement()->getDepth() < b->getElement()->getDepth();
					});

				if (m_hover != m_raycastUIObjects[0])
				{
					if (m_hover)
						m_hover->onPointerOut(mouseX, mouseY);

					m_hover = m_raycastUIObjects[0];
					m_hover->onPointerHover(mouseX, mouseY);
				}

				if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
				{
					if (m_hover)
						m_hover->onPointerDown(mouseX, mouseY);
				}
				else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
				{
					if (m_hover)
					{
						m_hover->onPointerUp(mouseX, mouseY);
						m_hover->onPressed();
					}
				}

				// disable event
				return false;
			}

			return true;
		}
	}
}