/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CListBox.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CListBox::CListBox(CBase* parent) :
				CListBase(parent)
			{

			}

			CListBox::~CListBox()
			{

			}

			void CListBox::postLayout()
			{
				CBase::postLayout();

				float w = getInnerWidth();
				for (CBase* child : m_innerPanel->Children)
				{
					if (child->width() < w)
						child->setWidth(w);
				}
			}

			CListItemBase* CListBox::addItem(const std::wstring& label, ESystemIcon icon)
			{
				return addRowItem(label, icon);
			}

			CListItemBase* CListBox::addItem(const std::wstring& label, CGUIImage* image, const SRect& srcRect)
			{
				return addRowItem(label);
			}

			CListItemBase* CListBox::addItem(const std::wstring& label)
			{
				return addRowItem(label);
			}

			CListRowItem* CListBox::addRowItem(const std::wstring& label, ESystemIcon icon)
			{
				CListRowItem* item = new CListRowItem(this);
				item->dock(EPosition::Top);
				item->setLabel(label);
				item->setIcon(icon);
				item->OnDown = BIND_LISTENER(&CListBox::onItemDown, this);
				return item;
			}

			CListRowItem* CListBox::addRowItem(const std::wstring& label)
			{
				CListRowItem* item = new CListRowItem(this);
				item->dock(EPosition::Top);
				item->setLabel(label);
				item->setIcon(ESystemIcon::None);
				item->OnDown = BIND_LISTENER(&CListBox::onItemDown, this);
				return item;
			}
		}
	}
}