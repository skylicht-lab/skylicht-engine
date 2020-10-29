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
#include "CSplitter.h"
#include "GUI/Input/CInput.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CSplitter::CSplitter(CBase* parent) :
				CBase(parent),
				m_row(1),
				m_col(1),
				m_expanderSize(4)
			{
				for (int x = 0; x < MAX_SPLITER_COL; x++)
				{
					for (int y = 0; y < MAX_SPLITER_ROW; y++)
					{
						m_control[x][y] = NULL;
					}
				}

				for (int x = 0; x < MAX_SPLITER_COL; x++)
					m_colWidth[x] = 0.0f;

				for (int y = 0; y < MAX_SPLITER_ROW; y++)
					m_rowHeight[y] = 0.0f;
			}

			CSplitter::~CSplitter()
			{

			}

			void CSplitter::layout()
			{
				predictChildSize();

				float x = 0.0f;
				float y = 0.0f;
				float w = 0.0f;
				float h = 0.0f;

				for (u32 i = 0; i < m_col; i++)
				{
					w = m_colWidth[i];

					for (u32 j = 0; j < m_row; j++)
					{
						h = m_rowHeight[j];

						if (m_control[i][j] != NULL)
						{
							m_control[i][j]->setBounds(SRect(x, y, w, h));
						}

						y = y + h + m_expanderSize;
					}

					x = x + w + m_expanderSize;
					y = 0.0f;
				}
			}

			void CSplitter::predictChildSize()
			{
				SRect bound = getRenderBounds();

				float spaceWidth = bound.Width - m_expanderSize * (m_col - 1);
				float spaceHeight = bound.Height - m_expanderSize * (m_row - 1);

				int numColPredict = 0;
				int numRowPredict = 0;

				for (u32 i = 0; i < m_col; i++)
				{
					if (m_colWidth[i] > 0.0f && spaceWidth > 0.0f)
					{
						if (m_colWidth[i] > spaceWidth)
							m_colWidth[i] = spaceWidth;

						spaceWidth = spaceWidth - m_colWidth[i];
					}
					else
					{
						// predict later
						m_colWidth[i] = 0.0f;
						numColPredict++;
					}
				}

				for (u32 i = 0; i < m_row; i++)
				{
					if (m_rowHeight[i] > 0.0f && spaceHeight > 0.0f)
					{
						if (m_rowHeight[i] > spaceHeight)
							m_rowHeight[i] = spaceHeight;

						spaceHeight = spaceHeight - m_rowHeight[i];
					}
					else
					{
						// predict later
						m_rowHeight[i] = 0.0f;
						numRowPredict++;
					}
				}

				if (numColPredict > 0)
				{
					float avgColSize = spaceWidth / (float)numColPredict;
					for (u32 i = 0; i < m_col; i++)
					{
						if (m_colWidth[i] <= 0.0f)
							m_colWidth[i] = avgColSize;
					}
				}
				else
				{
					if (spaceWidth > 0)
						m_colWidth[m_col - 1] = m_colWidth[m_col - 1] + spaceWidth;
				}

				if (numRowPredict > 0)
				{
					float avgRowSize = spaceHeight / (float)numRowPredict;
					for (u32 i = 0; i < m_row; i++)
					{
						if (m_rowHeight[i] <= 0.0f)
							m_rowHeight[i] = avgRowSize;
					}
				}
				else
				{
					if (spaceHeight > 0)
						m_rowHeight[m_row - 1] = m_rowHeight[m_row - 1] + spaceHeight;
				}
			}

			void CSplitter::setControl(CBase *base, u32 row, u32 col)
			{
				if (row >= m_row || col >= m_col || m_control[col][row] != NULL)
					return;

				m_control[col][row] = base;

				if (base != NULL)
				{
					base->setParent(this);
					base->dock(EPosition::None);
				}

				invalidate();
			}

			void CSplitter::setNumberRowCol(u32 row, u32 col)
			{
				m_row = row;
				m_col = col;

				m_row = core::clamp<u32>(m_row, 1, MAX_SPLITER_ROW);
				m_col = core::clamp<u32>(m_col, 1, MAX_SPLITER_COL);

				invalidate();
			}

			void CSplitter::setRowHeight(u32 row, float height)
			{
				if (row < m_row)
					m_rowHeight[row] = height;

				invalidate();
			}

			void CSplitter::setColWidth(u32 col, float width)
			{
				if (col < m_col)
					m_colWidth[col] = width;

				invalidate();
			}

			void CSplitter::renderUnder()
			{
				CRenderer::getRenderer()->drawFillRect(getRenderBounds(), CThemeConfig::WindowBackgroundColor);
			}

			void CSplitter::onMouseMoved(float x, float y, float deltaX, float deltaY)
			{
				if (m_disabled)
					return;
			}

			void CSplitter::onMouseClickLeft(float x, float y, bool bDown)
			{
				if (m_disabled)
					return;
			}
		}
	}
}