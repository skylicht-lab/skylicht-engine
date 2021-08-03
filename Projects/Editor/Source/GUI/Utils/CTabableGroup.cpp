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
				m_list.push_back(control);
			}

			void CTabableGroup::remove(CBase* control)
			{
				m_list.remove(control);
			}

			void CTabableGroup::next()
			{
				std::list<CBase*>::iterator it = std::find(m_list.begin(), m_list.end(), CurrentTab);
				if (it != --m_list.end())
				{
					++it;
					CurrentTab = (*it);
				}
				else
				{
					if (m_list.size() > 0)
						CurrentTab = m_list.front();
					else
						CurrentTab = NULL;
				}
			}

			void CTabableGroup::clear()
			{
				m_list.clear();
			}
		}
	}
}