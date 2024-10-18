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

#include "GUI/Input/CInput.h"
#include "EventManager/CEventManager.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CSkylichtInput :
				public CInput,
				public IEventProcessor
			{
			protected:
				float m_mouseX;
				float m_mouseY;

				ECursorType m_cursorType;

			public:
				CSkylichtInput();

				virtual ~CSkylichtInput();

				virtual bool OnProcessEvent(const SEvent& event);

				virtual void setCursor(ECursorType type);

				virtual void hideCursor(bool b);

				virtual void setCursorPosition(float x, float y);

				virtual void getCursorPosition(float& x, float& y);
			};
		}
	}
}