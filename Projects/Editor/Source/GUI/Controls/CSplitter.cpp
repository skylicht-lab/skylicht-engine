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
				m_expanderSize(2.0f),
				m_minSize(50.0f),
				m_pressed(false),
				m_dragRow(0),
				m_dragCol(0),
				m_lastSpaceWidth(0.0f),
				m_lastSpaceHeight(0.0f),
				m_weakCol(0),
				m_weakRow(0),
				m_firstTimeLayout(true),
				m_hitState(EHoverState::None)
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

				enableRenderFillRect(true);
				setFillRectColor(CThemeConfig::SpliterColor);
			}

			CSplitter::~CSplitter()
			{

			}

			void CSplitter::layout()
			{
				predictChildSize();

				if (m_firstTimeLayout == true)
				{
					saveUserExpectedSize();
					m_firstTimeLayout = false;
				}

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

				if (spaceWidth > 0.0f)
				{
					if (spaceWidth >= m_lastSpaceWidth)
					{
						predictChildWidth(spaceWidth);
						updateWidthLarger();
					}
					else
					{
						updateWidthSmaller(spaceWidth, m_lastSpaceWidth - spaceWidth);
					}
				}

				if (spaceHeight > 0.0f)
				{
					if (spaceHeight >= m_lastSpaceHeight)
					{
						predictChildHeight(spaceHeight);
						updateHeightLarger();
					}
					else
					{
						updateHeightSmaller(spaceHeight, m_lastSpaceHeight - spaceHeight);
					}
				}

				m_lastSpaceWidth = spaceWidth;
				m_lastSpaceHeight = spaceHeight;
			}

			void CSplitter::predictChildWidth(float spaceWidth)
			{
				int numColPredict = 0;
				float minWidth = m_col * m_minSize + m_expanderSize * (m_col - 1);

				// note: colwidth == 0, mean it is not yet init layout, we will calculate it
				for (u32 i = 0; i < m_col; i++)
				{
					if (m_colWidth[i] > 0.0f && spaceWidth > 0.0f)
					{
						spaceWidth = spaceWidth - m_colWidth[i];
					}
					else
					{
						// predict later
						m_colWidth[i] = 0.0f;
						numColPredict++;
					}
				}

				if (spaceWidth > 0)
				{
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
						u32 weakCol = m_weakCol;
						if (weakCol >= m_col)
							weakCol = m_col - 1;

						m_colWidth[weakCol] = m_colWidth[weakCol] + spaceWidth;
					}
				}
			}

			void CSplitter::predictChildHeight(float spaceHeight)
			{
				int numRowPredict = 0;
				float minHeight = m_row * m_minSize + m_expanderSize * (m_row - 1);

				// note: rowheight == 0, mean it is not yet init layout, we will calculate it
				for (u32 i = 0; i < m_row; i++)
				{
					if (m_rowHeight[i] > 0.0f && spaceHeight > 0.0f)
					{
						spaceHeight = spaceHeight - m_rowHeight[i];
					}
					else
					{
						// predict later
						m_rowHeight[i] = 0.0f;
						numRowPredict++;
					}
				}

				if (spaceHeight > 0)
				{
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
						u32 weakRow = m_weakRow;
						if (weakRow >= m_row)
							weakRow = m_row - 1;

						m_rowHeight[weakRow] = m_rowHeight[weakRow] + spaceHeight;
					}
				}
			}

			void CSplitter::getWeakPriorityRow(std::list<std::pair<u32, float>>& weakRowPriority, bool inverse)
			{
				std::list<std::pair<u32, float>>::iterator it, end;

				for (u32 i = 0; i < m_row; i++)
				{
					int d = (int)(m_weakRow - i);
					float distance = (float)(core::abs_(d));

					// right is weak
					if (i < m_weakRow)
						distance = distance + (float)m_row;

					if (weakRowPriority.size() == 0)
					{
						weakRowPriority.push_back(std::pair<u32, float>(i, distance));
					}
					else
					{
						// insert sort
						it = weakRowPriority.begin();
						end = weakRowPriority.end();
						bool insert = false;

						while (it != end)
						{
							float value = (*it).second;
							if (distance < value)
							{
								weakRowPriority.insert(it, std::pair<u32, float>(i, distance));
								insert = true;
							}
							++it;
						}

						if (insert == false)
							weakRowPriority.push_back(std::pair<u32, float>(i, distance));
					}
				}

				if (inverse == true)
				{
					std::list<std::pair<u32, float>> reverse;

					it = weakRowPriority.begin();
					end = weakRowPriority.end();
					while (it != end)
					{
						reverse.push_front((*it));
						++it;
					}

					weakRowPriority = reverse;
				}
			}

			void CSplitter::getWeakPriorityCol(std::list<std::pair<u32, float>>& weakColPriority, bool inverse)
			{
				std::list<std::pair<u32, float>>::iterator it, end;

				for (u32 i = 0; i < m_col; i++)
				{
					int d = (int)(m_weakCol - i);
					float distance = (float)(core::abs_(d));

					// right is weak
					if (i < m_weakCol)
						distance = distance + (float)m_col;

					if (weakColPriority.size() == 0)
					{
						weakColPriority.push_back(std::pair<u32, float>(i, distance));
					}
					else
					{
						// insert sort
						it = weakColPriority.begin();
						end = weakColPriority.end();
						bool insert = false;

						while (it != end)
						{
							float value = (*it).second;
							if (distance < value)
							{
								weakColPriority.insert(it, std::pair<u32, float>(i, distance));
								insert = true;
							}
							++it;
						}

						if (insert == false)
							weakColPriority.push_back(std::pair<u32, float>(i, distance));
					}
				}

				if (inverse == true)
				{
					std::list<std::pair<u32, float>> reverse;

					it = weakColPriority.begin();
					end = weakColPriority.end();
					while (it != end)
					{
						reverse.push_front((*it));
						++it;
					}

					weakColPriority = reverse;
				}
			}

			void CSplitter::updateWidthSmaller(float spaceWidth, float delta)
			{
				std::list<std::pair<u32, float>> weakColPriority;
				std::list<std::pair<u32, float>>::iterator it, end;

				getWeakPriorityCol(weakColPriority);

				it = weakColPriority.begin();
				end = weakColPriority.end();

				while (it != end)
				{
					u32 i = (*it).first;

					float w = m_colWidth[i] - delta;
					if (w < m_minSize)
						w = m_minSize;

					delta = delta - (m_colWidth[i] - w);

					m_colWidth[i] = w;

					if (delta <= 0.0f)
						return;

					++it;
				}
			}

			void CSplitter::updateHeightSmaller(float spaceHeight, float delta)
			{
				std::list<std::pair<u32, float>> weakRowPriority;
				std::list<std::pair<u32, float>>::iterator it, end;

				getWeakPriorityRow(weakRowPriority);

				it = weakRowPriority.begin();
				end = weakRowPriority.end();

				while (it != end)
				{
					u32 i = (*it).first;

					float h = m_rowHeight[i] - delta;
					if (h < m_minSize)
						h = m_minSize;

					delta = delta - (m_rowHeight[i] - h);

					m_rowHeight[i] = h;

					if (delta <= 0.0f)
						return;

					++it;
				}
			}

			void CSplitter::saveUserExpectedSize()
			{
				for (int x = 0; x < MAX_SPLITER_COL; x++)
					m_adjustColWidth[x] = m_colWidth[x];

				for (int y = 0; y < MAX_SPLITER_ROW; y++)
					m_adjustRowHeight[y] = m_rowHeight[y];
			}

			void CSplitter::updateWidthLarger()
			{
				std::list<std::pair<u32, float>> weakPriority;
				std::list<std::pair<u32, float>>::iterator it, end, test;

				getWeakPriorityCol(weakPriority, true);

				it = weakPriority.begin();
				end = weakPriority.end();

				while (it != end)
				{
					u32 i = (*it).first;

					if (m_colWidth[i] < m_adjustColWidth[i])
					{
						float target = m_adjustColWidth[i] - m_colWidth[i];

						test = it;
						++test;

						while (test != end)
						{
							u32 j = (*test).first;
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
							++test;
						}

						if (target > 0)
						{
							test = weakPriority.begin();
							while (test != it)
							{
								u32 j = (*test).first;
								if (m_colWidth[j] > m_adjustColWidth[j])
								{
									float delta = m_colWidth[j] - m_adjustColWidth[j];
									if (delta > target)
										delta = target;

									m_colWidth[i] = m_colWidth[i] + delta;
									m_colWidth[j] = m_colWidth[j] - delta;

									target = target - delta;
									if (target <= 0.0f)
										break;
								}
								++test;
							}
						}
					}
					++it;
				}
			}

			void CSplitter::updateHeightLarger()
			{
				std::list<std::pair<u32, float>> weakPriority;
				std::list<std::pair<u32, float>>::iterator it, end, test;

				getWeakPriorityRow(weakPriority, true);

				it = weakPriority.begin();
				end = weakPriority.end();

				while (it != end)
				{
					u32 i = (*it).first;

					if (m_rowHeight[i] < m_adjustRowHeight[i])
					{
						float target = m_adjustRowHeight[i] - m_rowHeight[i];

						test = it;
						++test;

						while (test != end)
						{
							u32 j = (*test).first;
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
							++test;
						}

						if (target > 0)
						{
							test = weakPriority.begin();
							while (test != it)
							{
								u32 j = (*test).first;
								if (m_rowHeight[j] > m_adjustRowHeight[j])
								{
									float delta = m_rowHeight[j] - m_adjustRowHeight[j];
									if (delta > target)
										delta = target;

									m_rowHeight[i] = m_rowHeight[i] + delta;
									m_rowHeight[j] = m_rowHeight[j] - delta;

									target = target - delta;
									if (target <= 0.0f)
										break;
								}
								++test;
							}
						}
					}

					++it;
				}
			}

			CBase* CSplitter::getControl(u32 row, u32 col)
			{
				if (row >= m_row || col >= m_col)
					return NULL;

				return m_control[col][row];
			}

			void CSplitter::setControl(CBase *base, u32 row, u32 col)
			{
				if (row >= m_row || col >= m_col)
					return;

				m_control[col][row] = base;

				if (base != NULL)
				{
					base->setParent(this);
					base->dock(EPosition::None);
				}

				invalidate();
			}

			bool CSplitter::getColRowFromControl(CBase *base, u32 &outRow, u32 &outCol)
			{
				for (u32 i = 0; i < m_col; i++)
				{
					for (u32 j = 0; j < m_row; j++)
					{
						if (m_control[i][j] == base)
						{
							outRow = j;
							outCol = i;
							return true;
						}
					}
				}

				return false;
			}

			void CSplitter::setNumberRowCol(u32 row, u32 col)
			{
				m_row = row;
				m_col = col;

				m_row = core::clamp<u32>(m_row, 1, MAX_SPLITER_ROW);
				m_col = core::clamp<u32>(m_col, 1, MAX_SPLITER_COL);

				m_lastSpaceWidth = 0.0f;
				m_lastSpaceHeight = 0.0f;

				for (int x = 0; x < MAX_SPLITER_COL; x++)
					m_adjustColWidth[x] = 0.0f;

				for (int y = 0; y < MAX_SPLITER_ROW; y++)
					m_adjustRowHeight[y] = 0.0f;

				invalidate();
			}

			void CSplitter::insertCol(u32 position)
			{
				setNumberRowCol(m_row, m_col + 1);

				for (u32 col = m_col; col > position; col--)
				{
					for (u32 row = 0; row < m_row; row++)
					{
						m_control[col][row] = m_control[col - 1][row];
					}

					m_colWidth[col] = m_colWidth[col - 1];
				}

				// init col that inserted
				for (u32 row = 0; row < m_row; row++)
				{
					m_control[position][row] = NULL;
					m_colWidth[position] = 0.0f;
				}
			}

			void CSplitter::insertRow(u32 position)
			{
				setNumberRowCol(m_row + 1, m_col);

				for (u32 row = m_row; row > position; row--)
				{
					for (u32 col = 0; col < m_col; col++)
					{
						m_control[col][row] = m_control[col][row - 1];
					}

					m_rowHeight[row] = m_rowHeight[row - 1];
				}

				// init row that inserted
				for (u32 col = 0; col < m_col; col++)
				{
					m_control[col][position] = NULL;
					m_rowHeight[position] = 0.0f;
				}
			}

			void CSplitter::removeCol(u32 position)
			{
				if (position > m_col)
					return;

				setNumberRowCol(m_col - 1, m_row);

				for (u32 col = position; col < m_col; col++)
				{
					for (u32 row = 0; row < m_row; row++)
					{
						m_control[col][row] = m_control[col + 1][row];
					}

					m_colWidth[col] = m_colWidth[col + 1];
				}

				// erase last col that removed
				for (u32 row = 0; row < m_row; row++)
				{
					m_control[m_col][row] = NULL;
					m_colWidth[m_col] = 0.0f;
				}
			}

			void CSplitter::removeRow(u32 position)
			{
				if (position > m_row)
					return;

				setNumberRowCol(m_col, m_row - 1);

				for (u32 row = position; row < m_row; row++)
				{
					for (u32 col = 0; col < m_col; col++)
					{
						m_control[col][row] = m_control[col][row + 1];
					}

					m_rowHeight[row] = m_rowHeight[row + 1];
				}

				// erase last row that removed
				for (u32 col = 0; col < m_col; col++)
				{
					m_control[col][m_row] = NULL;
					m_rowHeight[m_row] = 0.0f;
				}
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

			void CSplitter::onMouseMoved(float x, float y, float deltaX, float deltaY)
			{
				if (m_disabled)
					return;

				SPoint mousePoint(x, y);
				mousePoint = canvasPosToLocal(mousePoint);

				EHoverState state = None;

				if (m_pressed == false)
				{
					// just test hover to update cursor icon
					u32 hitRow, hitCol;
					state = mouseHittest(mousePoint.X, mousePoint.Y, hitRow, hitCol);
				}
				else
				{
					// update drag size
					state = m_hitState;

					SRect dragBound = getCellRect(m_dragRow, m_dragCol, m_dragRow + 1, m_dragCol + 1);

					float maxWidth = dragBound.Width - m_expanderSize - m_minSize;
					float maxHeight = dragBound.Height - m_expanderSize - m_minSize;

					bool save = false;

					if (state & CSplitter::Row)
					{
						float h1 = mousePoint.Y - dragBound.Y;
						if (h1 < m_minSize)
							h1 = m_minSize;
						if (h1 > maxHeight)
							h1 = maxHeight;

						float h2 = dragBound.Height - h1 - m_expanderSize;
						m_rowHeight[m_dragRow] = h1;
						m_rowHeight[m_dragRow + 1] = h2;

						save = true;
					}

					if (state & CSplitter::Col)
					{
						float w1 = mousePoint.X - dragBound.X;
						if (w1 < m_minSize)
							w1 = m_minSize;
						if (w1 > maxWidth)
							w1 = maxWidth;

						float w2 = dragBound.Width - w1 - m_expanderSize;
						m_colWidth[m_dragCol] = w1;
						m_colWidth[m_dragCol + 1] = w2;

						save = true;
					}

					if (save)
					{
						invalidate();
						saveUserExpectedSize();
					}
				}

				switch (state)
				{
				case CSplitter::None:
					setCursor(GUI::ECursorType::Normal);
					break;
				case CSplitter::Row:
					setCursor(GUI::ECursorType::SizeNS);
					break;
				case CSplitter::Col:
					setCursor(GUI::ECursorType::SizeWE);
					break;
				case CSplitter::RowAndCol:
					setCursor(GUI::ECursorType::SizeAll);
					break;
				default:
					break;
				}
			}

			void CSplitter::onMouseClickLeft(float x, float y, bool down)
			{
				if (m_disabled)
					return;

				SPoint mousePoint(x, y);
				mousePoint = canvasPosToLocal(mousePoint);

				m_pressed = down;
				m_hitState = mouseHittest(mousePoint.X, mousePoint.Y, m_dragRow, m_dragCol);

				if (m_pressed)
					CInput::getInput()->setCapture(this);
				else
					CInput::getInput()->setCapture(NULL);
			}

			CSplitter::EHoverState CSplitter::mouseHittest(float x, float y, u32& outRow, u32 &outCol)
			{
				bool isHoverH = false;
				bool isHoverV = false;
				float current = 0.0f;

				for (u32 i = 0; i < m_col; i++)
				{
					current += m_colWidth[i];

					if (x >= current && x <= current + m_expanderSize)
					{
						isHoverH = true;
						outCol = i;
						break;
					}

					current += m_expanderSize;
				}

				current = 0.0f;

				for (u32 i = 0; i < m_row; i++)
				{
					current += m_rowHeight[i];

					if (y >= current && y <= current + m_expanderSize)
					{
						isHoverV = true;
						outRow = i;
						break;
					}

					current += m_expanderSize;
				}


				if (isHoverH && isHoverV)
					return CSplitter::RowAndCol;

				if (isHoverH)
					return CSplitter::Col;

				if (isHoverV)
					return CSplitter::Row;

				return CSplitter::None;
			}

			SRect CSplitter::getCellRect(u32 fromRow, u32 fromCol, u32 toRow, u32 toCol)
			{
				SRect result;

				float x = 0.0f;
				float y = 0.0f;
				float w = 0.0f;
				float h = 0.0f;

				for (u32 i = 0; i < m_col; i++)
				{
					if (i == fromCol)
						result.X = x;

					if (i >= fromCol)
						w = w + m_colWidth[i] + m_expanderSize;

					x = x + m_colWidth[i] + m_expanderSize;

					if (i == toCol)
					{
						w = w - m_expanderSize;
						result.Width = w;
						break;
					}
				}

				for (u32 i = 0; i < m_row; i++)
				{
					if (i == fromRow)
						result.Y = y;

					if (i >= fromRow)
						h = h + m_rowHeight[i] + m_expanderSize;

					y = y + m_rowHeight[i] + m_expanderSize;

					if (i == toRow)
					{
						h = h - m_expanderSize;
						result.Height = h;
						break;
					}
				}

				return result;
			}
		}
	}
}