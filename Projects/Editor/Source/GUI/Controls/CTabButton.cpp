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
#include "CTabButton.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTabButton::CTabButton(CBase *parent, CBase *page) :
				CButton(parent),
				m_page(page),
				m_focus(false),
				m_showCloseButton(false),
				m_enableRenderOver(false)
			{
				m_color = CThemeConfig::TabButtonColor;
				m_pressColor = CThemeConfig::TabButtonActiveColor;
				m_hoverColor = CThemeConfig::TabButtonActiveColor;
				m_focusColor = CThemeConfig::TabButtonFocusColor;

				m_label->setMargin(SMargin(0.0f, 4.0f, 0.0f, 0.0f));
				m_label->setColor(CThemeConfig::TabTextColor);

				m_close = new CIconButton(this);
				m_close->setMargin(SMargin(0.0f, 1.0f, 8.0f, -1.0f));
				m_close->dock(EPosition::Right);
				m_close->setIcon(ESystemIcon::Close);
				m_close->setHidden(!m_showCloseButton);
				m_close->enableDrawBackground(false);
				m_close->enableDrawHoverRectangle(true);
				m_close->setHoverRectangleMargin(SMargin(3.0f, 3.0f, 3.0f, 3.0f));
			}

			CTabButton::~CTabButton()
			{

			}

			void CTabButton::onMouseClickLeft(float x, float y, bool down)
			{
				CButton::onMouseClickLeft(x, y, down);

				if (down)
				{
					m_holdPosition = canvasPosToLocal(SPoint(x, y));
					CInput::getInput()->setCapture(this);
				}
				else
				{
					CInput::getInput()->setCapture(NULL);
					m_enableRenderOver = false;
				}
			}

			void CTabButton::onMouseMoved(float x, float y, float deltaX, float deltaY)
			{
				CButton::onMouseMoved(x, y, deltaX, deltaY);

				if (m_pressed && (deltaX > 0 || deltaY > 0))
					m_enableRenderOver = true;
			}

			void CTabButton::renderOver()
			{
				if (m_enableRenderOver)
				{
					SGUIColor c(150, m_pressColor.R, m_pressColor.G, m_pressColor.B);

					SPoint oldOffset = CRenderer::getRenderer()->getRenderOffset();

					SPoint mousePosition = CInput::getInput()->getMousePosition();
					SPoint renderOffset(mousePosition.X - m_holdPosition.X, mousePosition.Y - m_holdPosition.Y);

					CRenderer::getRenderer()->setRenderOffset(renderOffset);
					CTheme::getTheme()->drawTabButton(getRenderBounds(), c, m_focusColor, m_focus);

					CRenderer::getRenderer()->setRenderOffset(oldOffset);
				}
			}

			void CTabButton::renderUnder()
			{
				SGUIColor c = m_color;

				if (isHovered() || m_close->isHovered())
				{
					if (m_pressed == true)
						c = m_pressColor;
					else
						c = m_hoverColor;
				}

				if (m_focus)
				{
					c = m_pressColor;

					m_label->setColor(CThemeConfig::TabTextFocusColor);
				}
				else
				{
					m_label->setColor(CThemeConfig::TabTextColor);
				}

				CTheme::getTheme()->drawTabButton(getRenderBounds(), c, m_focusColor, m_focus);
			}

			void CTabButton::sizeToContents()
			{
				CButton::sizeToContents();

				if (m_showCloseButton)
				{
					float w = width() + m_close->width();
					float h = height();

					setSize(w, h);
				}
			}
		}
	}
}