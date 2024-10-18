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
#include "CCanvas.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/Input/CInput.h"
#include "GUI/Utils/CDragAndDrop.h"

#include "CDockableWindow.h"
#include "CDockPanel.h"
#include "CDockTabControl.h"
#include "CDialogWindow.h"

#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CCanvas::CCanvas(float width, float height) :
				CBase(NULL),
				m_needSaveDockLayout(false),
				m_dialog(NULL)
			{
				setBounds(0.0, 0.0f, width, height);
				initialize();
			}

			CCanvas::~CCanvas()
			{
				saveDockLayoutToFile();
			}

			void CCanvas::initialize()
			{
			}

			CCanvas* CCanvas::getCanvas()
			{
				return this;
			}

			void CCanvas::addDelayedDelete(CBase* control)
			{
				if (m_deleteSet.find(control) == m_deleteSet.end())
				{
					m_deleteSet.insert(control);
					m_deleteList.push_back(control);
				}
			}

			void CCanvas::removeDelayDelete(CBase* control)
			{
				std::set<CBase*>::iterator itFind;

				if ((itFind = m_deleteSet.find(control)) != m_deleteSet.end())
				{
					m_deleteList.remove(control);
					m_deleteSet.erase(control);
				}
			}

			void CCanvas::processDelayedDeletes()
			{
				CBase::List deleteList = m_deleteList;
				m_deleteList.clear();
				m_deleteSet.clear();

				for (auto&& control : deleteList)
				{
					delete control;
				}
			}

			void CCanvas::closeMenu()
			{
				for (CBase* child : Children)
					child->closeMenu();
			}

			void CCanvas::update()
			{
				if (isHidden())
					return;

				TabableGroup.disableAllControl();

				recurseLayout();

				processDelayedDeletes();

				if (CInput::getInput() != NULL)
					CInput::getInput()->update();

				if (m_needSaveDockLayout == true)
				{
					m_needSaveDockLayout = false;
					saveDockLayoutToFile();
				}
			}

			void CCanvas::doRender()
			{
				CRenderer* render = CRenderer::getRenderer();

				render->begin();

				render->setClipRegion(m_bounds);

				render->setRenderOffset(SPoint(0.0f, 0.0f));

				CBase::doRender();

				render->enableClip(false);

				render->setRenderOffset(SPoint(0.0f, 0.0f));

				CBase::doRenderOverlay();

				CDragAndDrop::onRenderOverlay(this, render);

				render->end();
			}

			void CCanvas::render()
			{
				// do nothing
			}

			void CCanvas::saveDockLayoutToFile()
			{
				if (OnSaveDockLayout != nullptr)
				{
					std::string data;
					saveDockState(data, this, 0);

					OnSaveDockLayout(data);
				}
			}

			void CCanvas::saveDockState(std::string& data, CBase* base, int tab)
			{
				if (base == NULL)
					return;

				const SRect& bound = base->getBounds();

				// process
				CCanvas* canvas = dynamic_cast<CCanvas*>(base);
				CDockPanel* dockpanel = dynamic_cast<CDockPanel*>(base);
				CSplitter* spliter = dynamic_cast<CSplitter*>(base);
				CDockTabControl* dockTab = dynamic_cast<CDockTabControl*>(base);
				CDockableWindow* window = dynamic_cast<CDockableWindow*>(base);

				if (canvas != NULL)
				{
					data += generateTabSpace(tab);
					data += std::string("<canvas ") + getRectString(bound) + ">\n";

					for (CBase* c : base->Children)
						saveDockState(data, c, tab + 1);

					data += generateTabSpace(tab);
					data += "</canvas>\n";
				}
				else if (dockpanel != NULL)
				{
					data += generateTabSpace(tab);
					data += std::string("<dock-panel ") + getRectString(bound) + ">\n";

					for (CBase* c : base->Children)
						saveDockState(data, c, tab + 1);

					data += generateTabSpace(tab);
					data += "</dock-panel>\n";
				}
				else if (spliter != NULL)
				{
					u32 weakRow = spliter->getWeakRow();
					u32 weakCol = spliter->getWeakCol();

					if (spliter->isHorizontal())
					{
						data += generateTabSpace(tab);

						char text[512];
						sprintf(text, "<horizontal %s count='%d' weak-row='%d' weak-col='%d'>\n", getRectString(bound).c_str(), spliter->getNumCol(), weakRow, weakCol);
						data += text;

						for (u32 i = 0; i < spliter->getNumCol(); i++)
						{
							data += generateTabSpace(tab + 1);
							data += std::string("<col width='") + std::to_string((int)spliter->getColWidth(i)) + "'>\n";
							saveDockState(data, spliter->getControl(0, i), tab + 2);
							data += generateTabSpace(tab + 1);
							data += std::string("</col>\n");
						}

						data += generateTabSpace(tab);
						data += "</horizontal>\n";
					}
					else
					{
						data += generateTabSpace(tab);

						char text[512];
						sprintf(text, "<vertical %s count='%d' weak-row='%d' weak-col='%d'>\n", getRectString(bound).c_str(), spliter->getNumRow(), weakRow, weakCol);
						data += text;

						for (u32 i = 0; i < spliter->getNumRow(); i++)
						{
							data += generateTabSpace(tab + 1);
							data += std::string("<row height='") + std::to_string((int)spliter->getRowHeight(i)) + "'>\n";
							saveDockState(data, spliter->getControl(i, 0), tab + 2);
							data += generateTabSpace(tab + 1);
							data += std::string("</row>\n");
						}

						data += generateTabSpace(tab);
						data += "</vertical>\n";
					}
				}
				else if (dockTab != NULL)
				{
					CTabButton* currentTab = dockTab->getCurrentTab();
					CDockableWindow* currentWin = NULL;

					if (currentTab != NULL)
						currentWin = dynamic_cast<CDockableWindow*>(currentTab->getPage());

					data += generateTabSpace(tab);

					if (currentWin != NULL)
					{
						char label[256];
						CStringImp::convertUnicodeToUTF8(currentWin->getCaption().c_str(), label);

						char text[512];
						sprintf(text, "<docktab %s current='%s'>\n", getRectString(bound).c_str(), label);
						data += text;
					}
					else
						data += std::string("<docktab ") + getRectString(bound) + ">\n";

					CTabControl::ListTabButton listTab = dockTab->getListTabButton();
					CTabControl::ListTabButton::iterator i = listTab.begin(), end = listTab.end();

					while (i != end)
					{
						CTabButton* tabButton = (*i);
						CDockableWindow* child = dynamic_cast<CDockableWindow*>(tabButton->getPage());
						if (child != NULL)
						{
							data += generateTabSpace(tab + 1);
							data += std::string("<window name='") + CStringImp::convertUnicodeToUTF8(child->getCaption().c_str()) + "' " + getRectString(child->getBounds()) + "/>\n";
						}

						++i;
					}

					data += generateTabSpace(tab);
					data += "</docktab>\n";
				}
				else if (window != NULL)
				{
					if (window->getCurrentDockTab() == NULL)
					{
						data += generateTabSpace(tab);
						data += std::string("<window name='") + CStringImp::convertUnicodeToUTF8(window->getCaption().c_str()) + "' " + getRectString(bound) + "/>\n";
					}
				}
				else
				{
					for (CBase* c : base->Children)
						saveDockState(data, c, tab);
				}
			}

			std::string CCanvas::generateTabSpace(int n)
			{
				std::string result = "";
				for (int i = 0; i < n; i++)
					result += "    ";
				return result;
			}

			std::string CCanvas::getRectString(const SRect& r)
			{
				char string[512];
				sprintf(string, "x='%d' y='%d' w='%d' h='%d'", (int)r.X, (int)r.Y, (int)r.Width, (int)r.Height);
				return string;
			}

			bool CCanvas::handleAccelerator(const std::string& accelerator)
			{
				AccelMap::iterator iter = m_accelerators.find(accelerator);

				if (iter != m_accelerators.end())
				{
					iter->second(this);
					return true;
				}

				return false;
			}
		}
	}
}