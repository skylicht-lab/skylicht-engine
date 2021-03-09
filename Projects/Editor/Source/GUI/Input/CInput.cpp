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

#include "pch.h"
#include "CInput.h"

#include "GUI/Controls/CDialogWindow.h"
#include "GUI/Controls/CCanvas.h"
#include "GUI/CGUIContext.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CInput* g_baseInput = NULL;

			CInput::CInput() :
				m_mousePositionX(0.0f),
				m_mousePositionY(0.0f),
				m_lastClickPositionX(0.0f),
				m_lastClickPositionY(0.0f),
				m_capture(NULL),
				m_fastClickCount(0),
				m_keyRepeatDelay(50),
				m_firstKeyRepeatDelay(300)
			{
				for (int i = 0; i < 3; i++)
					m_lastClickTime[i] = 0.0f;
			}

			CInput::~CInput()
			{

			}

			void CInput::setInput(CInput* input)
			{
				g_baseInput = input;
			}

			CInput* CInput::getInput()
			{
				return g_baseInput;
			}

			void CInput::update()
			{
				if (CGUIContext::MouseFocus && CGUIContext::MouseFocus->isHidden())
					CGUIContext::MouseFocus = NULL;

				if (CGUIContext::KeyboardFocus
					&& (CGUIContext::KeyboardFocus->isHidden() || !CGUIContext::KeyboardFocus->getKeyboardInputEnabled()))
				{
					CGUIContext::KeyboardFocus = nullptr;
				}

				if (!CGUIContext::KeyboardFocus)
					return;

				float time = CGUIContext::getTime();

				// Simulate hold key-repeats
				for (int i = 0; i < EKey::KEY_KEY_CODES_COUNT; i++)
				{
					if (KeyData.KeyState[i] && KeyData.Target != CGUIContext::KeyboardFocus)
					{
						KeyData.KeyState[i] = false;
						continue;
					}

					if (KeyData.KeyState[i] && time > KeyData.NextRepeat[i])
					{
						KeyData.NextRepeat[i] = time + m_keyRepeatDelay;
						if (CGUIContext::KeyboardFocus)
							CGUIContext::KeyboardFocus->onKeyPress((EKey)i, true);
					}
				}
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

				CBase* hover = CGUIContext::getRoot()->getControlAt(x, y);

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

			bool CInput::inputMouseButton(int mouseButton, bool down)
			{
				if (down && (!CGUIContext::HoveredControl || !CGUIContext::HoveredControl->isMenuComponent()))
					CGUIContext::getRoot()->closeMenu();

				bool isDoubleClick = false;
				bool isTripleClick = false;

				if (down
					&& m_lastClickPositionX == m_mousePositionX
					&& m_lastClickPositionY == m_mousePositionY
					&& (CGUIContext::getTime() - m_lastClickTime[mouseButton]) < 400.0f)
				{
					m_fastClickCount++;

					if (m_fastClickCount >= 2 && m_fastClickCount % 2 == 0)
						isDoubleClick = true;

					if (m_fastClickCount >= 3 && m_fastClickCount % 3 == 0)
						isTripleClick = true;
				}
				else if (down)
				{
					m_fastClickCount = 1;
				}

				if (mouseButton == 0)
					KeyData.LeftMouseDown = down;
				else if (mouseButton == 1)
					KeyData.RightMouseDown = down;

				m_lastClickTime[mouseButton] = CGUIContext::getTime();

				if (down && !isDoubleClick)
				{
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

				// Check is topmost dialog enable
				CDialogWindow* dialog = CGUIContext::getRoot()->getTopmostDialog();
				if (dialog != NULL)
				{
					if (!dialog->isChild(CGUIContext::HoveredControl, true))
					{
						dialog->blinkCaption();
						return false;
					}
				}

				if (CGUIContext::HoveredControl == CGUIContext::getRoot())
					return false;

				// Do keyboard focus
				if (!keyboardFocus(CGUIContext::HoveredControl))
				{
					if (CGUIContext::KeyboardFocus)
						CGUIContext::KeyboardFocus->unfocus();
				}

				// This tells the child it has been touched, which
				// in turn tells its parents, who tell their parents.
				// This is basically so that Windows can pop themselves
				// to the top when one of their children has been clicked.
				if (down)
					CGUIContext::HoveredControl->touch();

				switch (mouseButton)
				{
				case 0:
					if (isTripleClick)
						CGUIContext::HoveredControl->onMouseTripleClickLeft(m_mousePositionX, m_mousePositionY);
					else if (isDoubleClick)
						CGUIContext::HoveredControl->onMouseDoubleClickLeft(m_mousePositionX, m_mousePositionY);
					else
						CGUIContext::HoveredControl->onMouseClickLeft(m_mousePositionX, m_mousePositionY, down);

					return true;

				case 1:
					if (isTripleClick)
						CGUIContext::HoveredControl->onMouseTripleClickRight(m_mousePositionX, m_mousePositionY);
					else if (isDoubleClick)
						CGUIContext::HoveredControl->onMouseDoubleClickRight(m_mousePositionX, m_mousePositionY);
					else
						CGUIContext::HoveredControl->onMouseClickRight(m_mousePositionX, m_mousePositionY, down);

					return true;
				case 2:
					if (isTripleClick)
						CGUIContext::HoveredControl->onMouseTripleClickMiddle(m_mousePositionX, m_mousePositionY);
					if (isDoubleClick)
						CGUIContext::HoveredControl->onMouseDoubleClickMiddle(m_mousePositionX, m_mousePositionY);
					else
						CGUIContext::HoveredControl->onMouseClickMiddle(m_mousePositionX, m_mousePositionY, down);

					return true;
				}

				return false;
			}

			bool CInput::keyboardFocus(CBase* hoverControl)
			{
				if (hoverControl == NULL)
					return false;

				if (hoverControl->getKeyboardInputEnabled())
				{
					// Make sure none of our children have keyboard focus first
					// todo recursive
					for (auto&& child : hoverControl->Children)
					{
						if (child == CGUIContext::KeyboardFocus)
							return false;
					}

					hoverControl->focus();
					return true;
				}

				return keyboardFocus(hoverControl->getParent());
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

				// Check is topmost dialog enable
				CDialogWindow* dialog = CGUIContext::getRoot()->getTopmostDialog();
				if (dialog != NULL)
				{
					if (!dialog->isChild(CGUIContext::HoveredControl, true))
					{
						return false;
					}
				}

				CGUIContext::HoveredControl->onMouseWheeled(wheel);
				return true;
			}

			bool CInput::inputKeyEvent(EKey key, bool down)
			{
				CBase* target = CGUIContext::KeyboardFocus;
				if (target && target->isHidden())
					target = NULL;

				// Check is topmost dialog enable
				CDialogWindow* dialog = CGUIContext::getRoot()->getTopmostDialog();
				if (dialog != NULL)
				{
					if (!dialog->isChild(target, true))
					{
						return false;
					}
				}

				if (down)
				{
					if (!KeyData.KeyState[key])
					{
						KeyData.KeyState[key] = true;
						KeyData.NextRepeat[key] = CGUIContext::getTime() + m_firstKeyRepeatDelay;
						KeyData.Target = target;

						if (target)
							return target->onKeyPress(key);
					}
				}
				else
				{
					if (KeyData.KeyState[key])
					{
						KeyData.KeyState[key] = false;

						//! @bug This causes shift left arrow in textboxes
						//! to not work. What is disabling it here breaking?
						//! `KeyData.Target = nullptr;`

						if (target)
							return target->onKeyRelease(key);
					}
				}

				return false;
			}

			bool CInput::inputCharacter(u32 character)
			{
				// Handle Accelerators
				if (handleAccelerator(character))
					return true;

				// Handle characters
				if (!CGUIContext::KeyboardFocus)
					return false;

				if (!CGUIContext::KeyboardFocus->isVisible())
					return false;

				if (CInput::IsControlDown())
					return false;

				// Check is topmost dialog enable
				CDialogWindow* dialog = CGUIContext::getRoot()->getTopmostDialog();
				if (dialog != NULL)
				{
					if (!dialog->isChild(CGUIContext::KeyboardFocus, true))
					{
						return false;
					}
				}

				return CGUIContext::KeyboardFocus->onChar(character);
			}

			bool CInput::handleAccelerator(u32 character)
			{
				// Build the accelerator search string
				std::string accelString;

				if (isAltDown())
					accelString += "ALT + ";

				if (IsControlDown())
					accelString += "CTRL + ";

				if (IsShiftDown())
					accelString += "SHIFT + ";

				accelString += toupper(character);

				CDialogWindow* dialog = CGUIContext::getRoot()->getTopmostDialog();
				if (dialog != NULL)
				{
					if (CGUIContext::KeyboardFocus && dialog->isChild(CGUIContext::KeyboardFocus, true) && CGUIContext::KeyboardFocus->handleAccelerator(accelString))
						return true;

					if (CGUIContext::MouseFocus && dialog->isChild(CGUIContext::MouseFocus, true) && CGUIContext::MouseFocus->handleAccelerator(accelString))
						return true;

					return dialog->handleAccelerator(accelString);
				}
				else
				{
					if (CGUIContext::KeyboardFocus && CGUIContext::KeyboardFocus->handleAccelerator(accelString))
						return true;

					if (CGUIContext::MouseFocus && CGUIContext::MouseFocus->handleAccelerator(accelString))
						return true;

					if (CGUIContext::getRoot()->handleAccelerator(accelString))
						return true;
				}

				return false;
			}
		}
	}
}