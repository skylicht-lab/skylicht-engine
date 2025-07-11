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
#include "GUI/Controls/CMenu.h"
#include "GUI/Controls/CCanvas.h"
#include "GUI/GUIContext.h"

#include "GUI/Utils/CDragAndDrop.h"

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
				if (Context::MouseFocus && Context::MouseFocus->isHidden())
					Context::MouseFocus = NULL;

				if (Context::KeyboardFocus
					&& (Context::KeyboardFocus->isHidden() || !Context::KeyboardFocus->getKeyboardInputEnabled()))
				{
					Context::KeyboardFocus = nullptr;
				}

				if (!Context::KeyboardFocus)
					return;

				float time = Context::getTime();

				// Simulate hold key-repeats
				for (int i = 0; i < EKey::KEY_KEY_CODES_COUNT; i++)
				{
					if (i != EKey::KEY_CONTROL && i != EKey::KEY_SHIFT && i != EKey::KEY_MENU)
					{
						if (KeyData.KeyState[i] && KeyData.Target != Context::KeyboardFocus)
						{
							KeyData.KeyState[i] = false;
							continue;
						}
					}
					
					if (KeyData.KeyState[i] && time > KeyData.NextRepeat[i])
					{
						KeyData.NextRepeat[i] = time + m_keyRepeatDelay;
						if (Context::KeyboardFocus)
							Context::KeyboardFocus->onKeyPress((EKey)i, true);
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

				CBase* hover = Context::getRoot()->getControlAt(x, y);

				if (Context::HoveredControl != hover)
				{
					if (Context::HoveredControl != NULL)
					{
						Context::HoveredControl->onMouseLeave();
						Context::HoveredControl = NULL;
					}

					Context::HoveredControl = hover;

					if (Context::HoveredControl != NULL)
						Context::HoveredControl->onMouseEnter();
				}

				if (Context::MouseFocus)
				{
					if (Context::HoveredControl)
						Context::HoveredControl = NULL;

					Context::HoveredControl = Context::MouseFocus;
				}

				if (!Context::HoveredControl)
					return false;

				if (Context::HoveredControl == Context::getRoot())
				{
					Context::HoveredControl->updateCursor();
					return false;
				}

				Context::HoveredControl->onMouseMoved(x, y, deltaX, deltaY);

				if (CDragAndDrop::onMouseMoved(Context::HoveredControl, x, y) == false)
					Context::HoveredControl->updateCursor();

				return true;
			}

			bool CInput::inputMouseButton(int mouseButton, bool down)
			{
				if (down && (!Context::HoveredControl || !Context::HoveredControl->isMenuComponent()))
					Context::getRoot()->closeMenu();

				bool isDoubleClick = false;
				bool isTripleClick = false;

				if (down
					&& m_lastClickPositionX == m_mousePositionX
					&& m_lastClickPositionY == m_mousePositionY
					&& (Context::getTime() - m_lastClickTime[mouseButton]) < 400.0f)
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

				m_lastClickTime[mouseButton] = Context::getTime();

				if (down && !isDoubleClick)
				{
					m_lastClickPositionX = m_mousePositionX;
					m_lastClickPositionY = m_mousePositionY;
				}

				if (m_capture != NULL)
				{
					if (mouseButton == 0)
					{
						if (CDragAndDrop::onMouseButton(m_capture, m_mousePositionX, m_mousePositionY, down))
							return true;

						if (isTripleClick)
							m_capture->onMouseTripleClickLeft(m_mousePositionX, m_mousePositionY);
						else if (isDoubleClick)
							m_capture->onMouseDoubleClickLeft(m_mousePositionX, m_mousePositionY);
						else
							m_capture->onMouseClickLeft(m_mousePositionX, m_mousePositionY, down);
					}
					else if (mouseButton == 1)
					{
						if (isTripleClick)
							m_capture->onMouseTripleClickRight(m_mousePositionX, m_mousePositionY);
						else if (isDoubleClick)
							m_capture->onMouseDoubleClickRight(m_mousePositionX, m_mousePositionY);
						else
							m_capture->onMouseClickRight(m_mousePositionX, m_mousePositionY, down);
					}
					else
					{
						if (isTripleClick)
							m_capture->onMouseTripleClickMiddle(m_mousePositionX, m_mousePositionY);
						else if (isDoubleClick)
							m_capture->onMouseDoubleClickMiddle(m_mousePositionX, m_mousePositionY);
						else
							m_capture->onMouseClickMiddle(m_mousePositionX, m_mousePositionY, down);
					}

					return true;
				}

				if (!Context::HoveredControl)
					return false;

				if (!Context::HoveredControl->isVisible())
					return false;

				Context::HoveredControl->updateCursor();

				// Check is topmost dialog enable
				CDialogWindow* dialog = Context::getRoot()->getTopmostDialog();
				if (dialog != NULL)
				{
					if (!dialog->isChild(Context::HoveredControl, true) && !isControlInTopmostMenu(Context::HoveredControl))
					{
						bool dragActivate = CDragAndDrop::getPressedControl() != NULL;

						// todo
						// fix sometime double click on item and open dialog
						// the drag/drop still activated
						CDragAndDrop::onMouseButton(Context::HoveredControl, m_mousePositionX, m_mousePositionY, down);

						if (!dragActivate)
							dialog->blinkCaption();

						return false;
					}
				}

				if (Context::HoveredControl == Context::getRoot())
					return false;

				// Do keyboard focus
				if (!keyboardFocus(Context::HoveredControl))
				{
					if (Context::KeyboardFocus)
						Context::KeyboardFocus->unfocus();
				}

				// This tells the child it has been touched, which
				// in turn tells its parents, who tell their parents.
				// This is basically so that Windows can pop themselves
				// to the top when one of their children has been clicked.
				if (down)
					Context::HoveredControl->touch();

				switch (mouseButton)
				{
				case 0:
					if (CDragAndDrop::onMouseButton(Context::HoveredControl, m_mousePositionX, m_mousePositionY, down))
						return true;

					if (isTripleClick)
						Context::HoveredControl->onMouseTripleClickLeft(m_mousePositionX, m_mousePositionY);
					else if (isDoubleClick)
						Context::HoveredControl->onMouseDoubleClickLeft(m_mousePositionX, m_mousePositionY);
					else
						Context::HoveredControl->onMouseClickLeft(m_mousePositionX, m_mousePositionY, down);

					return true;

				case 1:
					if (isTripleClick)
						Context::HoveredControl->onMouseTripleClickRight(m_mousePositionX, m_mousePositionY);
					else if (isDoubleClick)
						Context::HoveredControl->onMouseDoubleClickRight(m_mousePositionX, m_mousePositionY);
					else
						Context::HoveredControl->onMouseClickRight(m_mousePositionX, m_mousePositionY, down);

					return true;
				case 2:
					if (isTripleClick)
						Context::HoveredControl->onMouseTripleClickMiddle(m_mousePositionX, m_mousePositionY);
					if (isDoubleClick)
						Context::HoveredControl->onMouseDoubleClickMiddle(m_mousePositionX, m_mousePositionY);
					else
						Context::HoveredControl->onMouseClickMiddle(m_mousePositionX, m_mousePositionY, down);

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
						if (child == Context::KeyboardFocus)
							return false;
					}

					hoverControl->focus();
					return true;
				}

				return keyboardFocus(hoverControl->getParent());
			}

			bool CInput::inputMouseWheel(int wheel)
			{
				if (!Context::HoveredControl)
					return false;

				if (!Context::HoveredControl->isVisible())
					return false;

				Context::HoveredControl->updateCursor();

				if (Context::HoveredControl == Context::getRoot())
					return false;

				// Check is topmost dialog enable
				CDialogWindow* dialog = Context::getRoot()->getTopmostDialog();
				if (dialog != NULL)
				{
					if (!dialog->isChild(Context::HoveredControl, true) && !isControlInTopmostMenu(Context::HoveredControl))
					{
						return false;
					}
				}

				Context::HoveredControl->onMouseWheeled(wheel);
				return true;
			}

			bool CInput::inputKeyEvent(EKey key, bool down)
			{
				CBase* target = Context::KeyboardFocus;
				if (target && target->isHidden())
					target = NULL;

				// Check is topmost dialog enable
				CDialogWindow* dialog = Context::getRoot()->getTopmostDialog();
				if (dialog != NULL)
				{
					if (!dialog->isChild(target, true) &&
						dialog != target &&
						!isControlInTopmostMenu(target))
					{
						return false;
					}
				}

				if (down)
				{
					if (!KeyData.KeyState[key])
					{
						printf("Key: %d down\n", key);
						KeyData.KeyState[key] = true;
						KeyData.NextRepeat[key] = Context::getTime() + m_firstKeyRepeatDelay;
						KeyData.Target = target;

						if (target)
							return target->onKeyPress(key);
					}
				}
				else
				{
					if (KeyData.KeyState[key])
					{
						printf("Key: %d up\n", key);
						KeyData.KeyState[key] = false;

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
				if (!Context::KeyboardFocus)
					return false;

				if (!Context::KeyboardFocus->isVisible())
					return false;

				if (CInput::IsControlDown())
					return false;

				// Check is topmost dialog enable
				CDialogWindow* dialog = Context::getRoot()->getTopmostDialog();
				if (dialog != NULL)
				{
					if (!dialog->isChild(Context::KeyboardFocus, true) &&
						dialog != Context::KeyboardFocus &&
						!isControlInTopmostMenu(Context::KeyboardFocus))
					{
						return false;
					}
				}

				return Context::KeyboardFocus->onChar(character);
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

				CDialogWindow* dialog = Context::getRoot()->getTopmostDialog();
				if (dialog != NULL)
				{
					if (Context::KeyboardFocus && dialog->isChild(Context::KeyboardFocus, true) && Context::KeyboardFocus->handleAccelerator(accelString))
						return true;

					if (Context::MouseFocus && dialog->isChild(Context::MouseFocus, true) && Context::MouseFocus->handleAccelerator(accelString))
						return true;

					return dialog->handleAccelerator(accelString);
				}
				else
				{
					if (Context::KeyboardFocus && Context::KeyboardFocus->handleAccelerator(accelString))
						return true;

					if (Context::MouseFocus && Context::MouseFocus->handleAccelerator(accelString))
						return true;

					if (Context::getRoot()->handleAccelerator(accelString))
						return true;
				}

				return false;
			}

			bool CInput::isControlInTopmostMenu(CBase* control)
			{
				CBase* p = control;
				while (p != NULL)
				{
					if (dynamic_cast<CMenu*>(p) != NULL)
					{
						return dynamic_cast<CMenu*>(p)->isOnTop();
					}
					p = p->getParent();
				}
				return false;
			}
		}
	}
}
