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
#include "GUI/Input/CInput.h"

#include "Utils/CStringImp.h"

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
				m_focusTextbox(false),
				m_value(0.0f),
				m_stepValue(1.0f),
				m_mousePress(false),
				m_drag(false),
				m_numberType(ENumberInputType::Float)
			{
				m_textContainer->setTextAlignment(ETextAlign::TextCenter);
				setCursor(ECursorType::SizeWE);
			}

			CNumberInput::~CNumberInput()
			{

			}

			void CNumberInput::think()
			{
				if (m_focusTextbox)
					CTextBox::think();
				else
				{
					if (m_mousePress)
					{
						SPoint mousePos = CInput::getInput()->getMousePosition();
						float dx = mousePos.X - m_mouseDownX;
						if (fabs(dx) > 0)
						{
							m_drag = true;

							float value = m_value;
							value = value + m_stepValue * dx;
							setValue(value, true);

							CInput::getInput()->setCursorPosition(m_cursorX, m_cursorY);
						}
					}
				}
			}

			void CNumberInput::renderUnder()
			{
				CTextBox::renderUnder();

				if (m_drawTextbox && isHovered() && !m_focusTextbox)
				{
					CTheme* theme = CTheme::getTheme();
					const SRect& r = getRenderBounds();

					theme->drawTextBoxButton(r, CThemeConfig::TextBoxButtonColor, CThemeConfig::White, true, true);
				}
			}

			void CNumberInput::onKeyboardFocus()
			{
				// disable default textbox focus
			}

			void CNumberInput::onTabableFocus()
			{
				CTextBox::onKeyboardFocus();

				onSelectAll(this);

				m_focusTextbox = true;
				setCursor(ECursorType::Beam);
			}

			void CNumberInput::onLostKeyboardFocus()
			{
				CTextBox::onLostKeyboardFocus();

				applyTextValue();

				CInput::getInput()->hideCursor(false);
				m_focusTextbox = false;
				setCursor(ECursorType::SizeWE);
				setCaretToEnd();
			}

			void CNumberInput::onMouseClickLeft(float x, float y, bool down)
			{
				m_mousePress = down;

				if (m_focusTextbox == true)
				{
					// default textbox function
					CTextBox::onMouseClickLeft(x, y, down);
				}
				else
				{
					CInput* input = CInput::getInput();

					// check state drag to adjust number value
					if (down)
					{
						m_drag = false;

						m_mouseDownX = x;
						m_mouseDownY = y;

						input->getCursorPosition(m_cursorX, m_cursorY);
						input->hideCursor(true);
						input->setCapture(this);
					}
					else
					{
						input->setCapture(NULL);
						input->hideCursor(false);

						if (!m_drag)
						{
							CTextBox::onKeyboardFocus();
							CTextBox::onMouseClickLeft(x, y, true);
							CTextBox::onMouseClickLeft(x, y, false);

							onSelectAll(this);

							m_focusTextbox = true;
							setCursor(ECursorType::Beam);
						}

						m_drag = false;
					}
				}
			}

			void CNumberInput::onPaste(CBase* base)
			{
				CTextBox::onPaste(base);
			}

			bool CNumberInput::onKeyPress(EKey key, bool press)
			{
				if (key == EKey::KEY_TAB)
				{
					return CBase::onKeyTab(press);
				}

				return CTextBox::onKeyPress(key, press);
			}

			bool CNumberInput::onChar(u32 c)
			{
				if ((c >= '0' && c <= '9') || c == '\b')
				{
					return CTextBox::onChar(c);
				}
				else if (c == '-')
				{
					const std::wstring& s = getString();
					if (s.empty() == true)
						return CTextBox::onChar(c);
					else if (m_textContainer->getCaretBegin() == 0 && s.find(L'-') == std::wstring::npos)
						return CTextBox::onChar(c);
					else if (m_textContainer->getCaretEnd() == 0 && m_textContainer->getCaretBegin() == (u32)s.size())
						return CTextBox::onChar(c);
					else
						return false;
				}
				else if (c == '.')
				{
					if (m_numberType == Integer)
					{
						// no dot
						return false;
					}
					else
					{
						// just 1 dot
						const std::wstring& s = getString();
						if (s.find(L'.') == std::wstring::npos)
							return CTextBox::onChar(c);
						else
							return false;
					}
				}
				else if (c == '\r')
				{
					onLostKeyboardFocus();
					return true;
				}

				return false;
			}

			void CNumberInput::applyTextValue()
			{
				const std::wstring s = getString();
				std::string utf8 = CStringImp::convertUnicodeToUTF8(s.c_str());

				float value = (float)atof(utf8.c_str());
				setValue(value, false);
			}

			void CNumberInput::onValidateValue()
			{
				const std::wstring s = getString();
				std::string utf8 = CStringImp::convertUnicodeToUTF8(s.c_str());

				m_value = (float)atof(utf8.c_str());
			}

			void CNumberInput::setValue(float value, bool invokeEvent)
			{
				m_value = value;

				wchar_t text[64];

				if (m_numberType == Float)
					swprintf(text, 64, L"%.03f", m_value);
				else
					swprintf(text, 64, L"%d", (int)m_value);

				setString(std::wstring(text));

				if (invokeEvent && OnTextChanged != nullptr)
					OnTextChanged(this);
			}
		}
	}
}