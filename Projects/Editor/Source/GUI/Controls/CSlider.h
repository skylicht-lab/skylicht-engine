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
			class CSlider : public CTextBox
			{
			protected:
				float m_value;
				float m_min;
				float m_max;

				bool m_focusTextbox;

				ENumberInputType m_numberType;

				bool m_mousePress;
				bool m_drag;

				float m_mouseDownX;
				float m_mouseDownY;

			public:
				CSlider(CBase* base);

				virtual ~CSlider();

				virtual void think();

				virtual void renderUnder();

				virtual void onKeyboardFocus();

				virtual void onLostKeyboardFocus();

				virtual void onMouseClickLeft(float x, float y, bool down);

				virtual bool onChar(u32 c);

				void setValue(float value, float min, float max);

				inline float getValue()
				{
					return m_value;
				}

				inline int getValueInt()
				{
					return (int)m_value;
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