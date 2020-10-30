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

#define BIND_TABCONTROL_LISTENER(A, B) std::bind(A, B, std::placeholders::_1, std::placeholders::_2)

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CTabControl :public CBase
			{
			public:
				typedef std::function<void(CTabControl*, CBase*)> TabControlListener;

			protected:
				CTabStrip *m_tabStrip;

				typedef std::list<CTabButton*> ListTabButton;
				ListTabButton m_tabButtons;

				CTabButton* m_currentTab;
				CIconButton* m_dropDown;

				bool m_showTabCloseButton;

			public:
				CTabControl(CBase *parent);

				virtual ~CTabControl();

				virtual void postLayout();

				CBase* addPage(const std::wstring& label, CBase *page);

				void removePage(CBase *page);

				void showTabCloseButton(bool b);

				TabControlListener OnTabFocus;

				TabControlListener OnTabUnfocus;

				TabControlListener OnTabPressed;

				TabControlListener OnCloseTab;

			protected:

				virtual void onTabFocus(CTabButton *tab);

				virtual void onTabUnfocus(CTabButton *tab);

				virtual void onTabPressed(CBase *tab);

				virtual void onDropDownPressed(CBase *button);

				virtual void onTabClosePressed(CBase *tabClose);

				virtual void onCloseTab(CTabButton *tab);

				void addTabButton(CTabButton *button);

				void removeTabButton(CTabButton *button);

			};
		}
	}
}