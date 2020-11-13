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
#pragma once

#include "CBase.h"
#include "CTabButton.h"
#include "CTabStrip.h"
#include "CMenu.h"

#define BIND_TABCONTROL_LISTENER(A, B) std::bind(A, B, std::placeholders::_1, std::placeholders::_2)

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CTabControl :public CBase
			{
				friend class CTabButton;

			public:
				typedef std::function<void(CTabControl*, CBase*)> TabControlListener;
				typedef std::list<CTabButton*> ListTabButton;

			protected:
				CTabStrip *m_tabStrip;

				ListTabButton m_tabButtons;

				CTabButton* m_currentTab;
				CIconButton* m_dropDown;

				CMenu *m_dropDownMenu;
				bool m_isOpenMenu;

				bool m_showTabCloseButton;

			public:
				CTabControl(CBase *parent);

				virtual ~CTabControl();

				virtual void postLayout();

				virtual CBase* addPage(const std::wstring& label, CBase *page);

				void removeButtonOnly(CTabButton *button);

				void removePage(CBase *page);

				void showTabCloseButton(bool b);

				TabControlListener OnTabFocus;

				TabControlListener OnTabUnfocus;

				TabControlListener OnTabPressed;

				TabControlListener OnCloseTab;

				inline u32 getNumTabButton()
				{
					return m_tabButtons.size();
				}

				CTabButton* getCurrentTab()
				{
					return m_currentTab;
				}

				CTabButton* getTabButton(u32 id)
				{
					ListTabButton::iterator i = m_tabButtons.begin();
					std::advance(i, id);
					return (*i);
				}

				CTabButton* getTabButtonByPage(CBase *page);

				const ListTabButton& getListTabButton()
				{
					return m_tabButtons;
				}

				inline CTabStrip* getTabStrip()
				{
					return m_tabStrip;
				}

				void setTab(CTabButton *tab)
				{
					if (m_currentTab != tab)
						onTabUnfocus(m_currentTab);

					onTabFocus(tab);
					m_currentTab = tab;
				}

			protected:

				virtual void onDragOutTabStrip(CTabButton *tab) {}

				virtual void onDragOverTabStrip(CTabButton *tab) {}

				virtual void onTabFocus(CTabButton *tab);

				virtual void onTabUnfocus(CTabButton *tab);

				virtual void onTabPressed(CBase *tab);

				virtual void onDropDownPressed(CBase *button);

				virtual void onTabClosePressed(CBase *tabClose);

				virtual void onCloseTab(CTabButton *tab);

				virtual void doTabClose(CTabButton *button);

				void addTabButton(CTabButton *button);

				void removeTabButton(CTabButton *button);

			protected:

				CTabButton* getTabButton(CBase *base);

				float getMaxButtonRight();

				virtual void onMenuItem(CBase *menu);
			};
		}
	}
}