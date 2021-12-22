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
#include "CCollapsibleGroup.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CCollapsibleGroup::CCollapsibleGroup(CBase* parent) :
				CBase(parent)
			{
				m_header = new CCollapsibleButton(this);
				m_header->dock(EPosition::Top);
				m_header->OnPress = BIND_LISTENER(&CCollapsibleGroup::onExpand, this);

				m_innerPanel = new CBase(this);
				m_innerPanel->dock(EPosition::Top);
				m_innerPanel->setHeight(20.0f);
				m_innerPanel->setHidden(true);
			}

			CCollapsibleGroup::~CCollapsibleGroup()
			{

			}

			void CCollapsibleGroup::renderUnder()
			{
				CBase::renderUnder();
			}

			void CCollapsibleGroup::postLayout()
			{
				if (m_innerPanel->isHidden() == false)
					m_innerPanel->sizeToChildren(true, true);

				sizeToChildren(true, true);
			}

			void CCollapsibleGroup::onExpand(CBase* sender)
			{
				if (m_header->getToggle() == true)
				{
					m_innerPanel->setHidden(false);
				}
				else
				{
					m_innerPanel->setHidden(true);
				}
			}

			void CCollapsibleGroup::setExpand(bool b)
			{
				m_header->setToggle(b);

				if (b == true)
				{
					m_header->setIcon(GUI::ESystemIcon::SmallTriangleDown);
					m_innerPanel->setHidden(false);
				}
				else
				{
					m_header->setIcon(GUI::ESystemIcon::SmallTriangleRight);
					m_innerPanel->setHidden(true);
				}
			}
		}
	}
}