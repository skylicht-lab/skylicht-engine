/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CTabableGroup.h"
#include "GUI/Controls/CBase.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTabableGroup::CTabableGroup() :
				m_enable(false),
				CurrentTab(NULL)
			{

			}

			CTabableGroup::~CTabableGroup()
			{

			}

			void CTabableGroup::add(CBase* control)
			{
				std::list<SElement>::iterator i = m_list.begin(), end = m_list.end();
				while (i != end)
				{
					if (i->Control == control)
						return;
					++i;
				}

				control->setTabable(true);

				m_list.push_back(SElement(control, true));
			}

			void CTabableGroup::remove(CBase* control)
			{
				std::list<SElement>::iterator i = m_list.begin(), end = m_list.end();
				while (i != end)
				{
					if (i->Control == control)
					{
						control->setTabable(false);

						m_list.erase(i);
						return;
					}
					++i;
				}
			}

			void CTabableGroup::next()
			{
				std::list<SElement>::iterator it = m_list.begin(), end = m_list.end();
				while (it != end)
				{
					if (it->Control == CurrentTab)
						break;
					++it;
				}

				if (it != --m_list.end())
				{
					// next
					++it;
				}
				else
				{
					// go to begin
					it = m_list.begin();
				}

				// focus to enable control
				CurrentTab = NULL;
				do
				{
					if (it->Enable == true)
					{
						CurrentTab = it->Control;
						break;
					}
					++it;
				} while (it != end);
			}

			void CTabableGroup::disableAllControl()
			{
				std::list<SElement>::iterator it = m_list.begin(), end = m_list.end();
				while (it != end)
				{
					it->Enable = false;
					++it;
				}
			}

			void CTabableGroup::setEnableControl(CBase* control)
			{
				std::list<SElement>::iterator it = m_list.begin(), end = m_list.end();
				while (it != end)
				{
					if (it->Control == control)
					{
						it->Enable = true;
					}
					++it;
				}
			}

			void CTabableGroup::clear()
			{
				std::list<SElement>::iterator i = m_list.begin(), end = m_list.end();
				while (i != end)
				{
					i->Control->setTabable(false);
					++i;
				}

				m_list.clear();
			}
		}
	}
}