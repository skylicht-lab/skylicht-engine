/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CListBox.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CListBox::CListBox(CBase *parent) :
				CScrollControl(parent)
			{

			}

			CListBox::~CListBox()
			{

			}

			void CListBox::postLayout()
			{
				CBase::postLayout();

				float w = getInnerWidth();
				for (CBase *child : m_innerPanel->Children)
				{
					if (child->width() < w)
						child->setWidth(w);
				}
			}

			CListRowItem* CListBox::addItem(const std::wstring& label, ESystemIcon icon)
			{
				CListRowItem *item = new CListRowItem(this);
				item->dock(EPosition::Top);
				item->setLabel(label);
				item->setIcon(icon);
				item->OnDown = BIND_LISTENER(&CListBox::onItemDown, this);
				return item;
			}

			CListRowItem* CListBox::addItem(const std::wstring& label)
			{
				CListRowItem *item = new CListRowItem(this);
				item->dock(EPosition::Top);
				item->setLabel(label);
				item->setIcon(ESystemIcon::None);
				item->OnDown = BIND_LISTENER(&CListBox::onItemDown, this);
				return item;
			}

			CListRowItem* CListBox::getItemByLabel(const std::wstring& label)
			{
				for (CBase *child : m_innerPanel->Children)
				{
					CListRowItem *item = dynamic_cast<CListRowItem*>(child);
					if (item && item->getLabel() == label)
					{
						return item;
					}
				}
				return NULL;
			}

			CListRowItem* CListBox::getItemByTagValue(int value)
			{
				for (CBase *child : m_innerPanel->Children)
				{
					CListRowItem *item = dynamic_cast<CListRowItem*>(child);
					if (item && item->getTagInt() == value)
						return item;
				}
				return NULL;
			}

			void CListBox::onItemDown(CBase *base)
			{
				for (CBase *child : m_innerPanel->Children)
				{
					CListRowItem *item = dynamic_cast<CListRowItem*>(child);
					if (item != NULL)
					{
						if (item == base)
						{
							if (item->getToggle() == false)
							{
								if (OnSelected != nullptr)
									OnSelected(item);

								if (OnSelectChange != nullptr)
									OnSelectChange(item);
							}

							item->setToggle(true);
						}
						else
						{
							if (item->getToggle() == true && OnUnselected != nullptr)
								OnUnselected(item);

							item->setToggle(false);
						}
					}
				}
			}

			void CListBox::unSelectAll()
			{
				for (CBase *child : m_innerPanel->Children)
				{
					CListRowItem *item = dynamic_cast<CListRowItem*>(child);
					if (item != NULL)
					{
						item->setToggle(false);
					}
				}
			}
		}
	}
}