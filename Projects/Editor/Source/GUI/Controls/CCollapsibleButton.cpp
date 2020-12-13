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
#include "CCollapsibleButton.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CCollapsibleButton::CCollapsibleButton(CBase* parent) :
				CButton(parent)
			{
				setPadding(SPadding(0.0f, 0.0f, 8.0f, 0.0f));
				setIcon(GUI::ESystemIcon::SmallTriangleRight);
				showIcon(true);
				setIsToggle(true);

				setColor(CThemeConfig::CCollapsibleColor);
				setHoverColor(CThemeConfig::ButtonColor);
			}

			CCollapsibleButton::~CCollapsibleButton()
			{

			}

			void CCollapsibleButton::renderUnder()
			{
				SGUIColor c = m_color;

				bool pressed = m_pressed;
				bool hover = isHovered();
				bool disable = isDisabled();

				if (disable)
				{
					m_label->setColor(CThemeConfig::ButtonTextDisableColor);
					m_icon->setColor(CThemeConfig::ButtonTextDisableColor);
				}
				else
				{
					m_label->setColor(m_labelColor);
					m_icon->setColor(m_iconColor);
				}

				if (hover && !disable)
				{
					if (pressed == true)
						c = m_pressColor;
					else
						c = m_hoverColor;
				}

				if (!disable && (m_drawBackground || hover))
					CRenderer::getRenderer()->drawFillRect(getRenderBounds(), c);
			}

			void CCollapsibleButton::onMouseClickLeft(float x, float y, bool down)
			{
				if (down == false)
				{
					m_toggleStatus = !m_toggleStatus;

					if (m_toggleStatus == true)
						setIcon(GUI::ESystemIcon::SmallTriangleDown);
					else
						setIcon(GUI::ESystemIcon::SmallTriangleRight);
				}

				CButton::onMouseClickLeft(x, y, down);
			}
		}
	}
}