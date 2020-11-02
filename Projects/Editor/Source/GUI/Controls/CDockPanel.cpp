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
				m_hoverHint(NULL)
			{
				m_mainSpliter = new CSplitter(this);
				m_mainSpliter->dock(EPosition::Fill);

				// auto set root
				if (g_rootDockPanel == NULL)
					g_rootDockPanel = this;

				// if this is not a canvas
				if (!isRoot())
					m_innerPanel = new CBase(m_mainSpliter);
				else
					m_mainSpliter->enableRenderFillRect(false);

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

			}

			bool CDockPanel::isRoot()
			{
				return this == g_rootDockPanel;
			}

			void CDockPanel::layout()
			{
				CBase::layout();

				SPoint center(
					round(m_bounds.X + m_bounds.Width * 0.5f),
					round(m_bounds.Y + m_bounds.Height * 0.5f)
				);

				float w = m_dockHint[0]->width();
				float h = m_dockHint[0]->height();

				float halfW = round(w * 0.5f);
				float halfH = round(h * 0.5f);

				float padding = 0.0f;

				if (isRoot() == true)
				{
					padding = 60.0f;
					m_dockHint[0]->setPos(center.X - halfW, center.Y - halfH);
					m_dockHint[1]->setPos(center.X - halfW - padding, center.Y - halfH);
					m_dockHint[2]->setPos(center.X - halfW + padding, center.Y - halfH);
					m_dockHint[3]->setPos(center.X - halfW, center.Y - halfH - padding);
					m_dockHint[4]->setPos(center.X - halfW, center.Y - halfH + padding);
				}
				else
				{
					padding = 60.0f;
					m_dockHint[0]->setPos(center.X - halfW, center.Y - halfH);

					m_dockHint[5]->setPos(center.X - halfW - padding, center.Y - halfH);
					m_dockHint[6]->setPos(center.X - halfW + padding, center.Y - halfH);
					m_dockHint[7]->setPos(center.X - halfW, center.Y - halfH - padding);
					m_dockHint[8]->setPos(center.X - halfW, center.Y - halfH + padding);

					padding = 120.0f;
					m_dockHint[1]->setPos(center.X - halfW - padding, center.Y - halfH);
					m_dockHint[2]->setPos(center.X - halfW + padding, center.Y - halfH);
					m_dockHint[3]->setPos(center.X - halfW, center.Y - halfH - padding);
					m_dockHint[4]->setPos(center.X - halfW, center.Y - halfH + padding);
				}
			}

			void CDockPanel::showDockHintWindow(CDockHintIcon *hint, CDockableWindow *window)
			{
				m_hoverHint = hint;
				m_hoverHint->setColor(CThemeConfig::ButtonPressColor);

				m_hintWindow->bringNextToControl(m_dockHint[0], true);
				m_hintWindow->setHidden(false);

				CSplitter *outSpliter = NULL;

				SRect spaceBound = getSpaceBounds(outSpliter);
				SRect fullBound = getRenderBounds();

				float maxWidth = round(spaceBound.Width * 0.25f);
				float maxHeight = round(spaceBound.Height * 0.25f);

				EDockHintIcon icon = hint->getIcon();
				if (icon == Left || icon == Right || icon == Top || icon == Bottom)
				{
					maxWidth = round(fullBound.Width * 0.25f);
					maxHeight = round(fullBound.Height * 0.25f);
				}

				SRect bound = window->getBounds();
				if (bound.Width >= maxWidth)
					bound.Width = maxWidth;

				if (bound.Height > maxHeight)
					bound.Height = maxHeight;

				switch (icon)
				{
				case Center:
					m_hintWindow->setBounds(spaceBound.X, spaceBound.Y, spaceBound.Width, spaceBound.Height);
					break;
				case Left:
					m_hintWindow->setBounds(0.0f, 0.0f, bound.Width, height());
					break;
				case TargetLeft:
					m_hintWindow->setBounds(spaceBound.X, 0.0f, bound.Width, spaceBound.Height);
					break;
				case Right:
					m_hintWindow->setBounds(width() - bound.Width, 0.0f, bound.Width, height());
					break;
				case TargetRight:
					m_hintWindow->setBounds(spaceBound.X + spaceBound.Width - bound.Width, 0.0f, bound.Width, spaceBound.Height);
					break;
				case Top:
					m_hintWindow->setBounds(0.0f, 0.0f, width(), bound.Height);
					break;
				case TargetTop:
					m_hintWindow->setBounds(spaceBound.X, spaceBound.Y, spaceBound.Width, bound.Height);
					break;
				case Bottom:
					m_hintWindow->setBounds(0.0f, height() - bound.Height, width(), bound.Height);
					break;
				case TargetBottom:
					m_hintWindow->setBounds(spaceBound.X, spaceBound.Y + spaceBound.Height - bound.Height, spaceBound.Width, bound.Height);
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

			void CDockPanel::dockChildWindow(CDockableWindow *window, EDock dock)
			{
				CSplitter *outSpliter = NULL;
				SRect boundSpace = getSpaceBounds(outSpliter);

				if (dock != DockCenter && isRoot())
					boundSpace = getRenderBounds();

				float maxWidth = round(boundSpace.Width * 0.25f);
				float maxHeight = round(boundSpace.Height * 0.25f);

				SRect bound = window->getBounds();
				if (bound.Width >= maxWidth)
					bound.Width = maxWidth;

				if (bound.Height > maxHeight)
					bound.Height = maxHeight;

				u32 numRow = m_mainSpliter->getNumRow();
				u32 numCol = m_mainSpliter->getNumCol();

				u32 weakRow = m_mainSpliter->getWeakRow();
				u32 weakCol = m_mainSpliter->getWeakCol();

				if (dock == EDock::DockRight)
				{
					if (m_mainSpliter->isHorizontal())
					{
						numCol++;
						u32 rowID = numRow - 1;
						u32 colID = numCol - 1;

						m_mainSpliter->setNumberRowCol(numRow, numCol);

						CDockTabControl *tabControl = new CDockTabControl(m_mainSpliter);
						tabControl->dockWindow(window);

						m_mainSpliter->setControl(tabControl, rowID, colID);
						m_mainSpliter->setColWidth(colID, bound.Width);
					}
					else
					{
						CBase *base = m_innerPanel;
						m_innerPanel = NULL;

						CSplitter *newSpliter = new CSplitter(this);
						newSpliter->dock(EPosition::Fill);
						newSpliter->setNumberRowCol(1, 2);

						newSpliter->setControl(m_mainSpliter, 0, 0);

						CDockTabControl *tabControl = new CDockTabControl(newSpliter);
						tabControl->dockWindow(window);
						newSpliter->setControl(tabControl, 0, 1);

						newSpliter->setColWidth(0, 0.0f);
						newSpliter->setColWidth(1, bound.Width);

						weakRow = 0;
						weakCol = 0;

						newSpliter->setWeakRow(weakRow);
						newSpliter->setWeakCol(weakCol);

						window->setStyleChild(true);

						m_mainSpliter = newSpliter;
						m_innerPanel = base;
					}
				}
				else if (dock == EDock::DockLeft)
				{
					if (m_mainSpliter->isHorizontal())
					{
						numCol++;
						u32 rowID = numRow - 1;
						u32 colID = 0;

						m_mainSpliter->insertCol(0);

						CDockTabControl *tabControl = new CDockTabControl(m_mainSpliter);
						tabControl->dockWindow(window);

						m_mainSpliter->setControl(tabControl, rowID, colID);
						m_mainSpliter->setColWidth(colID, bound.Width);

						window->setStyleChild(true);

						weakCol++;
					}
					else
					{
						CBase *base = m_innerPanel;
						m_innerPanel = NULL;

						CSplitter *newSpliter = new CSplitter(this);
						newSpliter->dock(EPosition::Fill);
						newSpliter->setNumberRowCol(1, 2);

						CDockTabControl *tabControl = new CDockTabControl(newSpliter);
						tabControl->dockWindow(window);

						newSpliter->setControl(tabControl, 0, 0);
						newSpliter->setControl(m_mainSpliter, 0, 1);

						newSpliter->setColWidth(0, bound.Width);
						newSpliter->setColWidth(1, 0.0f);

						weakRow = 0;
						weakCol = 1;

						newSpliter->setWeakRow(weakRow);
						newSpliter->setWeakCol(weakCol);

						window->setStyleChild(true);

						m_mainSpliter = newSpliter;
						m_innerPanel = base;
					}
				}
				else if (dock == EDock::DockTop)
				{
					if (m_mainSpliter->isVertical())
					{
						numRow++;
						u32 rowID = 0;
						u32 colID = numCol - 1;

						m_mainSpliter->insertRow(0);

						CDockTabControl *tabControl = new CDockTabControl(m_mainSpliter);
						tabControl->dockWindow(window);

						m_mainSpliter->setControl(tabControl, rowID, colID);
						m_mainSpliter->setRowHeight(rowID, bound.Height);

						window->setStyleChild(true);

						weakRow++;
					}
					else
					{
						CBase *base = m_innerPanel;
						m_innerPanel = NULL;

						CSplitter *newSpliter = new CSplitter(this);
						newSpliter->dock(EPosition::Fill);
						newSpliter->setNumberRowCol(2, 1);

						CDockTabControl *tabControl = new CDockTabControl(newSpliter);
						tabControl->dockWindow(window);

						newSpliter->setControl(tabControl, 0, 0);
						newSpliter->setControl(m_mainSpliter, 1, 0);

						newSpliter->setRowHeight(0, bound.Height);
						newSpliter->setRowHeight(1, 0.0f);

						weakRow = 1;
						weakCol = 0;

						newSpliter->setWeakRow(weakRow);
						newSpliter->setWeakCol(weakCol);

						window->setStyleChild(true);

						m_mainSpliter = newSpliter;
						m_innerPanel = base;
					}
				}
				else if (dock == EDock::DockBottom)
				{
					if (m_mainSpliter->isVertical())
					{
						numRow++;
						m_mainSpliter->setNumberRowCol(numRow, numCol);

						u32 rowID = numRow - 1;
						u32 colID = numCol - 1;

						CDockTabControl *tabControl = new CDockTabControl(m_mainSpliter);
						tabControl->dockWindow(window);

						m_mainSpliter->setControl(tabControl, rowID, colID);
						m_mainSpliter->setRowHeight(rowID, bound.Height);

						window->setStyleChild(true);
					}
					else
					{
						CBase *base = m_innerPanel;
						m_innerPanel = NULL;

						CSplitter *newSpliter = new CSplitter(this);
						newSpliter->dock(EPosition::Fill);
						newSpliter->setNumberRowCol(2, 1);

						CDockTabControl *tabControl = new CDockTabControl(newSpliter);
						tabControl->dockWindow(window);

						newSpliter->setControl(m_mainSpliter, 0, 0);
						newSpliter->setControl(tabControl, 1, 0);

						newSpliter->setRowHeight(0, 0.0f);
						newSpliter->setRowHeight(1, bound.Height);

						window->setStyleChild(true);

						weakRow = 0;
						weakCol = 0;

						newSpliter->setWeakRow(weakRow);
						newSpliter->setWeakCol(weakCol);

						m_mainSpliter = newSpliter;
						m_innerPanel = base;
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
							CDockTabControl *tabControl = new CDockTabControl(outSpliter);
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
				}

				m_mainSpliter->setWeakCol(weakCol);
				m_mainSpliter->setWeakRow(weakRow);

				m_mainSpliter->setColWidth(weakCol, 0.0f);
				m_mainSpliter->setRowHeight(weakRow, 0.0f);

				m_mainSpliter->sendToBack();

				invalidate();
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
				for (int i = 0; i < 9; i++)
				{
					if (isRoot() == true && i >= 5)
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
				for (int i = 0; i < 5; i++)
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