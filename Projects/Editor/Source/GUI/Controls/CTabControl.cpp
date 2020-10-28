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
				CBase(parent)
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
				while (i != end)
				{
					if ((*i)->getPage() == page)
					{
						removeTabButton((*i));
						m_tabButtons.erase(i);						
						return;
					}
					++i;
				}
			}

			void CTabControl::addTabButton(CTabButton *button)
			{
				CBase *page = button->getPage();
				page->dock(EPosition::Fill);

				button->dock(EPosition::Left);
				button->sizeToContents();

				if (m_tabButtons.size() > 0)
					button->setMargin(SMargin(2.0f, 6.0f, 0.0f, 2.0f));
				else
					button->setMargin(SMargin(0.0f, 6.0f, 0.0f, 2.0f));

				m_tabButtons.push_back(button);

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