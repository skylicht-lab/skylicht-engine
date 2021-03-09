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
#include "CSkylichtInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CSkylichtInput::CSkylichtInput() :
				m_mouseX(0.0f),
				m_mouseY(0.0f),
				m_cursorType(ECursorType::CursorCount)
			{
				CEventManager::getInstance()->registerProcessorEvent("EditorEventManager", this);
			}

			CSkylichtInput::~CSkylichtInput()
			{
				CEventManager::getInstance()->unRegisterProcessorEvent(this);
			}

			bool CSkylichtInput::OnProcessEvent(const SEvent& event)
			{

				switch (event.EventType)
				{
				case irr::EET_MOUSE_INPUT_EVENT:
					switch (event.MouseInput.Event)
					{
					case irr::EMIE_MOUSE_MOVED:
					{
						float x = (float)event.MouseInput.X;
						float y = (float)event.MouseInput.Y;
						float dx = x - m_mouseX;
						float dy = y - m_mouseY;
						m_mouseX = x;
						m_mouseY = y;
						inputMouseMoved(x, y, dx, dy);
						break;
					}

					case irr::EMIE_MOUSE_WHEEL:
						inputMouseWheel((int)event.MouseInput.Wheel);
						break;
					case irr::EMIE_LMOUSE_PRESSED_DOWN:
						inputMouseButton(0, true);
						break;
					case irr::EMIE_LMOUSE_LEFT_UP:
						inputMouseButton(0, false);
						break;
					case irr::EMIE_RMOUSE_PRESSED_DOWN:
						inputMouseButton(1, true);
						break;
					case irr::EMIE_RMOUSE_LEFT_UP:
						inputMouseButton(1, false);
						break;
					case irr::EMIE_MMOUSE_PRESSED_DOWN:
						inputMouseButton(2, true);
						break;
					case irr::EMIE_MMOUSE_LEFT_UP:
						inputMouseButton(2, false);
						break;
					default:
						break;
					}
					break;
				case irr::EET_KEY_INPUT_EVENT:
					switch (event.KeyInput.Key)
					{
					case irr::KEY_SHIFT:
					case irr::KEY_LSHIFT:
					case irr::KEY_RSHIFT:
						inputKeyEvent(EKey::KEY_SHIFT, event.KeyInput.PressedDown);
						break;
					case irr::KEY_CONTROL:
					case irr::KEY_LCONTROL:
					case irr::KEY_RCONTROL:
						inputKeyEvent(EKey::KEY_CONTROL, event.KeyInput.PressedDown);
						break;
					case irr::KEY_MENU:
					case irr::KEY_LMENU:
					case irr::KEY_RMENU:
						inputKeyEvent(EKey::KEY_MENU, event.KeyInput.PressedDown);
						break;
					default:
						if (event.KeyInput.Char != 0 && /*!isKeyDown(EKey::KEY_MENU) &&*/ event.KeyInput.PressedDown)
							inputCharacter(event.KeyInput.Char);
						else
							inputKeyEvent((EKey)event.KeyInput.Key, event.KeyInput.PressedDown);
						break;
					}
					break;
				default:
					break;
				}

				// Disable all event
				return false;
			}

			void CSkylichtInput::setCursor(ECursorType type)
			{
				if (m_cursorType == type)
					return;

				m_cursorType = type;

				gui::ICursorControl* cursorControl = getIrrlichtDevice()->getCursorControl();

				switch (type)
				{
				case Skylicht::Editor::GUI::Normal:
					cursorControl->setActiveIcon(gui::ECI_NORMAL);
					break;
				case Skylicht::Editor::GUI::Beam:
					cursorControl->setActiveIcon(gui::ECI_IBEAM);
					break;
				case Skylicht::Editor::GUI::SizeNS:
					cursorControl->setActiveIcon(gui::ECI_SIZENS);
					break;
				case Skylicht::Editor::GUI::SizeWE:
					cursorControl->setActiveIcon(gui::ECI_SIZEWE);
					break;
				case Skylicht::Editor::GUI::SizeNWSE:
					cursorControl->setActiveIcon(gui::ECI_SIZENWSE);
					break;
				case Skylicht::Editor::GUI::SizeNESW:
					cursorControl->setActiveIcon(gui::ECI_SIZENESW);
					break;
				case Skylicht::Editor::GUI::SizeAll:
					cursorControl->setActiveIcon(gui::ECI_SIZEALL);
					break;
				case Skylicht::Editor::GUI::No:
					cursorControl->setActiveIcon(gui::ECI_NO);
					break;
				case Skylicht::Editor::GUI::Wait:
					cursorControl->setActiveIcon(gui::ECI_WAIT);
					break;
				case Skylicht::Editor::GUI::Finger:
					cursorControl->setActiveIcon(gui::ECI_HAND);
					break;
				default:
					break;
				}
			}

			void CSkylichtInput::hideCursor(bool b)
			{
				gui::ICursorControl* cursorControl = getIrrlichtDevice()->getCursorControl();
				cursorControl->setVisible(!b);
			}

			void CSkylichtInput::setCursorPosition(float x, float y)
			{
				CInput::setCursorPosition(x, y);

				gui::ICursorControl* cursorControl = getIrrlichtDevice()->getCursorControl();
				cursorControl->setPosition((int)x, (int)y);
			}

			void CSkylichtInput::getCursorPosition(float& x, float& y)
			{
				gui::ICursorControl* cursorControl = getIrrlichtDevice()->getCursorControl();
				core::vector2di p = cursorControl->getPosition();
				x = (float)p.X;
				y = (float)p.Y;
			}
		}
	}
}