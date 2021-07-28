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

#include "CTextBox.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CNumberInput : public CTextBox
			{
			protected:
				float m_mouseDownX;
				float m_mouseDownY;

				float m_cursorX;
				float m_cursorY;

				bool m_focusTextbox;

				float m_stepValue;
				float m_value;

				bool m_mousePress;
				bool m_drag;

				ENumberInputType m_numberType;

			public:
				CNumberInput(CBase* base);

				virtual ~CNumberInput();

				virtual void think();

				virtual void renderUnder();

				virtual void onKeyboardFocus();

				virtual void onLostKeyboardFocus();

				virtual void onMouseClickLeft(float x, float y, bool down);

				virtual void onPaste(CBase* base);

				virtual bool onChar(u32 c);

				inline float getValue()
				{
					return m_value;
				}

				inline int getValueInt()
				{
					return (int)m_value;
				}

				void setValue(float value, bool invokeEvent);

				inline void setStep(float step)
				{
					m_stepValue = step;
				}

				inline float getStep()
				{
					return m_stepValue;
				}

				inline void setNumberType(ENumberInputType type)
				{
					m_numberType = type;
				}

			protected:

				void applyTextValue();

			};
		}
	}
}