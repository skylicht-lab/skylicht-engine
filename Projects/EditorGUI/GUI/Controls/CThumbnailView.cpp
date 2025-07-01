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
#include "CThumbnailView.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CThumbnailView::CThumbnailView(CBase* parent, float itemWidth, float itemHeight) :
				CListBase(parent),
				m_itemWidth(itemWidth),
				m_itemHeight(itemHeight)
			{

			}

			CThumbnailView::~CThumbnailView()
			{

			}

			void CThumbnailView::layout()
			{
				CListBase::layout();

				float totalWidth = width() - 5.0f;

				int numItems = (int)m_innerPanel->Children.size();
				int numItemInRow = (int)(totalWidth / m_itemWidth);
				if (numItemInRow == 0)
					return;

				float itemWidth = totalWidth / numItemInRow;

				float offsetX = (itemWidth - m_itemWidth) * 0.5f;
				float x = 0.0f;
				float y = 0.0f;
				int i = 0;

				for (CBase* child : m_innerPanel->Children)
				{
					CThumbnailItem* item = dynamic_cast<CThumbnailItem*>(child);
					if (item)
					{
						if (i > 0 && i % numItemInRow == 0)
						{
							x = 0.0f;
							y = y + m_itemHeight + 5.0f;
						}

						item->setPos(x + offsetX, y);
						x = x + itemWidth;
						i++;
					}
				}
			}

			void CThumbnailView::postLayout()
			{
				CListBase::postLayout();
			}

			int CThumbnailView::getNumItemInRow()
			{
				float totalWidth = width() - 5.0f;
				return (int)(totalWidth / m_itemWidth);
			}

			CListItemBase* CThumbnailView::addItem(const std::wstring& label, ESystemIcon icon)
			{
				CThumbnailItem* item = addItem(true);
				item->setLabel(label.c_str());
				item->setIcon(icon);
				return item;
			}

			CListItemBase* CThumbnailView::addItem(const std::wstring& label)
			{
				CThumbnailItem* item = addItem(false);
				item->setLabel(label.c_str());
				return item;
			}

			CThumbnailItem* CThumbnailView::addItem(bool haveLargeIcon)
			{
				CThumbnailItem* item = new CThumbnailItem(this, m_itemWidth, m_itemHeight, haveLargeIcon);
				item->OnDown = BIND_LISTENER(&CThumbnailView::onItemDown, this);
				invalidate();
				return item;
			}

			bool CThumbnailView::onKeyUp(bool down)
			{
				if (down)
				{
					int numItemInRow = getNumItemInRow();
					std::vector<CButton*> btns;

					for (CBase* child : m_innerPanel->Children)
					{
						CButton* item = dynamic_cast<CButton*>(child);
						if (item != NULL)
						{
							if (item->getToggle() == true)
							{
								if (btns.size() > 0 && btns.size() >= numItemInRow)
								{
									CButton* sel = btns[btns.size() - numItemInRow];

									if (OnSelected != nullptr)
										OnSelected(sel);

									if (OnSelectChange != nullptr)
										OnSelectChange(sel);

									sel->setToggle(true);
									item->setToggle(false);

									scrollToItem(sel);
								}
								break;
							}
							btns.push_back(item);

							if (btns.size() > numItemInRow + 1)
								btns.erase(btns.begin());
						}
					}
				}
				return true;
			}

			bool CThumbnailView::onKeyDown(bool down)
			{
				if (down)
				{
					int numItemInRow = getNumItemInRow();

					auto it = m_innerPanel->Children.begin();
					auto end = m_innerPanel->Children.end();
					while (it != end)
					{
						CButton* item = dynamic_cast<CButton*>(*it);
						if (item != NULL)
						{
							if (item->getToggle() == true)
							{
								CButton* sel = NULL;

								for (int i = 0; i < numItemInRow; i++)
								{
									++it;
									if (it == end)
										break;
									if (i == numItemInRow - 1)
										sel = dynamic_cast<CButton*>(*it);
								}

								if (sel)
								{
									if (OnSelected != nullptr)
										OnSelected(sel);

									if (OnSelectChange != nullptr)
										OnSelectChange(sel);

									sel->setToggle(true);
									item->setToggle(false);

									scrollToItem(sel);
								}
								break;
							}
						}
						++it;
					}
				}
				return true;
			}

			bool CThumbnailView::onKeyLeft(bool down)
			{
				if (down)
				{
					CButton* lastItem = NULL;
					for (CBase* child : m_innerPanel->Children)
					{
						CButton* item = dynamic_cast<CButton*>(child);
						if (item != NULL)
						{
							if (item->getToggle() == true)
							{
								if (lastItem != NULL && !lastItem->isDisabled())
								{
									if (OnSelected != nullptr)
										OnSelected(lastItem);

									if (OnSelectChange != nullptr)
										OnSelectChange(lastItem);

									lastItem->setToggle(true);
									item->setToggle(false);

									scrollToItem(lastItem);
									return true;
								}
								else
								{
									return true;
								}
							}
						}
						lastItem = item;
					}
				}

				return true;
			}

			bool CThumbnailView::onKeyRight(bool down)
			{
				if (down)
				{
					CButton* lastItem = NULL;
					for (CBase* child : m_innerPanel->Children)
					{
						CButton* item = dynamic_cast<CButton*>(child);
						if (item != NULL)
						{
							if (lastItem != NULL && !item->isDisabled())
							{
								if (OnSelected != nullptr)
									OnSelected(item);

								if (OnSelectChange != nullptr)
									OnSelectChange(item);

								item->setToggle(true);
								lastItem->setToggle(false);

								scrollToItem(item);
								return true;
							}
							else if (item->getToggle() == true)
							{
								lastItem = item;
							}
						}
					}
				}
				return true;
			}
		}
	}
}