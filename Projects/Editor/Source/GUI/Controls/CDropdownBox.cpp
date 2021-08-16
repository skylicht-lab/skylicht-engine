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
#include "CDropdownBox.h"
#include "CCanvas.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CDropdownBox::CDropdownBox(CBase* parent) :
				CButton(parent)
			{
				m_menu = new CMenuEx(getCanvas());
				m_menu->setHidden(true);

				OnDown = BIND_LISTENER(&CDropdownBox::onClickDown, this);
			}

			CDropdownBox::~CDropdownBox()
			{

			}

			void CDropdownBox::renderUnder()
			{
				CTheme* theme = CTheme::getTheme();

				const SRect& r = getRenderBounds();

				theme->drawTextBox(r, CThemeConfig::TextBoxColor);

				if (m_menu->isOpen())
					theme->drawTextBoxBorder(r, CThemeConfig::ButtonPressColor);
				else if (isHovered())
					theme->drawTextBoxBorder(r, CThemeConfig::ButtonHoverColor);
				else
					theme->drawTextBoxBorder(r, CThemeConfig::TextBoxBorderColor);

				if (m_menu->isOpen())
					theme->drawComboBoxButton(r, CThemeConfig::ButtonPressColor, CThemeConfig::White);
				else
					theme->drawComboBoxButton(r, CThemeConfig::TextBoxButtonColor, CThemeConfig::White);
			}

			void CDropdownBox::onClickDown(CBase* sender)
			{
				if (!m_menu->isOpen())
				{
					openMenu();
				}
				else
				{
					closeMenu();
				}
			}

			void CDropdownBox::openMenu()
			{
				getCanvas()->closeMenu();

				SPoint p = localPosToCanvas();
				m_menu->open(SPoint(p.X, p.Y + height()));
				m_menu->setWidth(width());
			}

			void CDropdownBox::closeMenu()
			{
				getCanvas()->closeMenu();
			}
		}
	}
}
