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
#include "CUIEventManager.h"
#include "CUIContainer.h"

namespace Skylicht
{
	namespace UI
	{
		IMPLEMENT_SINGLETON(CUIEventManager);

		CUIEventManager::CUIEventManager() :
			m_pointerId(-1),
			m_capture(NULL),
			m_focus(NULL),
			m_pointerX(0),
			m_pointerY(0)
		{
			CEventManager::getInstance()->registerProcessorEvent("CUIEventManager", this);
		}

		CUIEventManager::~CUIEventManager()
		{
			CEventManager::getInstance()->unRegisterProcessorEvent(this);
		}

		bool CUIEventManager::OnProcessEvent(const SEvent& event)
		{
			if (event.EventType == EET_MOUSE_INPUT_EVENT)
			{
				f32 mouseX = (f32)event.MouseInput.X;
				f32 mouseY = (f32)event.MouseInput.Y;
				int mouseId = event.MouseInput.ID;

				if (m_pointerId == -1)
					m_pointerId = mouseId;

				if (m_pointerId == mouseId)
				{
					m_pointerX = event.MouseInput.X;
					m_pointerY = event.MouseInput.Y;
				}

				std::vector<CUIContainer*> list = m_containers;

				std::sort(list.begin(), list.end(), [](CUIContainer*& a, CUIContainer*& b)
					{
						return a->getCanvas()->getSortDepth() > b->getCanvas()->getSortDepth();
					});

				CUIBase* base = NULL;

				if (m_capture)
				{
					base = m_capture;

					CUIContainer* captureContainer = m_capture->getContainer();
					captureContainer->OnProcessEvent(event, m_capture);

					for (CUIContainer* ui : list)
					{
						if (captureContainer != ui)
							ui->onPointerOut(mouseId, mouseX, mouseY);
					}
				}
				else
				{
					for (CUIContainer* ui : list)
					{
						if (!base)
						{
							base = ui->OnProcessEvent(event);
						}
						else
						{
							ui->onPointerOut(mouseId, mouseX, mouseY);
						}
					}
				}

				if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
				{
					setFocus(base);

					if (m_pointerId == mouseId)
						m_pointerId = -1;
				}

				if (!base)
					return true;
				else
					return base->isContinueGameEvent();
			}
			else if (event.EventType == EET_KEY_INPUT_EVENT)
			{
				if (m_focus)
				{
					m_focus->onKeyEvent(event);
					return m_focus->isContinueGameEvent();
				}
			}

			return true;
		}

		void CUIEventManager::registerUIContainer(CUIContainer* container)
		{
			std::vector<CUIContainer*>::iterator i = m_containers.begin(), end = m_containers.end();
			while (i != end)
			{
				if (*i == container)
					return;
				++i;
			}

			m_containers.push_back(container);
		}

		void CUIEventManager::unregisterUIContainer(CUIContainer* container)
		{
			std::vector<CUIContainer*>::iterator i = m_containers.begin(), end = m_containers.end();
			while (i != end)
			{
				if (*i == container)
				{
					m_containers.erase(i);
					return;
				}
				++i;
			}
		}

		void CUIEventManager::setFocus(CUIBase* focus)
		{
			if (m_focus == focus)
				return;

			if (m_focus && m_focus != focus)
			{
				m_focus->onLostFocus();
				m_focus->startMotion(UI::EMotionEvent::UnFocus);
			}

			m_focus = focus;
			if (m_focus)
			{
				m_focus->onFocus();
				m_focus->startMotion(UI::EMotionEvent::Focus);
			}
		}
	}
}