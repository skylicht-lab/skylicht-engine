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
				CBase(parent),
				m_itemWidth(itemWidth),
				m_itemHeight(itemHeight)
			{
				m_view = new CScrollControl(this);
				m_view->dock(EPosition::Fill);
			}

			CThumbnailView::~CThumbnailView()
			{

			}

			void CThumbnailView::layout()
			{
				CBase::layout();

				float totalWidth = width() - 5.0f;

				int numItemInRow = (int)(totalWidth / m_itemWidth);
				int numItems = (int)m_items.size();

				float itemWidth = totalWidth / numItemInRow;

				float offsetX = (int)((itemWidth - m_itemWidth) * 0.5f);
				float x = 0.0f;
				float y = 0.0f;

				for (int i = 0; i < numItems; i++)
				{
					if (i > 0 && i % numItemInRow == 0)
					{
						x = 0.0f;
						y = y + m_itemHeight;
					}

					CThumbnailItem* item = m_items[i];
					item->setPos(x + offsetX, y);

					x = x + itemWidth;
				}
			}

			void CThumbnailView::postLayout()
			{
				CBase::postLayout();
			}

			void CThumbnailView::unSelectAll()
			{

			}

			bool CThumbnailView::onKeyUp(bool down)
			{
				return true;
			}

			bool CThumbnailView::onKeyDown(bool down)
			{
				return true;
			}

			bool CThumbnailView::onKeyHome(bool down)
			{
				return true;
			}

			bool CThumbnailView::onKeyEnd(bool down)
			{
				return true;
			}

			CThumbnailItem* CThumbnailView::addItem()
			{
				CThumbnailItem* item = new CThumbnailItem(m_view, this, m_itemWidth, m_itemHeight);
				item->OnDown = BIND_LISTENER(&CThumbnailView::onItemDown, this);
				m_items.push_back(item);
				invalidate();
				return item;
			}

			void CThumbnailView::onItemDown(CBase* base)
			{
				for (CThumbnailItem* item : m_items)
				{
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
		}
	}
}