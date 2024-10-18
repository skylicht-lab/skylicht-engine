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
#include "CDockPanel.h"
#include "CDockableWindow.h"
#include "CDockTabControl.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/CGUIContext.h"
#include "GUI/Input/CInput.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CDockPanel *g_rootDockPanel = NULL;

			void CDockPanel::setRootPanel(CDockPanel *panel)
			{
				g_rootDockPanel = panel;
			}

			CDockPanel* CDockPanel::getRootPanel()
			{
				return g_rootDockPanel;
			}

			CDockPanel::CDockPanel(CBase *parent) :
				CBase(parent),
				m_hoverHint(NULL),
				m_dragOverWindow(NULL)
			{
				m_mainSpliter = new CSplitter(this);
				m_mainSpliter->dock(EPosition::Fill);

				// auto set root
				if (g_rootDockPanel == NULL)
					g_rootDockPanel = this;

				// if this is not a canvas
				if (!isRoot())
					m_innerPanel = new CBase(m_mainSpliter);

				m_mainSpliter->setControl(m_innerPanel, 0, 0);

				m_hintWindow = new CBase(this);
				m_hintWindow->enableRenderFillRect(true);
				m_hintWindow->setFillRectColor(CThemeConfig::DockHintWindowColor);

				m_dockHint[0] = new CDockHintIcon(this, EDockHintIcon::Center);

				m_dockHint[1] = new CDockHintIcon(this, EDockHintIcon::Left);
				m_dockHint[2] = new CDockHintIcon(this, EDockHintIcon::Right);
				m_dockHint[3] = new CDockHintIcon(this, EDockHintIcon::Top);
				m_dockHint[4] = new CDockHintIcon(this, EDockHintIcon::Bottom);

				m_dockHint[5] = new CDockHintIcon(this, EDockHintIcon::TargetLeft);
				m_dockHint[6] = new CDockHintIcon(this, EDockHintIcon::TargetRight);
				m_dockHint[7] = new CDockHintIcon(this, EDockHintIcon::TargetTop);
				m_dockHint[8] = new CDockHintIcon(this, EDockHintIcon::TargetBottom);

				hideDockHint();
			}

			CDockPanel::~CDockPanel()
			{
				if (g_rootDockPanel == this)
					g_rootDockPanel = NULL;
			}

			bool CDockPanel::isRoot()
			{
				return this == g_rootDockPanel;
			}

			void CDockPanel::layout()
			{
				CBase::layout();
				layoutHintIcon();
			}

			void CDockPanel::layoutHintIcon()
			{
				SPoint center, spaceCenter, rootCenter;

				CSplitter *outSpliter;

				SRect rootBound = getSpaceBounds(outSpliter);
				spaceCenter = SPoint(
					round(rootBound.X + rootBound.Width * 0.5f),
					round(rootBound.Y + rootBound.Height * 0.5f)
				);

				rootCenter = SPoint(
					round(m_bounds.X + m_bounds.Width * 0.5f),
					round(m_bounds.Y + m_bounds.Height * 0.5f)
				);

				center = rootCenter;

				if (m_dragOverWindow != NULL)
				{
					SRect winBound = m_dragOverWindow->getBounds();

					SPoint canvasPos = m_dragOverWindow->localPosToCanvas();
					SPoint panelPos = canvasPosToLocal(canvasPos);

					center = SPoint(
						round(panelPos.X + winBound.Width * 0.5f),
						round(panelPos.Y + winBound.Height * 0.5f)
					);
				}

				float w = m_dockHint[0]->width();
				float h = m_dockHint[0]->height();

				float halfW = round(w * 0.5f);
				float halfH = round(h * 0.5f);

				float padding = 0.0f;

				if (m_dragOverWindow == NULL)
				{
					padding = 20.0f;
					m_dockHint[0]->setPos(spaceCenter.X - halfW, spaceCenter.Y - halfH);

					m_dockHint[1]->setPos(padding, rootCenter.Y - halfH);
					m_dockHint[2]->setPos(width() - padding - w, rootCenter.Y - halfH);
					m_dockHint[3]->setPos(rootCenter.X - halfW, padding);
					m_dockHint[4]->setPos(rootCenter.X - halfW, height() - padding - h);
				}
				else
				{
					padding = 50.0f;
					m_dockHint[0]->setPos(center.X - halfW, center.Y - halfH);

					m_dockHint[5]->setPos(center.X - halfW - padding, center.Y - halfH);
					m_dockHint[6]->setPos(center.X - halfW + padding, center.Y - halfH);
					m_dockHint[7]->setPos(center.X - halfW, center.Y - halfH - padding);
					m_dockHint[8]->setPos(center.X - halfW, center.Y - halfH + padding);

					padding = 20.0f;
					m_dockHint[1]->setPos(padding, rootCenter.Y - halfH);
					m_dockHint[2]->setPos(width() - padding - w, rootCenter.Y - halfH);
					m_dockHint[3]->setPos(rootCenter.X - halfW, padding);
					m_dockHint[4]->setPos(rootCenter.X - halfW, height() - padding - h);
				}
			}

			void CDockPanel::showDockHintWindow(CDockHintIcon *hint, CDockableWindow *window)
			{
				if (m_hoverHint != NULL && m_hoverHint != hint)
					m_hoverHint->setColor(CThemeConfig::DefaultIconColor);

				m_hoverHint = hint;
				m_hoverHint->setColor(CThemeConfig::ButtonPressColor);

				m_hintWindow->bringNextToControl(m_dockHint[0], false);
				m_hintWindow->setHidden(false);

				CSplitter *outSpliter = NULL;

				SRect spaceBounds;
				if (m_dragOverWindow == NULL)
					spaceBounds = getSpaceBounds(outSpliter);
				else
				{
					spaceBounds = m_dragOverWindow->getBounds();
					SPoint p = m_dragOverWindow->localPosToCanvas();
					p = canvasPosToLocal(p);
					spaceBounds.X = p.X;
					spaceBounds.Y = p.Y;
				}

				SRect fullBound = getRenderBounds();

				float maxWidth = round(spaceBounds.Width * 0.5f);
				float maxHeight = round(spaceBounds.Height * 0.5f);

				EDockHintIcon icon = hint->getIcon();
				if (icon == Left || icon == Right || icon == Top || icon == Bottom)
				{
					maxWidth = round(fullBound.Width * 0.5f);
					maxHeight = round(fullBound.Height * 0.5f);
				}

				SRect bound = window->getBounds();
				if (bound.Width >= maxWidth)
					bound.Width = maxWidth;

				if (bound.Height > maxHeight)
					bound.Height = maxHeight;

				switch (icon)
				{
				case Center:
					m_hintWindow->setBounds(spaceBounds.X, spaceBounds.Y, spaceBounds.Width, spaceBounds.Height);
					break;
				case Left:
					m_hintWindow->setBounds(0.0f, 0.0f, bound.Width, height());
					break;
				case TargetLeft:
					m_hintWindow->setBounds(spaceBounds.X, spaceBounds.Y, bound.Width, spaceBounds.Height);
					break;
				case Right:
					m_hintWindow->setBounds(width() - bound.Width, 0.0f, bound.Width, height());
					break;
				case TargetRight:
					m_hintWindow->setBounds(spaceBounds.X + spaceBounds.Width - bound.Width, spaceBounds.Y, bound.Width, spaceBounds.Height);
					break;
				case Top:
					m_hintWindow->setBounds(0.0f, 0.0f, width(), bound.Height);
					break;
				case TargetTop:
					m_hintWindow->setBounds(spaceBounds.X, spaceBounds.Y, spaceBounds.Width, bound.Height);
					break;
				case Bottom:
					m_hintWindow->setBounds(0.0f, height() - bound.Height, width(), bound.Height);
					break;
				case TargetBottom:
					m_hintWindow->setBounds(spaceBounds.X, spaceBounds.Y + spaceBounds.Height - bound.Height, spaceBounds.Width, bound.Height);
					break;
				default:
					break;
				}
			}

			SRect CDockPanel::getSpaceBounds(CSplitter* &outSpliter)
			{
				return getSpaceBounds(m_mainSpliter, 0.0f, 0.0f, outSpliter);
			}

			SRect CDockPanel::getSpaceBounds(CSplitter *spliter, float x, float y, CSplitter* &outSpliter)
			{
				u32 weakRow = spliter->getWeakRow();
				u32 weakCol = spliter->getWeakCol();

				SRect result = spliter->getCellRect(weakRow, weakCol, weakRow, weakCol);
				result.X = x + result.X;
				result.Y = y + result.Y;

				CBase *base = spliter->getControl(weakRow, weakCol);
				if (base != NULL)
				{
					CSplitter *childSpliter = dynamic_cast<CSplitter*>(base);
					if (childSpliter != NULL)
						return getSpaceBounds(childSpliter, x + childSpliter->X(), y + childSpliter->Y(), outSpliter);
				}

				outSpliter = spliter;
				return result;
			}

			void CDockPanel::unDockChildWindow(CDockTabControl *win)
			{
				CSplitter *spliter = dynamic_cast<CSplitter*>(win->getParent());
				if (spliter != NULL)
				{
					bool finish = false;
					CBase *base = win;

					do
					{
						u32 row, col;
						if (spliter->getColRowFromControl(base, row, col) == true)
						{
							if (spliter->isHorizontal() == true)
							{
								spliter->removeCol(col);
							}
							else
							{
								spliter->removeRow(row);
							}

							base->remove();
						}

						if (spliter->isEmpty() && spliter != m_mainSpliter)
						{
							// continue remove empty spliter
							CSplitter *parentSpliter = dynamic_cast<CSplitter*>(spliter->getParent());
							if (parentSpliter != NULL)
							{
								base = spliter;
								spliter = parentSpliter;
							}
							else
							{
								finish = true;
							}
						}
						else
						{
							finish = true;
						}
					} while (finish == false);

					mergeSpliter(m_mainSpliter);
				}
			}

			void CDockPanel::mergeSpliter(CSplitter *spliter)
			{
				if (spliter->isHorizontal())
				{
					bool finish = true;
					do
					{
						finish = true;
						for (u32 col = 0, numCol = spliter->getNumCol(); col < numCol; col++)
						{
							CSplitter *child = dynamic_cast<CSplitter*>(spliter->getControl(0, col));
							if (child != NULL)
							{
								mergeSpliter(child);

								if (child->isHorizontal())
								{
									// merge to parent
									for (int i = 0, n = child->getNumCol(); i < n; i++)
									{
										CBase *control = child->getControl(0, i);
										spliter->insertCol(col + i + 1);
										spliter->setControl(control, 0, col + i + 1);
										spliter->setColWidth(col + i + 1, child->getColWidth(i));
									}

									// remove this col, that merged
									child->remove();
									spliter->removeCol(col);

									finish = false;
									break;
								}
							}
						}
					} while (finish == false);
				}
				else
				{
					bool finish = true;
					do
					{
						finish = true;
						for (u32 row = 0, numRow = spliter->getNumCol(); row < numRow; row++)
						{
							CSplitter *child = dynamic_cast<CSplitter*>(spliter->getControl(row, 0));
							if (child != NULL)
							{
								mergeSpliter(child);

								if (child->isVertical())
								{
									// merge to parent
									for (int i = 0, n = child->getNumRow(); i < n; i++)
									{
										CBase *control = child->getControl(i, 0);
										spliter->insertRow(row + i + 1);
										spliter->setControl(control, row + i + 1, 0);
										spliter->setRowHeight(row + i + 1, child->getRowHeight(i));
									}

									// remove this row, that merged
									child->remove();
									spliter->removeRow(row);

									finish = false;
									break;
								}
							}
						}
					} while (finish == false);
				}
			}

			void CDockPanel::dockChildWindow(CDockableWindow *window, CDockTabControl *neighborhood, EDock dock)
			{
				CSplitter *mainSpliter = NULL;
				CSplitter *outSpliter = NULL;

				SRect spaceBounds;

				if (neighborhood == NULL ||
					dock == EDock::DockLeft ||
					dock == EDock::DockRight ||
					dock == EDock::DockBottom ||
					dock == EDock::DockTop)
				{
					spaceBounds = getSpaceBounds(outSpliter);
					mainSpliter = m_mainSpliter;
				}
				else
				{
					spaceBounds = neighborhood->getBounds();
					SPoint p = neighborhood->localPosToCanvas();
					p = canvasPosToLocal(p);
					spaceBounds.X = p.X;
					spaceBounds.Y = p.Y;

					mainSpliter = (CSplitter*)neighborhood->getParent();
				}

				float maxWidth = round(spaceBounds.Width * 0.5f);
				float maxHeight = round(spaceBounds.Height * 0.5f);

				SRect bound = window->getBounds();
				if (bound.Width >= maxWidth)
					bound.Width = maxWidth;

				if (bound.Height > maxHeight)
					bound.Height = maxHeight;

				u32 numRow = mainSpliter->getNumRow();
				u32 numCol = mainSpliter->getNumCol();

				u32 weakRow = mainSpliter->getWeakRow();
				u32 weakCol = mainSpliter->getWeakCol();

				if (dock == EDock::DockRight)
				{
					if (mainSpliter->isHorizontal())
					{
						numCol++;
						u32 rowID = numRow - 1;
						u32 colID = numCol - 1;

						mainSpliter->setNumberRowCol(numRow, numCol);

						CDockTabControl *tabControl = new CDockTabControl(mainSpliter, this);
						tabControl->dockWindow(window);

						mainSpliter->setControl(tabControl, rowID, colID);
						mainSpliter->setColWidth(colID, bound.Width);
					}
					else
					{
						CBase *base = m_innerPanel;
						m_innerPanel = NULL;

						CSplitter *newSpliter = new CSplitter(this);
						newSpliter->dock(EPosition::Fill);
						newSpliter->setNumberRowCol(1, 2);

						newSpliter->setControl(mainSpliter, 0, 0);

						CDockTabControl *tabControl = new CDockTabControl(newSpliter, this);
						tabControl->dockWindow(window);
						newSpliter->setControl(tabControl, 0, 1);

						newSpliter->setColWidth(0, 0.0f);
						newSpliter->setColWidth(1, bound.Width);

						weakRow = 0;
						weakCol = 0;

						mainSpliter = newSpliter;
						m_mainSpliter = newSpliter;
						m_innerPanel = base;
					}
				}
				else if (dock == EDock::DockTargetRight)
				{
					if (neighborhood != NULL)
					{
						u32 row, col;
						if (mainSpliter->getColRowFromControl(neighborhood, row, col) == true)
						{
							if (mainSpliter->isHorizontal() == true)
							{
								numCol++;
								u32 rowID = row;
								u32 colID = col + 1;

								mainSpliter->insertCol(col + 1);

								CDockTabControl *tabControl = new CDockTabControl(mainSpliter, this);
								tabControl->dockWindow(window);

								mainSpliter->setControl(tabControl, rowID, colID);
								mainSpliter->setColWidth(colID, bound.Width);

								if (mainSpliter->getNumCol() == 2)
									mainSpliter->setColWidth(colID - 1, 0.0f);

								if (weakCol > col)
									weakCol++;
							}
							else
							{

								CBase *base = m_innerPanel;
								m_innerPanel = NULL;

								CSplitter *newSpliter = new CSplitter(mainSpliter);
								newSpliter->setNumberRowCol(1, 2);
								newSpliter->setControl(neighborhood, 0, 0);

								CDockTabControl *tabControl = new CDockTabControl(newSpliter, this);
								tabControl->dockWindow(window);
								newSpliter->setControl(tabControl, 0, 1);

								newSpliter->setColWidth(0, 0.0f);
								newSpliter->setColWidth(1, bound.Width);

								// replace
								mainSpliter->setControl(newSpliter, row, col);

								mainSpliter = newSpliter;
								m_innerPanel = base;
							}
						}
					}
				}
				else if (dock == EDock::DockLeft)
				{
					if (mainSpliter->isHorizontal())
					{
						numCol++;
						u32 rowID = numRow - 1;
						u32 colID = 0;

						mainSpliter->insertCol(0);

						CDockTabControl *tabControl = new CDockTabControl(mainSpliter, this);
						tabControl->dockWindow(window);

						mainSpliter->setControl(tabControl, rowID, colID);
						mainSpliter->setColWidth(colID, bound.Width);

						weakCol++;
					}
					else
					{
						CBase *base = m_innerPanel;
						m_innerPanel = NULL;

						CSplitter *newSpliter = new CSplitter(this);
						newSpliter->dock(EPosition::Fill);
						newSpliter->setNumberRowCol(1, 2);

						CDockTabControl *tabControl = new CDockTabControl(newSpliter, this);
						tabControl->dockWindow(window);

						newSpliter->setControl(tabControl, 0, 0);
						newSpliter->setControl(mainSpliter, 0, 1);

						newSpliter->setColWidth(0, bound.Width);
						newSpliter->setColWidth(1, 0.0f);

						weakRow = 0;
						weakCol = 1;

						mainSpliter = newSpliter;
						m_mainSpliter = newSpliter;
						m_innerPanel = base;
					}
				}
				else if (dock == EDock::DockTargetLeft)
				{
					if (neighborhood != NULL)
					{
						u32 row, col;
						if (mainSpliter->getColRowFromControl(neighborhood, row, col) == true)
						{
							if (mainSpliter->isHorizontal() == true)
							{
								numCol++;
								u32 rowID = row;
								u32 colID = col;

								mainSpliter->insertCol(col);

								CDockTabControl *tabControl = new CDockTabControl(mainSpliter, this);
								tabControl->dockWindow(window);

								mainSpliter->setControl(tabControl, rowID, colID);
								mainSpliter->setColWidth(colID, bound.Width);

								if (mainSpliter->getNumCol() == 2)
									mainSpliter->setColWidth(colID + 1, 0.0f);

								if (weakCol > col)
									weakCol++;
							}
							else
							{
								CBase *base = m_innerPanel;
								m_innerPanel = NULL;

								CSplitter *newSpliter = new CSplitter(mainSpliter);
								newSpliter->setNumberRowCol(1, 2);

								CDockTabControl *tabControl = new CDockTabControl(newSpliter, this);
								tabControl->dockWindow(window);
								newSpliter->setControl(tabControl, 0, 0);

								newSpliter->setControl(neighborhood, 0, 1);

								newSpliter->setColWidth(0, bound.Width);
								newSpliter->setColWidth(1, 0.0f);

								// replace
								mainSpliter->setControl(newSpliter, row, col);

								mainSpliter = newSpliter;
								m_innerPanel = base;
							}
						}
					}
				}
				else if (dock == EDock::DockTop)
				{
					if (mainSpliter->isVertical())
					{
						numRow++;
						u32 rowID = 0;
						u32 colID = numCol - 1;

						mainSpliter->insertRow(0);

						CDockTabControl *tabControl = new CDockTabControl(mainSpliter, this);
						tabControl->dockWindow(window);

						mainSpliter->setControl(tabControl, rowID, colID);
						mainSpliter->setRowHeight(rowID, bound.Height);

						weakRow++;
					}
					else
					{
						CBase *base = m_innerPanel;
						m_innerPanel = NULL;

						CSplitter *newSpliter = new CSplitter(this);
						newSpliter->dock(EPosition::Fill);
						newSpliter->setNumberRowCol(2, 1);

						CDockTabControl *tabControl = new CDockTabControl(newSpliter, this);
						tabControl->dockWindow(window);

						newSpliter->setControl(tabControl, 0, 0);
						newSpliter->setControl(mainSpliter, 1, 0);

						newSpliter->setRowHeight(0, bound.Height);
						newSpliter->setRowHeight(1, 0.0f);

						weakRow = 1;
						weakCol = 0;

						mainSpliter = newSpliter;
						m_mainSpliter = newSpliter;
						m_innerPanel = base;
					}
				}
				else if (dock == EDock::DockTargetTop)
				{
					if (neighborhood != NULL)
					{
						u32 row, col;
						if (mainSpliter->getColRowFromControl(neighborhood, row, col) == true)
						{
							if (mainSpliter->isVertical() == true)
							{
								numRow++;
								u32 rowID = row;
								u32 colID = col;

								mainSpliter->insertRow(row);

								CDockTabControl *tabControl = new CDockTabControl(mainSpliter, this);
								tabControl->dockWindow(window);

								mainSpliter->setControl(tabControl, rowID, colID);
								mainSpliter->setRowHeight(rowID, bound.Height);
								mainSpliter->setRowHeight(rowID + 1, 0.0f);

								if (weakRow > row)
									weakRow++;
							}
							else
							{
								CBase *base = m_innerPanel;
								m_innerPanel = NULL;

								CSplitter *newSpliter = new CSplitter(mainSpliter);
								newSpliter->setNumberRowCol(2, 1);

								CDockTabControl *tabControl = new CDockTabControl(newSpliter, this);
								tabControl->dockWindow(window);
								newSpliter->setControl(tabControl, 0, 0);

								newSpliter->setControl(neighborhood, 1, 0);

								newSpliter->setRowHeight(0, bound.Height);
								newSpliter->setRowHeight(1, 0.0f);

								// replace
								mainSpliter->setControl(newSpliter, row, col);

								mainSpliter = newSpliter;
								m_innerPanel = base;
							}
						}
					}
				}
				else if (dock == EDock::DockBottom)
				{
					if (mainSpliter->isVertical())
					{
						numRow++;
						mainSpliter->setNumberRowCol(numRow, numCol);

						u32 rowID = numRow - 1;
						u32 colID = numCol - 1;

						CDockTabControl *tabControl = new CDockTabControl(mainSpliter, this);
						tabControl->dockWindow(window);

						mainSpliter->setControl(tabControl, rowID, colID);
						mainSpliter->setRowHeight(rowID, bound.Height);
					}
					else
					{
						CBase *base = m_innerPanel;
						m_innerPanel = NULL;

						CSplitter *newSpliter = new CSplitter(this);
						newSpliter->dock(EPosition::Fill);
						newSpliter->setNumberRowCol(2, 1);

						CDockTabControl *tabControl = new CDockTabControl(newSpliter, this);
						tabControl->dockWindow(window);

						newSpliter->setControl(mainSpliter, 0, 0);
						newSpliter->setControl(tabControl, 1, 0);

						newSpliter->setRowHeight(0, 0.0f);
						newSpliter->setRowHeight(1, bound.Height);

						weakRow = 0;
						weakCol = 0;

						mainSpliter = newSpliter;
						m_mainSpliter = newSpliter;
						m_innerPanel = base;
					}
				}
				else if (dock == DockTargetBottom)
				{
					if (neighborhood != NULL)
					{
						u32 row, col;
						if (mainSpliter->getColRowFromControl(neighborhood, row, col) == true)
						{
							if (mainSpliter->isVertical() == true)
							{
								numRow++;
								u32 rowID = row + 1;
								u32 colID = col;

								mainSpliter->insertRow(row + 1);

								CDockTabControl *tabControl = new CDockTabControl(mainSpliter, this);
								tabControl->dockWindow(window);

								mainSpliter->setControl(tabControl, rowID, colID);
								mainSpliter->setRowHeight(rowID, bound.Height);
								mainSpliter->setRowHeight(rowID - 1, 0.0f);

								if (weakRow > row)
									weakRow++;
							}
							else
							{
								CBase *base = m_innerPanel;
								m_innerPanel = NULL;

								CSplitter *newSpliter = new CSplitter(mainSpliter);
								newSpliter->setNumberRowCol(2, 1);

								CDockTabControl *tabControl = new CDockTabControl(newSpliter, this);
								tabControl->dockWindow(window);

								newSpliter->setControl(neighborhood, 0, 0);
								newSpliter->setControl(tabControl, 1, 0);

								newSpliter->setRowHeight(0, 0.0f);
								newSpliter->setRowHeight(1, bound.Height);

								// replace
								mainSpliter->setControl(newSpliter, row, col);

								mainSpliter = newSpliter;
								m_innerPanel = base;
							}
						}
					}
				}
				else
				{
					if (outSpliter != NULL)
					{
						u32 row = outSpliter->getWeakRow();
						u32 col = outSpliter->getWeakCol();

						CBase *base = outSpliter->getControl(row, col);
						if (base == NULL)
						{
							CDockTabControl *tabControl = new CDockTabControl(outSpliter, this);
							tabControl->dockWindow(window);
							outSpliter->setControl(tabControl, row, col);
						}
						else
						{
							CDockTabControl *tabControl = dynamic_cast<CDockTabControl*>(base);
							if (tabControl != NULL)
								tabControl->dockWindow(window);
						}
					}
					else if (neighborhood != NULL)
					{
						neighborhood->dockWindow(window);
					}
				}

				mainSpliter->setWeakCol(weakCol);
				mainSpliter->setWeakRow(weakRow);

				mainSpliter->setColWidth(weakCol, 0.0f);
				mainSpliter->setRowHeight(weakRow, 0.0f);

				m_mainSpliter->sendToBack();

				invalidate();

				CGUIContext::getRoot()->notifySaveDockLayout();
			}

			void CDockPanel::hideDockHintWindow()
			{
				if (m_hoverHint != NULL)
					m_hoverHint->setColor(CThemeConfig::DefaultIconColor);
				m_hoverHint = NULL;

				m_hintWindow->setHidden(true);
			}

			void CDockPanel::showDockHint()
			{
				SPoint mousePoint = CInput::getInput()->getMousePosition();

				SPoint local = canvasPosToLocal(mousePoint);

				m_dragOverWindow = (CDockTabControl*)getControlAt(local.X, local.Y, typeid(CDockTabControl));

				layoutHintIcon();

				for (int i = 0; i < 9; i++)
				{
					if (m_dragOverWindow == NULL && i >= 5)
					{
						m_dockHint[i]->setHidden(true);
						continue;
					}

					m_dockHint[i]->setHidden(false);
					m_dockHint[i]->bringToFront();
				}
			}

			void CDockPanel::hideDockHint()
			{
				m_hintWindow->setHidden(true);

				for (int i = 0; i < 9; i++)
				{
					m_dockHint[i]->setHidden(true);

					if (m_dockHint[i] == m_hoverHint)
						m_dockHint[i]->setColor(CThemeConfig::ButtonPressColor);
					else
						m_dockHint[i]->setColor(CThemeConfig::DefaultIconColor);
				}
			}

			CDockHintIcon* CDockPanel::hitTestDockHint(const SPoint& point)
			{
				for (int i = 8; i >= 0; i--)
				{
					CDockHintIcon *hintControl = m_dockHint[i];
					SPoint localPoint = hintControl->canvasPosToLocal(point);

					if (hintControl->isHidden() == false)
					{
						float x = localPoint.X;
						float y = localPoint.Y;

						if (x < 0 || y < 0 || x >= hintControl->width() || y >= hintControl->height())
							continue;

						return hintControl;
					}
				}

				return NULL;
			}
		}
	}
}