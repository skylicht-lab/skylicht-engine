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
#include "CComboBox.h"
#include "CCanvas.h"
#include "GUI/Theme/ThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CComboBox::CComboBox(CBase* parent) :
				CButton(parent),
				m_selectIndex(0)
			{
				m_menu = new CMenu(getCanvas());
				m_menu->setHidden(true);
				m_menu->OnCommand = BIND_LISTENER(&CComboBox::onCommand, this);

				OnDown = BIND_LISTENER(&CComboBox::onClickDown, this);
			}

			CComboBox::~CComboBox()
			{

			}

			void CComboBox::renderUnder()
			{
				CTheme* theme = CTheme::getTheme();

				const SRect& r = getRenderBounds();

				if (isDisabled())
				{
					m_label->setColor(ThemeConfig::ButtonTextDisableColor);
					m_icon->setColor(ThemeConfig::ButtonTextDisableColor);
				}
				else
				{
					m_label->setColor(m_labelColor);
					m_icon->setColor(m_iconColor);
				}

				theme->drawTextBox(r, ThemeConfig::TextBoxColor);

				if (m_menu->isOpen())
					theme->drawTextBoxBorder(r, ThemeConfig::ButtonPressColor);
				else if (isHovered())
					theme->drawTextBoxBorder(r, ThemeConfig::ButtonHoverColor);
				else
					theme->drawTextBoxBorder(r, ThemeConfig::TextBoxBorderColor);

				if (m_menu->isOpen())
					theme->drawComboBoxButton(r, ThemeConfig::ButtonPressColor, ThemeConfig::White);
				else
					theme->drawComboBoxButton(r, ThemeConfig::TextBoxButtonColor, ThemeConfig::White);
			}

			void CComboBox::clear()
			{
				m_list.clear();
				m_menuItems.clear();

				m_menu->releaseChildren();
				m_selectIndex = 0;
			}

			void CComboBox::setListValue(const std::vector<std::wstring>& list)
			{
				clear();

				for (const std::wstring& s : list)
				{
					addItem(s);
				}

				if (list.size() > 0)
				{
					setLabel(list[m_selectIndex]);
				}
			}

			void CComboBox::setSelectIndex(u32 index, bool invokeEvent)
			{
				if (index < m_list.size())
				{
					m_selectIndex = index;
					setLabel(m_list[m_selectIndex]);

					if (invokeEvent && OnChanged != nullptr)
						OnChanged(this);
				}
			}

			void CComboBox::addItem(const std::wstring& value)
			{
				m_list.push_back(value);

				CMenuItem* item = m_menu->addItem(value);
				m_menuItems.push_back(item);
			}

			void CComboBox::onClickDown(CBase* sender)
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

			void CComboBox::onCommand(CBase* sender)
			{
				u32 id = 0;

				for (CMenuItem* item : m_menuItems)
				{
					if (item == sender && id < m_list.size())
					{
						setLabel(m_list[id]);

						m_selectIndex = id;

						if (OnChanged != nullptr)
							OnChanged(this);

						break;
					}

					id++;
				}
			}

			void CComboBox::openMenu()
			{
				// getCanvas()->closeMenu();

				m_menu->open(this);
				m_menu->closeAllMenuWhenClicked(false);
				m_menu->setWidth(width());
			}

			void CComboBox::closeMenu()
			{
				// getCanvas()->closeMenu();

				m_menu->close();
			}
		}
	}
}
