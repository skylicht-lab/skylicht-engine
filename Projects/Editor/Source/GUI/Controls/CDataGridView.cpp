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

			void CDataGridView::removeAllItem()
			{

			}

			void CDataGridView::setColumnWidth(CBase* control, int c, float w)
			{
				m_width[c] = w;

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

			void CDataGridView::onItemDown(CBase* item)
			{

			}
		}
	}
}