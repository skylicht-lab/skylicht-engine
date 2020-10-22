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
#include "CSkylichtInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CSkylichtInput::CSkylichtInput() :
				m_mouseX(0.0f),
				m_mouseY(0.0f)
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
						return inputMouseMoved(x, y, dx, dy);
					}

					case irr::EMIE_MOUSE_WHEEL:
						return inputMouseWheel((int)-event.MouseInput.Wheel);

					case irr::EMIE_LMOUSE_PRESSED_DOWN:
						return inputMouseButton(0, true);

					case irr::EMIE_LMOUSE_LEFT_UP:
						return inputMouseButton(0, false);

					case irr::EMIE_RMOUSE_PRESSED_DOWN:
						return inputMouseButton(1, true);

					case irr::EMIE_RMOUSE_LEFT_UP:
						return inputMouseButton(1, false);

					case irr::EMIE_MMOUSE_PRESSED_DOWN:
						return inputMouseButton(2, true);

					case irr::EMIE_MMOUSE_LEFT_UP:
						return inputMouseButton(2, false);

					default:
						break;
					}
					break;
				case irr::EET_KEY_INPUT_EVENT:
					switch (event.KeyInput.Key)
					{
					case irr::KEY_SHIFT:
						return inputModifierKey(EKey::KEY_SHIFT, event.KeyInput.PressedDown);

					case irr::KEY_RETURN:
						return inputModifierKey(EKey::KEY_RETURN, event.KeyInput.PressedDown);

					case irr::KEY_BACK:
						return inputModifierKey(EKey::KEY_BACK, event.KeyInput.PressedDown);

					case irr::KEY_DELETE:
						return inputModifierKey(EKey::KEY_DELETE, event.KeyInput.PressedDown);

					case irr::KEY_LEFT:
						return inputModifierKey(EKey::KEY_LEFT, event.KeyInput.PressedDown);

					case irr::KEY_RIGHT:
						return inputModifierKey(EKey::KEY_RIGHT, event.KeyInput.PressedDown);

					case irr::KEY_TAB:
						return inputModifierKey(EKey::KEY_TAB, event.KeyInput.PressedDown);

					case irr::KEY_SPACE:
						return !inputModifierKey(EKey::KEY_SPACE, event.KeyInput.PressedDown);

					case irr::KEY_HOME:
						return inputModifierKey(EKey::KEY_HOME, event.KeyInput.PressedDown);

					case irr::KEY_END:
						return inputModifierKey(EKey::KEY_END, event.KeyInput.PressedDown);

					case irr::KEY_CONTROL:
						return inputModifierKey(EKey::KEY_CONTROL, event.KeyInput.PressedDown);

					case irr::KEY_UP:
						return inputModifierKey(EKey::KEY_UP, event.KeyInput.PressedDown);

					case irr::KEY_DOWN:
						return inputModifierKey(EKey::KEY_DOWN, event.KeyInput.PressedDown);

					default:
						return inputCharacter(event.KeyInput.Char);
					}
					break;
				default:
					break;
				}

				return true;
			}
		}
	}
}