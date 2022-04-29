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
#include "CDataGridView.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CDataGridView::CDataGridView(CBase* parent, int numColumn) :
				CBase(parent),
				m_numColumn(numColumn),
				m_resizeWidth(3.0f)
			{
				if (m_numColumn >= GRIDVIEW_MAX_COLUMN)
					m_numColumn = GRIDVIEW_MAX_COLUMN - 1;

				for (int i = 0; i < GRIDVIEW_MAX_COLUMN; i++)
					m_width[i] = -1.0f;

				m_header = new CDataHeader(this);
				m_view = new CScrollControl(this);
				m_view->dock(EPosition::Fill);

				setKeyboardInputEnabled(true);
			}

			CDataGridView::~CDataGridView()
			{

			}

			void CDataGridView::layout()
			{
				CBase::layout();

				float totalWidth = width();

				float size = totalWidth;
				int c = 0;

				for (int i = 0; i < m_numColumn; i++)
				{
					if (m_width[i] > 0)
						size = size - m_width[i];
					else
						c++;
				}

				float colSize = 0.0f;
				if (c > 0 && size > 0.0f)
					colSize = (float)round(size / (float)c);

				for (int i = 0; i < m_numColumn; i++)
				{
					if (m_width[i] <= 0)
						m_width[i] = colSize;
				}

				int id = 0;
				for (CDataRowView* row : m_items)
					row->setRowID(id++);
			}

			void CDataGridView::postLayout()
			{
				CBase::postLayout();

				float w = m_view->getInnerWidth();
				for (CBase* child : m_view->getInnerPanel()->Children)
				{
					if (child->width() < w)
						child->setWidth(w);
				}
			}

			void CDataGridView::unSelectAll()
			{

			}

			CDataRowView* CDataGridView::addItem(const wchar_t* label, ESystemIcon icon)
			{
				CDataRowView* item = new CDataRowView(m_view, this, label, icon);
				m_items.push_back(item);

				item->dock(EPosition::Top);
				item->OnDown = BIND_LISTENER(&CDataGridView::onItemDown, this);
				return item;
			}

			void CDataGridView::removeItem(CDataRowView* row)
			{
				std::vector<CDataRowView*>::iterator i = std::find(m_items.begin(), m_items.end(), row);
				if (i != m_items.end())
				{
					m_items.erase(i);
					row->remove();
				}
			}

			void CDataGridView::removeAllItem()
			{
				for (CDataRowView* row : m_items)
				{
					row->remove();
				}

				m_items.clear();
			}

			void CDataGridView::setColumnWidth(int c, float w)
			{
				m_width[c] = w;
				invalidate();
			}

			bool CDataGridView::onKeyUp(bool down)
			{
				return true;
			}

			bool CDataGridView::onKeyDown(bool down)
			{
				return true;
			}

			bool CDataGridView::onKeyHome(bool down)
			{
				return true;
			}

			bool CDataGridView::onKeyEnd(bool down)
			{
				return true;
			}

			void CDataGridView::onItemDown(CBase* base)
			{
				for (CDataRowView* item : m_items)
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