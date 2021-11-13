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
#include "CColorPicker.h"
#include "CCanvas.h"
#include "GUI//Renderer/CRenderer.h"
#include "GUI/Theme/CTheme.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CColorPicker::CColorPicker(CBase* parent) :
				CButton(parent)
			{
				setSize(100.0f, 20.0f);

				setPadding(SPadding(0.0f, 0.0f, 0.0f, 0.0f));

				OnDown = BIND_LISTENER(&CColorPicker::onClickDown, this);

				m_label->setHidden(true);

				m_menuPicker = new CMenu(parent->getCanvas());

				m_hueRGBPicker = new CColorHueRGBPicker(m_menuPicker);
				m_hueRGBPicker->setHeight(385.0f);
				m_hueRGBPicker->dock(GUI::EPosition::Top);
			}

			CColorPicker::~CColorPicker()
			{
				m_menuPicker->closeMenu();
				m_menuPicker->remove();
			}

			void CColorPicker::setColor(const SGUIColor& color)
			{
				m_color = color;
				m_hueRGBPicker->setColor(color);
			}

			void CColorPicker::renderUnder()
			{
				CButton::renderUnder();

				SRect bounds = getRenderBounds();
				float offset = 2.0f;
				bounds.X = bounds.X + offset;
				bounds.Y = bounds.Y + offset;
				bounds.Width = bounds.Width - 2.0f * offset;
				bounds.Height = bounds.Height - 2.0f * offset;

				CRenderer::getRenderer()->drawFillRect(bounds, m_color);
			}

			void CColorPicker::onClickDown(CBase* sender)
			{
				m_menuPicker->setWidth(325.0f);
				m_menuPicker->open(this);
			}
		}
	}
}