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
#include "CMenuBar.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Controls/CCanvas.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CMenuBar::CMenuBar(CBase *parent) :
				CMenu(parent)
			{
				dock(EPosition::Top);
				setHeight(26.0f);
				setHidden(false);

				enableRenderFillRect(true);
				setFillRectColor(CThemeConfig::MenuBarColor);
			}

			CMenuBar::~CMenuBar()
			{

			}

			void CMenuBar::layout()
			{
				CBase::layout();
			}

			void CMenuBar::renderUnder()
			{
				CBase::renderUnder();
			}

			void CMenuBar::onAddItem(CMenuItem *item)
			{
				if (item->getIcon() == ESystemIcon::None)
					item->showIcon(false);

				item->dock(EPosition::Left);
				item->setPadding(SPadding(6.0f, 0.0f, -6.0f, 0.0f));
				item->setMargin(SMargin(0.0f, 3.0f, 0.0f, 3.0f));
				item->setLabelMargin(SMargin(0.0f, 2.0f, 0.0f, 0.0f));
				item->setInMenuBar(true);
				item->sizeToContents();
				item->OnDown = BIND_LISTENER(&CMenuBar::onMenuItemDown, this);
			}

			void CMenuBar::onMenuItemHover(CBase *item)
			{
				if (m_isOpenSubMenu == true)
				{
					CMenu::onMenuItemHover(item);
				}
			}

			void CMenuBar::onMenuItemDown(CBase *item)
			{
				CMenuItem *menuItem = dynamic_cast<CMenuItem*>(item);
				if (menuItem != NULL)
				{
					if (m_isOpenSubMenu)
						closeMenu();
					else
					{
						getCanvas()->closeMenu();
						openMenu(menuItem);
					}
				}
			}
		}
	}
}