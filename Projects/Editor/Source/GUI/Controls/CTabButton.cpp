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
#include "CTabControl.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTabButton::CTabButton(CTabControl *control, CBase *parent, CBase *page) :
				CButton(parent),
				m_control(control),
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

					m_tabPosition = localPosToCanvas();
					m_tabStripPosition = m_parent->localPosToCanvas();

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

				if (m_pressed && (fabsf(deltaX) > 0 || fabsf(deltaY) > 0))
				{
					m_enableRenderOver = true;

					SPoint p = m_parent->canvasPosToLocal(SPoint(x, m_tabPosition.Y + height() * 0.5f));
					CBase *control = m_parent->getControlAt(p.X, p.Y);

					if (control != NULL)
					{
						CTabButton *btn = m_control->getTabButton(control);
						if (btn != NULL && btn != this)
							m_parent->bringSwapChildControl(this, btn);
					}
				}
			}

			void CTabButton::renderOver()
			{
				if (m_enableRenderOver)
				{
					CRenderer *renderer = CRenderer::getRenderer();
					SPoint oldOffset = renderer->getRenderOffset();
					SPoint mousePosition = CInput::getInput()->getMousePosition();

					float maxX = m_control->getMaxButtonRight();
					float x = mousePosition.X - m_holdPosition.X;
					float y = m_tabPosition.Y;

					x = core::max_(x, m_tabStripPosition.X);
					x = core::min_(x, m_tabStripPosition.X + maxX - width() - m_margin.Right);

					SRect clip = renderer->clipRegion();
					SPoint p = m_parent->localPosToCanvas();

					// on clipping
					renderer->enableClip(true);

					// set clip rect
					renderer->setClipRegion(SRect(p.X, p.Y, m_parent->width(), m_parent->height()));
					renderer->startClip();

					renderer->setRenderOffset(SPoint(x, y));
					CTheme::getTheme()->drawTabButton(getRenderBounds(), SGUIColor(150, m_pressColor), m_focusColor, m_focus);
					renderer->setRenderOffset(oldOffset);

					renderer->endClip();

					// set old clip rect
					renderer->setClipRegion(clip);
					renderer->startClip();
					renderer->endClip();

					// of clipping
					renderer->enableClip(false);
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

				SRect renderBound = getRenderBounds();

				if (m_focus)
				{
					renderBound.Height = renderBound.Height + 2.0f;
					CTheme::getTheme()->drawTabButton(renderBound, c, m_focusColor, m_focus);
				}
				else
				{
					renderBound.Y = renderBound.Y + 2.0f;
					CTheme::getTheme()->drawTabButton(renderBound, c, m_focusColor, m_focus);
				}
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