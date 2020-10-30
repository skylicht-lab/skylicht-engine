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
			protected:
				float m_lastClickPositionX;
				float m_lastClickPositionY;

				float m_mousePositionX;
				float m_mousePositionY;

				float m_lastClickTime[3];

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

				bool inputMouseMoved(float x, float y, float deltaX, float deltaY);

				bool inputMouseButton(int iButton, bool down);

				bool inputMouseWheel(int wheel);

				bool inputModifierKey(EKey key, bool down);

				bool inputCharacter(u32 character);

				virtual void setCursor(ECursorType type) {}

				inline void setCapture(CBase *p)
				{
					m_capture = p;
				}
			};
		}
	}
}