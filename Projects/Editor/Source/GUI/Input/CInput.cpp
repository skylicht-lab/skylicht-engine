/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#pragma once

#include "pch.h"
#include "CInput.h"
#include "GUI/Controls/CCanvas.h"
#include "GUI/CGUIContext.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CInput *g_baseInput = NULL;

			CInput::CInput() :
				m_mousePositionX(0.0f),
				m_mousePositionY(0.0f),
				m_lastClickPositionX(0.0f),
				m_lastClickPositionY(0.0f),
				m_capture(NULL)
			{
				for (int i = 0; i < 3; i++)
					m_lastClickTime[i] = 0.0f;
			}

			CInput::~CInput()
			{

			}

			void CInput::setInput(CInput *input)
			{
				g_baseInput = input;
			}

			CInput* CInput::getInput()
			{
				return g_baseInput;
			}

			bool CInput::inputMouseMoved(float x, float y, float deltaX, float deltaY)
			{
				m_mousePositionX = x;
				m_mousePositionY = y;

				if (m_capture != NULL)
				{
					m_capture->onMouseMoved(x, y, deltaX, deltaY);

					if (m_capture != NULL)
						m_capture->updateCursor();
					return true;
				}

				CBase *hover = CGUIContext::getRoot()->getControlAt(x, y);

				if (CGUIContext::HoveredControl != hover)
				{
					if (CGUIContext::HoveredControl != NULL)
					{
						CGUIContext::HoveredControl->onMouseLeave();
						CGUIContext::HoveredControl = NULL;
					}

					CGUIContext::HoveredControl = hover;

					if (CGUIContext::HoveredControl != NULL)
						CGUIContext::HoveredControl->onMouseEnter();
				}

				if (CGUIContext::MouseFocus)
				{
					if (CGUIContext::HoveredControl)
					{
						CGUIContext::HoveredControl = NULL;
					}

					CGUIContext::HoveredControl = CGUIContext::MouseFocus;
				}

				if (!CGUIContext::HoveredControl)
					return false;

				if (CGUIContext::HoveredControl == CGUIContext::getRoot())
				{
					CGUIContext::HoveredControl->updateCursor();
					return false;
				}

				CGUIContext::HoveredControl->onMouseMoved(x, y, deltaX, deltaY);
				CGUIContext::HoveredControl->updateCursor();
				return true;
			}

			bool CInput::inputMouseButton(int iMouseButton, bool down)
			{
				if (down && (!CGUIContext::HoveredControl || !CGUIContext::HoveredControl->isMenuComponent()))
					CGUIContext::getRoot()->closeMenu();

				bool isDoubleClick = false;

				if (down
					&& m_lastClickPositionX == m_mousePositionX
					&& m_lastClickPositionY == m_mousePositionY
					&& (CGUIContext::getTime() - m_lastClickTime[iMouseButton]) < 500.0f)
				{
					isDoubleClick = true;
				}

				if (down && !isDoubleClick)
				{
					m_lastClickTime[iMouseButton] = CGUIContext::getTime();
					m_lastClickPositionX = m_mousePositionX;
					m_lastClickPositionY = m_mousePositionY;
				}

				if (m_capture != NULL)
				{
					if (isDoubleClick)
						m_capture->onMouseDoubleClickLeft(m_mousePositionX, m_mousePositionY);
					else
						m_capture->onMouseClickLeft(m_mousePositionX, m_mousePositionY, down);
					return true;
				}

				if (!CGUIContext::HoveredControl)
					return false;

				if (!CGUIContext::HoveredControl->isVisible())
					return false;

				CGUIContext::HoveredControl->updateCursor();

				if (CGUIContext::HoveredControl == CGUIContext::getRoot())
					return false;

				// Do keyboard focus

				// This tells the child it has been touched, which
				// in turn tells its parents, who tell their parents.
				// This is basically so that Windows can pop themselves
				// to the top when one of their children has been clicked.
				if (down)
					CGUIContext::HoveredControl->touch();

				switch (iMouseButton)
				{
				case 0:
					if (isDoubleClick)
						CGUIContext::HoveredControl->onMouseDoubleClickLeft(m_mousePositionX, m_mousePositionY);
					else
						CGUIContext::HoveredControl->onMouseClickLeft(m_mousePositionX, m_mousePositionY, down);

					return true;

				case 1:
					if (isDoubleClick)
						CGUIContext::HoveredControl->onMouseDoubleClickRight(m_mousePositionX, m_mousePositionY);
					else
						CGUIContext::HoveredControl->onMouseClickRight(m_mousePositionX, m_mousePositionY, down);

					return true;
				case 2:
					if (isDoubleClick)
						CGUIContext::HoveredControl->onMouseDoubleClickMiddle(m_mousePositionX, m_mousePositionY);
					else
						CGUIContext::HoveredControl->onMouseClickMiddle(m_mousePositionX, m_mousePositionY, down);

					return true;
				}

				return false;
			}

			bool CInput::inputMouseWheel(int wheel)
			{
				if (!CGUIContext::HoveredControl)
					return false;

				if (!CGUIContext::HoveredControl->isVisible())
					return false;

				CGUIContext::HoveredControl->updateCursor();

				if (CGUIContext::HoveredControl == CGUIContext::getRoot())
					return false;

				CGUIContext::HoveredControl->onMouseWheeled(wheel);
				return true;
			}

			bool CInput::inputModifierKey(EKey key, bool down)
			{
				return false;
			}

			bool CInput::inputCharacter(u32 character)
			{
				return false;
			}
		}
	}
}