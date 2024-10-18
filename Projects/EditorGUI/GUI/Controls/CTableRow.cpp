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
#include "CTableRow.h"
#include "CTextContainer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTableRow::CTableRow(CBase *parent) :
				CBase(parent),
				m_numColumn(0)
			{
				for (int i = 0; i < MAX_COLUMN; i++)
					m_columns[i] = NULL;

				setHeight(20.0f);
			}

			CTableRow::~CTableRow()
			{

			}

			void CTableRow::setColumnCount(int count)
			{
				if (count == m_numColumn)
					return;

				if (count >= MAX_COLUMN)
					count = MAX_COLUMN;

				for (int i = 0; i < MAX_COLUMN; i++)
				{
					if (i < count)
					{
						if (!m_columns[i])
						{
							m_columns[i] = new CIconTextItem(this);
							m_columns[i]->dock(EPosition::Left);
						}
					}
					else if (m_columns[i])
					{
						m_columns[i]->remove();
						m_columns[i] = NULL;
					}

					m_numColumn = count;
				}
			}

			void CTableRow::setColumnWidth(int i, float width)
			{
				if (!m_columns[i])
					return;

				if (m_columns[i]->width() == width)
					return;

				m_columns[i]->setWidth(width);
			}

			void CTableRow::setCellText(int i, const std::wstring& string)
			{
				if (!m_columns[i])
					return;

				m_columns[i]->setLabel(string);
			}

			void CTableRow::setCellIcon(int i, ESystemIcon icon)
			{
				if (!m_columns[i])
					return;

				m_columns[i]->setIcon(icon);
			}

			void CTableRow::setCellText(int i, const std::wstring& string, ESystemIcon icon)
			{
				if (!m_columns[i])
					return;

				m_columns[i]->setLabel(string);
				m_columns[i]->setIcon(icon);
			}

			void CTableRow::setCellContents(int i, CBase* control, bool enableMouseInput)
			{
				if (!m_columns[i])
					return;

				control->setParent(m_columns[i]);
				m_columns[i]->setMouseInputEnabled(enableMouseInput);
			}

			void CTableRow::layout()
			{
				float x = 0.0f;

				for (int i = 0; i < m_numColumn; i++)
				{
					m_columns[i]->setPos(x, 0.0f);
					m_columns[i]->setHeight(height());

					x = x + m_columns[i]->width();
				}

				CBase::layout();
			}
		}
	}
}