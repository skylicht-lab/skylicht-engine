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
#include "CMenu.h"
#include "CCanvas.h"

#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CMenu::CMenu(CBase *parent) :
				CBase(parent),
				m_isOpenSubMenu(false),
				m_isOpenPopup(false)
			{
				setBounds(0.0f, 0.0f, 210, 24.0f);
				setHidden(true);
			}

			CMenu::~CMenu()
			{

			}

			void CMenu::layout()
			{
				float menuHeight = 5.0f;
				bool firstMenu = true;

				for (CBase *menuItem : Children)
				{
					if (firstMenu == true)
					{
						SMargin margin = menuItem->getMargin();
						margin.Top = 2.0f;
						menuItem->setMargin(margin);

						firstMenu = false;
					}
					menuHeight = menuHeight + menuItem->height();
				}

				// if tall menu descends off screen then clamp to screen height
				if (Y() + menuHeight > getCanvas()->height())
				{
					const float dy = getCanvas()->height() - Y();
					const float ytop = core::max_(Y() - dy, 0.0f);

					setPos(X(), ytop);
					menuHeight = core::min_(menuHeight, getCanvas()->height());
				}

				setSize(width(), menuHeight);

				SRect bound = m_parent->getBounds();
				if (X() + width() > bound.X + bound.Width)
				{
					float newX = bound.X + bound.Width - width();
					setPos(newX, Y());
				}

				CBase::layout();
			}

			void CMenu::renderUnder()
			{
				CTheme::getTheme()->drawWindowShadow(getRenderBounds());

				CTheme::getTheme()->drawWindow(getRenderBounds(), true);
			}

			CMenuItem* CMenu::getItemByPath(const std::wstring& path)
			{
				std::vector<std::wstring> result;
				CStringImp::splitString(path.c_str(), L"/", result);

				CMenu *menu = this;
				CMenuItem *item = NULL;

				while (result.size() > 0)
				{
					if (result[0].empty() == true)
						return NULL;

					item = menu->getItemByLabel(result[0]);
					if (item == NULL)
						return NULL;

					result.erase(result.begin());

					if (result.size() > 0)
					{
						if (item->haveSubMenu() == false)
							return NULL;
						else
						{
							menu = item->getMenu();
							item = NULL;
						}
					}
				}

				return item;
			}

			CMenuItem* CMenu::getItemByLabel(const std::wstring& label)
			{
				for (CBase* child : Children)
				{
					CMenuItem *item = dynamic_cast<CMenuItem*>(child);
					if (item != NULL && item->getLabel() == label)
					{
						return item;
					}
				}
				return NULL;
			}

			CMenuItem* CMenu::addItemByPath(const std::wstring& path)
			{
				return addItemByPath(path, ESystemIcon::None, L"");
			}

			CMenuItem* CMenu::addItemByPath(const std::wstring& path, const std::wstring& accelerator)
			{
				return addItemByPath(path, ESystemIcon::None, accelerator);
			}

			CMenuItem* CMenu::addItemByPath(const std::wstring& path, ESystemIcon icon)
			{
				return addItemByPath(path, icon, L"");
			}

			CMenuItem* CMenu::addItemByPath(const std::wstring& path, ESystemIcon icon, const std::wstring& accelerator)
			{
				std::vector<std::wstring> result;
				CStringImp::splitString(path.c_str(), L"/", result);

				CMenu *menu = this;
				CMenuItem *item = NULL;

				while (result.size() > 0)
				{
					bool created = false;

					if (result[0].empty() == true)
						return NULL;

					item = menu->getItemByLabel(result[0]);
					if (item == NULL)
					{
						if (result.size() > 0)
							item = menu->addItem(result[0]);
						else
							item = menu->addItem(result[0], icon, accelerator);

						created = true;
					}

					result.erase(result.begin());

					if (result.size() > 0)
						menu = item->getMenu();

					if (created == false)
						item = NULL;
				}

				return item;
			}

			CMenuItem* CMenu::addItem(ESystemIcon icon)
			{
				CMenuItem *iconItem = addItem(std::wstring(L""), icon, std::wstring(L""));
				iconItem->setPadding(GUI::SPadding(6.0f, 0.0f, 0.0f, 0.0f));
				iconItem->setIconMargin(GUI::SMargin(-2.0f, 0.0f, 0.0f, 0.0f));
				iconItem->sizeToContents();
				return iconItem;
			}

			CMenuItem* CMenu::addItem(const std::wstring& label)
			{
				return addItem(label, ESystemIcon::None, std::wstring(L""));
			}

			CMenuItem* CMenu::addItem(const std::wstring& label, const std::wstring& accelerator)
			{
				return addItem(label, ESystemIcon::None, accelerator);
			}

			CMenuItem* CMenu::addItem(const std::wstring& label, ESystemIcon icon)
			{
				return addItem(label, icon, std::wstring(L""));
			}

			CMenuItem* CMenu::addItem(const std::wstring& label, ESystemIcon icon, const std::wstring& accelerator)
			{
				CMenuItem *item = new CMenuItem(this);
				item->setLabel(label);
				item->setIcon(icon);
				item->showIcon(true);
				item->setAccelerator(accelerator);
				item->setLabelMargin(SMargin(4.0f, 3.0f, 0.0f, 0.0f));
				item->OnHoverEnter = BIND_LISTENER(&CMenu::onMenuItemHover, this);
				item->OnPress = BIND_LISTENER(&CMenu::onMenuItemPress, this);
				onAddItem(item);

				return item;
			}

			CMenuSeparator* CMenu::addSeparator()
			{
				CMenuSeparator *item = new CMenuSeparator(this);
				item->dock(EPosition::Top);
				return item;
			}

			void CMenu::onAddItem(CMenuItem *item)
			{
				item->dock(EPosition::Top);
				item->sizeToContents();

				float w = core::max_<float>(item->width() + 30.0f, width());
				setSize(w, height());
			}

			void CMenu::onMenuItemHover(CBase *item)
			{
				CMenuItem *menuItem = dynamic_cast<CMenuItem*>(item);
				if (menuItem != NULL)
				{
					openMenu(menuItem);
				}
			}

			void CMenu::onMenuItemPress(CBase *item)
			{
				CMenuItem *menuItem = dynamic_cast<CMenuItem*>(item);
				if (menuItem != NULL)
				{
					if (menuItem->haveSubMenu() == false)
						getCanvas()->closeMenu();

					if (OnCommand != nullptr)
					{

						OnCommand(menuItem);
					}
				}
			}

			void CMenu::open(const SPoint& position)
			{
				setHidden(false);
				setPos(position);

				if (OnOpen != nullptr)
					OnOpen(this);

				bringToFront();
				m_isOpenPopup = true;
			}

			void CMenu::close()
			{
				if (m_isOpenPopup == true)
				{
					m_isOpenPopup = false;
					setHidden(true);
				}
			}

			void CMenu::openMenu(CMenuItem *menuItem)
			{
				if (menuItem->isOpenMenu())
					return;

				closeChildMenu();
				menuItem->openMenu();

				m_isOpenSubMenu = true;
			}

			void CMenu::closeMenu()
			{
				closeChildMenu();
				close();
			}

			void CMenu::closeChildMenu()
			{
				for (CBase *child : Children)
				{
					CMenuItem *menuItem = dynamic_cast<CMenuItem*>(child);
					if (menuItem != NULL)
						menuItem->closeMenu();
				}
				m_isOpenSubMenu = false;
			}
		}
	}
}