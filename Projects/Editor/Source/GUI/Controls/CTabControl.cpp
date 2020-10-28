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
#include "CTabControl.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTabControl::CTabControl(CBase *parent) :
				CBase(parent),
				m_currentTab(NULL)
			{
				m_tabStrip = new CTabStrip(this);
				m_tabStrip->dock(EPosition::Top);

				m_innerPanel = new CBase(this);
				m_innerPanel->dock(EPosition::Fill);
			}

			CTabControl::~CTabControl()
			{

			}

			CBase* CTabControl::addPage(const std::wstring& label, CBase *page)
			{
				if (page == NULL)
					page = new CBase(this);
				else
					page->setParent(this);

				CTabButton *tabButton = new CTabButton(m_tabStrip, page);
				tabButton->setLabel(label);

				addTabButton(tabButton);
				return page;
			}

			void CTabControl::removePage(CBase *page)
			{
				ListTabButton::iterator i = m_tabButtons.begin(), end = m_tabButtons.end();
				ListTabButton::iterator next;

				bool findNewFocus = false;

				while (i != end)
				{
					CTabButton *button = (*i);

					if (button->getPage() == page)
					{
						if (button == m_currentTab)
						{
							next = i;
							++next;
							onTabUnfocus(m_currentTab);
							findNewFocus = true;
						}

						onCloseTab(button);

						removeTabButton(button);
						m_tabButtons.erase(i);

						if (findNewFocus == true)
						{
							if (next != m_tabButtons.end())
							{
								m_currentTab = (*next);
								onTabFocus(m_currentTab);
							}
							else
							{
								m_currentTab = NULL;

								if (m_tabButtons.size() > 0)
									m_currentTab = m_tabButtons.front();

								if (m_currentTab != NULL)
									onTabFocus(m_currentTab);
							}
						}

						return;
					}
					++i;
				}
			}

			void CTabControl::onTabFocus(CTabButton *tab)
			{
				if (OnTabFocus != nullptr)
					OnTabFocus(this, tab->getPage());

				tab->setFocus(true);
				tab->getPage()->setHidden(false);
			}

			void CTabControl::onTabUnfocus(CTabButton *tab)
			{
				if (OnTabUnfocus != nullptr)
					OnTabUnfocus(this, tab->getPage());

				tab->setFocus(false);
				tab->getPage()->setHidden(true);
			}

			void CTabControl::onTabPressed(CBase *tab)
			{
				if (m_currentTab == tab)
					return;

				CTabButton *tabButton = (CTabButton*)tab;

				if (m_currentTab)
					onTabUnfocus(m_currentTab);

				m_currentTab = tabButton;
				onTabFocus(m_currentTab);

				if (OnTabPressed != nullptr)
					OnTabPressed(this, tabButton->getPage());
			}

			void CTabControl::onCloseTab(CTabButton *tab)
			{
				if (OnCloseTab != nullptr)
					OnCloseTab(this, tab);
			}

			void CTabControl::addTabButton(CTabButton *button)
			{
				if (m_currentTab != NULL)
				{
					onTabUnfocus(m_currentTab);
				}

				CBase *page = button->getPage();
				page->dock(EPosition::Fill);

				button->dock(EPosition::Left);
				button->sizeToContents();

				if (m_tabButtons.size() > 0)
					button->setMargin(SMargin(2.0f, 6.0f, 0.0f, 2.0f));
				else
					button->setMargin(SMargin(0.0f, 6.0f, 0.0f, 2.0f));

				button->OnDown = BIND_LISTENER(&CTabControl::onTabPressed, this);

				m_tabButtons.push_back(button);

				m_currentTab = button;
				onTabFocus(m_currentTab);

				invalidate();
			}

			void CTabControl::removeTabButton(CTabButton *button)
			{
				button->getPage()->remove();
				button->remove();
			}
		}
	}
}