#include "pch.h"
#include "CUIContainer.h"

#include "Projective/CProjective.h"
#include "Control/CTouchManager.h"
#include "GameObject/CGameObject.h"

namespace Skylicht
{
	namespace UI
	{
		CUIContainer::CUIContainer() :
			m_enable(true),
			m_hover(NULL),
			m_canvas(NULL),
			m_inMotion(false),
			m_outMotion(false),
			m_pointerDown(false)
		{

		}

		CUIContainer::~CUIContainer()
		{
			if (CUIEventManager::getInstance() != NULL)
				CUIEventManager::getInstance()->unregisterUIContainer(this);

			// delete all ui
			std::vector<CUIBase*> objects = m_arrayUIObjects;
			for (CUIBase* base : objects)
				base->remove();
		}

		void CUIContainer::initComponent()
		{
			if (getCanvas() == NULL)
			{
				os::Printer::log("[CUIContainer] Should add CCanvas in this Object");
				return;
			}

			if (CUIEventManager::getInstance() == NULL)
			{
				os::Printer::log("[CUIContainer] Should call CUIEventManager::createGetInstance");
				return;
			}

			CUIEventManager::getInstance()->registerUIContainer(this);
		}

		void CUIContainer::updateComponent()
		{
			if (m_inMotion)
			{
				bool isPlaying = false;
				for (CUIBase* base : m_arrayUIObjects)
				{
					if (base->isMotionPlaying(UI::EMotionEvent::In))
					{
						isPlaying = true;
						break;
					}
				}

				if (isPlaying == false)
				{
					m_inMotion = false;
					if (OnMotionInFinish != nullptr)
						OnMotionInFinish();
				}
			}

			if (m_outMotion)
			{
				bool isPlaying = false;
				for (CUIBase* base : m_arrayUIObjects)
				{
					if (base->isMotionPlaying(UI::EMotionEvent::Out))
					{
						isPlaying = true;
						break;
					}
				}

				if (isPlaying == false)
				{
					m_outMotion = false;
					if (OnMotionOutFinish != nullptr)
						OnMotionOutFinish();
				}
			}

			for (CUIBase* base : m_arrayUIObjects)
			{
				if (base->isEnable() && base->isVisible())
					base->update();
			}
		}

		CCanvas* CUIContainer::getCanvas()
		{
			if (!m_canvas)
				m_canvas = m_gameObject->getComponent<CCanvas>();
			return m_canvas;
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

		void CUIContainer::removeChildsByGUI(CGUIElement* element)
		{
			std::stack<CGUIElement*> stack;
			stack.push(element);

			while (stack.size() > 0)
			{
				CGUIElement* gui = stack.top();
				stack.pop();

				CUIBase* base = getChildByGUI(gui);
				if (base)
					base->remove();

				std::vector<CGUIElement*>& childs = element->getChilds();
				for (CGUIElement* e : childs)
					stack.push(e);
			}
		}

		CUIBase* CUIContainer::getChildByGUI(CGUIElement* element)
		{
			std::vector<CUIBase*>::iterator i = m_arrayUIObjects.begin(), end = m_arrayUIObjects.end();
			while (i != end)
			{
				if ((*i)->getElement() == element)
					return *i;
				++i;
			}
			return NULL;
		}

		CUIBase* CUIContainer::OnProcessEvent(const SEvent& event)
		{
			if (!m_enable)
				return NULL;

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

					if (!base->getElement()->isVisible())
						continue;

					if (!base->isEnable() || !base->isVisible())
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

					base->onPointerMove(mouseX, mouseY);
				}

				if (m_raycastUIObjects.size() == 0)
				{
					if (m_hover)
					{
						m_hover->onPointerOut(mouseX, mouseY);
						if (m_hover->isPointerDown())
							m_hover->onPointerUp(mouseX, mouseY);

						m_hover = NULL;
					}
					return NULL;
				}

				std::sort(m_raycastUIObjects.begin(), m_raycastUIObjects.end(), [](CUIBase*& a, CUIBase*& b)
					{
						return a->getElement()->getDepth() > b->getElement()->getDepth();
					});

				if (m_hover != m_raycastUIObjects[0])
				{
					if (m_hover)
					{
						m_hover->onPointerOut(mouseX, mouseY);
						if (m_hover->isPointerDown())
							m_hover->onPointerUp(mouseX, mouseY);
					}

					m_hover = m_raycastUIObjects[0];
					m_hover->onPointerHover(mouseX, mouseY);

					// if drag over
					if (m_pointerDown)
						m_hover->onPointerDown(mouseX, mouseY);
				}

				if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
				{
					m_pointerDown = true;
					if (m_hover)
						m_hover->onPointerDown(mouseX, mouseY);
				}
				else if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
				{
					m_pointerDown = false;
					if (m_hover)
					{
						m_hover->onPointerUp(mouseX, mouseY);
						m_hover->onPressed();
					}
				}

				// disable event
				return m_hover;
			}

			return NULL;
		}

		CUIBase* CUIContainer::OnProcessEvent(const SEvent& event, CUIBase* capture)
		{
			if (!m_enable)
				return NULL;

			if (m_hover != capture)
				return NULL;

			if (event.EventType == EET_MOUSE_INPUT_EVENT)
			{
				f32 mouseX = (f32)event.MouseInput.X;
				f32 mouseY = (f32)event.MouseInput.Y;

				if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
				{
					m_pointerDown = false;
					capture->onPointerUp(mouseX, mouseY);
					capture->onPressed();
				}
				else if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
				{
					m_pointerDown = true;
					capture->onPointerDown(mouseX, mouseY);
				}
				else
				{
					capture->onPointerMove(mouseX, mouseY);
				}
			}

			return capture;
		}

		void CUIContainer::onPointerOut(float x, float y)
		{
			if (m_hover)
			{
				m_hover->onPointerOut(x, y);
				m_hover = NULL;
			}
			m_pointerDown = false;
		}

		void CUIContainer::startInMotion()
		{
			m_inMotion = true;
			for (CUIBase* base : m_arrayUIObjects)
				base->startMotion(UI::EMotionEvent::In);
		}

		void CUIContainer::startOutMotion()
		{
			m_enable = false;
			m_outMotion = true;
			for (CUIBase* base : m_arrayUIObjects)
				base->startMotion(UI::EMotionEvent::Out);
		}
	}
}