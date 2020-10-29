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
				m_expanderSize(4),
				m_minSize(50.0f)
			{
				for (int x = 0; x < MAX_SPLITER_COL; x++)
				{
					for (int y = 0; y < MAX_SPLITER_ROW; y++)
					{
						m_control[x][y] = NULL;
					}
				}

				for (int x = 0; x < MAX_SPLITER_COL; x++)
				{
					m_colWidth[x] = 0.0f;
					m_adjustColWidth[x] = 0.0f;
				}

				for (int y = 0; y < MAX_SPLITER_ROW; y++)
				{
					m_rowHeight[y] = 0.0f;
					m_adjustRowHeight[y] = 0.0f;
				}
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

				float minWidth = m_col * m_minSize + m_expanderSize * (m_col - 1);
				float minHeight = m_row * m_minSize + m_expanderSize * (m_row - 1);

				bool save = false;

				// note: colwidth == 0, mean it is not yet init layout, we will calculate it
				if (spaceWidth > minWidth)
				{
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
							save = true;
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
				}

				// note: rowheight == 0, mean it is not yet init layout, we will calculate it
				if (spaceHeight > minHeight)
				{
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
							save = true;
						}
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

				fixForMinSize();

				if (save)
					saveUserExpectedSize();

				fixForUserExpected();
			}

			void CSplitter::fixForMinSize()
			{
				for (u32 i = 0; i < m_col; i++)
				{
					if (m_colWidth[i] < m_minSize)
					{
						float delta = m_minSize - m_colWidth[i];

						for (int j = (int)i - 1; j >= 0; j--)
						{
							if (m_colWidth[j] > delta + m_minSize)
							{
								m_colWidth[j] = m_colWidth[j] - delta;
								m_colWidth[i] = m_colWidth[i] + delta;
								break;
							}
						}
					}
				}

				for (u32 i = 0; i < m_row; i++)
				{
					if (m_rowHeight[i] < m_minSize)
					{
						float delta = m_minSize - m_rowHeight[i];

						for (int j = (int)i - 1; j >= 0; j--)
						{
							if (m_rowHeight[j] > delta + m_minSize)
							{
								m_rowHeight[j] = m_rowHeight[j] - delta;
								m_rowHeight[i] = m_rowHeight[i] + delta;
								break;
							}
						}
					}
				}
			}

			void CSplitter::saveUserExpectedSize()
			{
				for (int x = 0; x < MAX_SPLITER_COL; x++)
					m_adjustColWidth[x] = m_colWidth[x];

				for (int y = 0; y < MAX_SPLITER_ROW; y++)
					m_adjustRowHeight[y] = m_rowHeight[y];
			}

			void CSplitter::fixForUserExpected()
			{
				for (u32 i = 0; i < m_col; i++)
				{
					if (m_colWidth[i] < m_adjustColWidth[i])
					{
						float target = m_adjustColWidth[i] - m_colWidth[i];

						for (u32 j = i + 1; j < m_col; j++)
						{
							if (m_colWidth[j] > m_minSize)
							{
								float delta = m_colWidth[j] - m_minSize;
								if (delta > target)
									delta = target;

								m_colWidth[i] = m_colWidth[i] + delta;
								m_colWidth[j] = m_colWidth[j] - delta;

								target = target - delta;
								if (target <= 0.0f)
									break;
							}
						}
					}
				}

				for (u32 i = 0; i < m_row; i++)
				{
					if (m_rowHeight[i] < m_adjustRowHeight[i])
					{
						float target = m_adjustRowHeight[i] - m_rowHeight[i];

						for (u32 j = i + 1; j < m_row; j++)
						{
							if (m_rowHeight[j] > m_minSize)
							{
								float delta = m_rowHeight[j] - m_minSize;
								if (delta > target)
									delta = target;

								m_rowHeight[i] = m_rowHeight[i] + delta;
								m_rowHeight[j] = m_rowHeight[j] - delta;

								target = target - delta;
								if (target <= 0.0f)
									break;
							}
						}
					}
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
				CRenderer::getRenderer()->drawFillRect(getRenderBounds(), CThemeConfig::SpliterColor);
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