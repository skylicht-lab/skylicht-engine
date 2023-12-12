#include "pch.h"
#include "CUIContainer.h"

#include "Projective/CProjective.h"

namespace Skylicht
{
	namespace UI
	{
		CUIContainer::CUIContainer() :
			m_enable(true)
		{
			CEventManager::getInstance()->registerProcessorEvent(m_name, this);
		}

		CUIContainer::~CUIContainer()
		{
			CEventManager::getInstance()->unRegisterProcessorEvent(this);
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
				return false;

			bool skipAnotherEvent = false;

			if (event.EventType == EET_MOUSE_INPUT_EVENT)
			{
				const core::recti& vp = getVideoDriver()->getViewPort();

				core::vector3df p2d[4];
				m_raycastUIObjects.set_used(0);

				for (CUIBase* base : m_arrayUIObjects)
				{
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
					core::vector3df mousePos(
						(f32)event.MouseInput.X,
						(f32)event.MouseInput.Y,
						0.0f
					);

					if (t1.isPointInside(mousePos) == true || t2.isPointInside(mousePos) == true)
					{
						// mouse over on this
						m_raycastUIObjects.push_back(base);
					}
				}

				if (m_raycastUIObjects.size() == 0)
					return false;

				skipAnotherEvent = true;

				if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
				{

				}
				else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
				{

				}
			}

			return skipAnotherEvent;
		}
	}
}