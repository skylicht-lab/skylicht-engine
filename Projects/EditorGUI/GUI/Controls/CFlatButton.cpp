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
#include "CFlatButton.h"
#include "GUI/CGUIContext.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Renderer/CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CFlatButton::CFlatButton(CBase* parent) :
				CButton(parent),
				m_borderL(true),
				m_borderR(true),
				m_borderT(true),
				m_borderB(true),
				m_borderColor(CThemeConfig::WindowInnerColor)
			{
				setHeight(25.0f);
				m_label->setMargin(SMargin(0.0f, 5.0f, 0.0f, 0.0f));
			}

			CFlatButton::~CFlatButton()
			{

			}

			void CFlatButton::renderUnder()
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
					if (pressed == true && !m_isToggle)
						c = m_pressColor;
					else
						c = m_hoverColor;
				}

				if (isToggle())
				{
					if (m_toggleStatus)
						c = m_pressColor;
				}

				if (!disable && (m_drawBackground || hover || (m_isToggle && m_toggleStatus)))
				{
					CRenderer::getRenderer()->drawFillRect(getRenderBounds(), c);
					CRenderer::getRenderer()->drawBorderRect(
						getRenderBounds(),
						m_borderColor,
						m_borderL, m_borderT, m_borderR, m_borderB
					);
				}
			}
		}
	}
}