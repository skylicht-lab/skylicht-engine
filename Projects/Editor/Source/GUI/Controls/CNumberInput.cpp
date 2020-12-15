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
#include "CNumberInput.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CNumberInput::CNumberInput(CBase* base) :
				CTextBox(base),
				m_mouseDownX(0.0f),
				m_mouseDownY(0.0f),
				m_focusTextbox(false)
			{
				m_textContainer->setTextAlignment(ETextAlign::TextCenter);
				setCursor(ECursorType::SizeWE);
			}

			CNumberInput::~CNumberInput()
			{

			}

			void CNumberInput::renderUnder()
			{
				CTextBox::renderUnder();

				if (m_drawTextbox && isHovered() && !m_focusTextbox)
				{
					CTheme* theme = CTheme::getTheme();
					const SRect& r = getRenderBounds();

					theme->drawTexboxButton(r, CThemeConfig::TextBoxButtonColor, CThemeConfig::White, true, true);
				}
			}

			void CNumberInput::onKeyboardFocus()
			{
				// disable default textbox focus
			}

			void CNumberInput::onLostKeyboardFocus()
			{
				CTextBox::onLostKeyboardFocus();

				m_focusTextbox = false;
				setCursor(ECursorType::SizeWE);
				setCaretToEnd();
			}

			void CNumberInput::onMouseClickLeft(float x, float y, bool down)
			{
				if (m_focusTextbox == true)
				{
					// default textbox function
					CTextBox::onMouseClickLeft(x, y, down);
				}
				else
				{
					// check state drag to adjust number value
					if (down)
					{
						m_mouseDownX = x;
						m_mouseDownY = y;
					}
					else
					{
						if (fabsf(x - m_mouseDownX) <= 1 && fabsf(y - m_mouseDownY) <= 1.0f)
						{
							CTextBox::onKeyboardFocus();
							CTextBox::onMouseClickLeft(x, y, true);
							CTextBox::onMouseClickLeft(x, y, false);

							onSelectAll(this);

							m_focusTextbox = true;
							setCursor(ECursorType::Beam);
						}
					}
				}
			}

			bool CNumberInput::onChar(u32 c)
			{
				if ((c >= '0' && c <= '9') || c == '.' || c == '\b')
				{
					return CTextBox::onChar(c);
				}
				else if (c == '\r')
				{
					return true;
				}

				return false;
			}
		}
	}
}