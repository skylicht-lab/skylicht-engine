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
#include "CSlider.h"

#include "GUI/Input/CInput.h"
#include "GUI/Renderer/CRenderer.h"
#include "GUI/Theme/ThemeConfig.h"

#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CSlider::CSlider(CBase* base) :
				CTextBox(base),
				m_value(0.0f),
				m_min(-1.0f),
				m_max(1.0f),
				m_numberType(ENumberInputType::Float),
				m_focusTextbox(false),
				m_mousePress(false),
				m_drag(false),
				m_mouseDownX(0.0f),
				m_mouseDownY(0.0f)
			{
				m_textContainer->setTextAlignment(ETextAlign::TextCenter);

				setCursor(ECursorType::SizeWE);
			}

			CSlider::~CSlider()
			{

			}

			void CSlider::think()
			{
				if (m_focusTextbox)
					CTextBox::think();
				else
				{
					if (m_mousePress && m_bounds.Width > 0 && m_max > m_min)
					{
						SPoint mousePos = CInput::getInput()->getMousePosition();
						if (m_drag || mousePos.X != m_mouseDownX)
						{
							m_drag = true;
							SPoint local = canvasPosToLocal(mousePos);

							float x = core::clamp(local.X, 0.0f, m_bounds.Width);
							float f = x / m_bounds.Width;

							float value = m_min + (m_max - m_min) * f;
							setValue(value, m_min, m_max, true);
						}

					}
				}
			}

			void CSlider::renderUnder()
			{
				if (m_drawTextbox)
				{
					CTheme* theme = CTheme::getTheme();
					CRenderer* renderer = CRenderer::getRenderer();

					theme->drawTextBox(getRenderBounds(), ThemeConfig::SliderColor);

					float d = m_max - m_min;
					if (!m_focusTextbox && d > 0.0f)
					{
						SRect clip = m_bounds;
						float f = (m_value - m_min) / d;
						clip.Width = clip.Width * f;

						if (clip.Width > 0.0f)
						{
							const SRect oldClipRect = renderer->clipRegion();
							bool isEnableClip = renderer->isEnableClip();

							renderer->addClipRegion(clip);
							renderer->enableClip(true);
							renderer->startClip();

							theme->drawTextBox(getRenderBounds(), ThemeConfig::SliderBarColor);

							renderer->setClipRegion(oldClipRect);
							renderer->enableClip(isEnableClip);
							renderer->startClip();
						}
					}

					if (m_textContainer->isActivate())
						theme->drawTextBoxBorder(getRenderBounds(), ThemeConfig::ButtonPressColor);
					else
					{
						if (isHovered())
							theme->drawTextBoxBorder(getRenderBounds(), ThemeConfig::ButtonHoverColor);
						else
							theme->drawTextBoxBorder(getRenderBounds(), ThemeConfig::TextBoxBorderColor);
					}
				}
				else
				{
					CScrollControl::renderUnder();
				}
			}

			bool CSlider::onChar(u32 c)
			{
				if ((c >= '0' && c <= '9') || c == '\b')
				{
					return CTextBox::onChar(c);
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

			void CSlider::onKeyboardFocus()
			{
				// disable default textbox focus
			}

			void CSlider::onLostKeyboardFocus()
			{
				if (m_focusTextbox)
				{
					CTextBox::onLostKeyboardFocus();

					applyTextValue();

					m_focusTextbox = false;
					setCursor(ECursorType::SizeWE);
					setCaretToEnd();

					if (OnEndTextEdit != nullptr)
						OnEndTextEdit(this);
				}
			}

			void CSlider::onMouseClickLeft(float x, float y, bool down)
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

						input->setCapture(this);
					}
					else
					{
						input->setCapture(NULL);

						if (!m_drag)
						{
							CTextBox::onKeyboardFocus();
							CTextBox::onMouseClickLeft(x, y, true);
							CTextBox::onMouseClickLeft(x, y, false);

							onSelectAll(this);

							m_focusTextbox = true;
							setCursor(ECursorType::Beam);
						}
						else
						{
							if (OnEndTextEdit != nullptr)
								OnEndTextEdit(this);
						}

						m_drag = false;
					}
				}
			}

			void CSlider::setValue(float value, float min, float max, bool invokeEvent)
			{
				m_value = value;
				m_min = min;
				m_max = max;

				m_value = core::clamp(m_value, m_min, m_max);

				if (m_min > m_max)
					core::swap(m_min, m_max);

				wchar_t text[64];
				if (m_numberType == Float)
					swprintf(text, 64, L"%.05f", m_value);
				else
					swprintf(text, 64, L"%d", (int)m_value);
				setString(std::wstring(text));


				if (invokeEvent && OnTextChanged != nullptr)
					OnTextChanged(this);
			}

			void CSlider::setValue(float value, bool invokeEvent)
			{
				setValue(value, m_min, m_max, invokeEvent);
			}

			void CSlider::applyTextValue()
			{
				const std::wstring s = getString();

				char utf8[512];
				CStringImp::convertUnicodeToUTF8(s.c_str(), utf8);

				float value = (float)atof(utf8);
				setValue(value, m_min, m_max, true);
			}
		}
	}
}