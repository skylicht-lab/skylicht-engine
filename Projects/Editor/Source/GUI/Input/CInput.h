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

#include "GUI/Controls/CBase.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CInput
			{
			public:
				struct SKeyData
				{
					SKeyData()
					{
						for (int i = 0; i < EKey::KEY_KEY_CODES_COUNT; i++)
						{
							KeyState[i] = false;
							NextRepeat[i] = 0;
						}

						Target = nullptr;
						LeftMouseDown = false;
						RightMouseDown = false;
					}

					bool KeyState[EKey::KEY_KEY_CODES_COUNT];
					float NextRepeat[EKey::KEY_KEY_CODES_COUNT];
					CBase* Target;
					bool LeftMouseDown;
					bool RightMouseDown;
				};

				SKeyData KeyData;

			protected:
				float m_keyRepeatDelay;

				float m_lastClickPositionX;
				float m_lastClickPositionY;

				float m_mousePositionX;
				float m_mousePositionY;

				float m_lastClickTime[3];
				u32 m_fastClickCount;

				CBase *m_capture;

			public:
				CInput();

				virtual ~CInput();

				static void setInput(CInput *input);

				static CInput* getInput();

				SPoint getMousePosition()
				{
					return SPoint(m_mousePositionX, m_mousePositionY);
				}

				inline bool isKeyDown(EKey key)
				{
					return KeyData.KeyState[key];
				}

				inline bool isLeftMouseDown()
				{
					return KeyData.LeftMouseDown;
				}

				inline bool isRightMouseDown()
				{
					return KeyData.RightMouseDown;
				}

				inline bool IsShiftDown()
				{
					return isKeyDown(EKey::KEY_SHIFT);
				}

				inline bool IsControlDown()
				{
					return isKeyDown(EKey::KEY_CONTROL);
				}

				virtual void update();

				bool inputMouseMoved(float x, float y, float deltaX, float deltaY);

				bool inputMouseButton(int iButton, bool down);

				bool inputMouseWheel(int wheel);

				bool inputKeyEvent(EKey key, bool down);

				bool inputCharacter(u32 character);

				bool keyboardFocus(CBase *hoverControl);

				virtual void setCursor(ECursorType type) {}

				inline void setCapture(CBase *p)
				{
					m_capture = p;
				}

				inline CBase* getCapture()
				{
					return m_capture;
				}
			};
		}
	}
}