/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CEditor.h"
#include "CWindowConfig.h"
#include "Utils/CStringImp.h"

#include "Space/Scene/CSpaceScene.h"
#include "Space/Assets/CSpaceAssets.h"

namespace Skylicht
{
	namespace Editor
	{
		CEditor::CEditor() :
			m_menuBar(NULL),
			m_canvas(NULL)

		{
			// init canvas
			m_canvas = GUI::CGUIContext::getRoot();
		}

		CEditor::~CEditor()
		{

		}

		void CEditor::update()
		{
			for (CSpace *s : m_workspaces)
				s->update();
		}

		void CEditor::initImportProjectGUI()
		{

		}

		bool CEditor::updateImporting()
		{
			return true;
		}

		void CEditor::initEditorGUI()
		{
			// bind save layout function
			m_canvas->OnSaveDockLayout = std::bind(&CEditor::saveLayout, this, std::placeholders::_1);

			// init menu bar
			m_menuBar = new GUI::CMenuBar(m_canvas);
			initMenuBar();

			// init status bar
			GUI::CBase *statusBar = new GUI::CBase(m_canvas);
			statusBar->setHeight(20.0f);
			statusBar->enableRenderFillRect(true);
			statusBar->setFillRectColor(GUI::CThemeConfig::WindowBackgroundColor);
			statusBar->dock(GUI::EPosition::Bottom);

			m_statusInfo = new GUI::CTableRow(statusBar);
			m_statusInfo->setWidth(450.0f);
			m_statusInfo->setColumnCount(4);

			float colWidth = round(m_statusInfo->width() / 4.0f);
			for (int i = 0; i < 4; i++)
				m_statusInfo->setColumnWidth(i, colWidth);

			m_statusInfo->setCellText(0, L"CTRL");
			m_statusInfo->setCellText(1, L"SHIFT");
			m_statusInfo->setCellText(2, L"CAPLOCK");
			m_statusInfo->setCellText(3, L"INS");
			m_statusInfo->dock(GUI::EPosition::Right);

			m_status = new GUI::CIconTextItem(statusBar);
			m_status->setLabel(L"Item(s) Save!");
			m_status->setIcon(GUI::ESystemIcon::Save);
			m_status->dock(GUI::EPosition::Fill);

			// init dock panel
			m_dockPanel = new GUI::CDockPanel(m_canvas);
			m_dockPanel->dock(GUI::EPosition::Fill);

			// compute layout
			m_canvas->recurseLayout();

			u32 x, y, w, h;
			bool maximize = false;
			std::string data;
			CWindowConfig::loadConfigAndExtraData(x, y, w, h, maximize, data);

			if (data.length() > 0)
			{
				initSessionLayout(data);
			}
			else
			{
				initDefaultLayout();
			}
		}

		void CEditor::initMenuBar()
		{
			GUI::CMenu *submenu, *temp;

			GUI::CMenuItem *logo = m_menuBar->addItem(GUI::ESystemIcon::Windows);
			submenu = logo->getMenu();
			submenu->addItem(L"About");
			submenu->addItem(L"Development Fund", GUI::ESystemIcon::Web);
			submenu->addSeparator();
			submenu->addItem(L"Project Setting", GUI::ESystemIcon::Setting);

			GUI::CMenuItem *file = m_menuBar->addItem(L"File");
			submenu = file->getMenu();
			submenu->addItem(L"New", GUI::ESystemIcon::NewFile, L"Ctrl + N");
			submenu->addItem(L"Open", GUI::ESystemIcon::Open, L"Ctrl + O");
			submenu->addItem(L"Open Recent")->setDisabled(true);
			submenu->addSeparator();
			submenu->addItem(L"Save", GUI::ESystemIcon::Save, L"Ctrl + S");
			submenu->addItem(L"Save As");
			submenu->addSeparator();
			submenu->addItem(L"Close", GUI::ESystemIcon::Quit, L"Ctrl + Q");

			GUI::CMenuItem *edit = m_menuBar->addItem(L"Edit");
			submenu = edit->getMenu();
			submenu->addItem(L"Search", GUI::ESystemIcon::Search, L"Ctrl + F");
			submenu->addSeparator();
			submenu->addItem(L"Undo", L"Ctrl + Z");
			submenu->addItem(L"Redo", L"Ctrl + Y");
			submenu->addSeparator();
			submenu->addItem(L"Copy", GUI::ESystemIcon::Copy, L"Ctrl + C");
			submenu->addItem(L"Paste", GUI::ESystemIcon::Paste, L"Ctrl + V");
			submenu->addItem(L"Cut", GUI::ESystemIcon::None, L"Ctrl + X");
			submenu->addSeparator();
			submenu->addItem(L"Delete");

			GUI::CMenuItem *asset = m_menuBar->addItem(L"Assets");
			submenu = asset->getMenu();
			temp = submenu;

			GUI::CMenuItem *create = submenu->addItem(L"Create");
			submenu = create->getMenu();
			submenu->addItem(L"Scene");
			submenu->addItem(L"Animation");
			submenu->addItem(L"Material");
			submenu->addItem(L"GUI");
			submenu->addItem(L"Sprite");

			submenu = temp;
			submenu->addSeparator();
			submenu->addItem(L"Copy path", GUI::ESystemIcon::Copy);
			submenu->addSeparator();
			submenu->addItem(L"Show in Explorer");
			submenu->addItem(L"Open");
			submenu->addItem(L"Rename");
			submenu->addItem(L"Delete");
			submenu->addSeparator();
			submenu->addItem(L"Refresh", L"Ctrl + R");

			GUI::CMenuItem *gameObject = m_menuBar->addItem(L"GameObject");
			submenu = gameObject->getMenu();
			submenu->addItem(L"Empty Object");
			submenu->addItem(L"Container Object");
			submenu->addSeparator();
			temp = submenu;

			GUI::CMenuItem *object = submenu->addItem(L"Object");
			submenu = object->getMenu();
			submenu->addItem(L"Cube");
			submenu->addItem(L"Sphere");
			submenu->addItem(L"Capsule");
			submenu->addItem(L"Cylinder");
			submenu->addItem(L"Plane");
			submenu->addSeparator();
			submenu->addItem(L"Mesh");
			submenu->addSeparator();
			submenu->addItem(L"Terrain");
			submenu->addSeparator();
			submenu->addItem(L"Skydome");
			submenu->addItem(L"Skybox");
			submenu->addItem(L"Sky");

			submenu = temp;
			GUI::CMenuItem *effect = submenu->addItem(L"Effect");
			submenu = effect->getMenu();
			submenu->addItem(L"Particle System");
			submenu->addItem(L"Line");
			submenu->addItem(L"Trail");

			submenu = temp;
			GUI::CMenuItem *lighting = submenu->addItem(L"Lighting");
			submenu = lighting->getMenu();
			submenu->addItem(L"Direction Light");
			submenu->addItem(L"Point Light");
			submenu->addItem(L"Spot Light");
			submenu->addSeparator();
			submenu->addItem(L"Reflection Probe");
			submenu->addItem(L"Light Probe");

			submenu = temp;
			submenu->addSeparator();
			submenu->addItem(L"Camera");
			submenu->addSeparator();
			submenu->addItem(L"Trigger");

			GUI::CMenuItem *window = m_menuBar->addItem(L"Window");
			submenu = window->getMenu();
			submenu->addItem(L"New workspace");
			submenu->addSeparator();
			submenu->addItem(L"Assets");
			submenu->addItem(L"Property");
			submenu->addItem(L"Scene");
			submenu->addItem(L"GUI Design");
			submenu->addItem(L"Animation");
			submenu->addSeparator();
			submenu->addItem(L"Reset layout");

			GUI::CMenuItem *help = m_menuBar->addItem(L"Help");
			submenu = help->getMenu();
			submenu->addItem(L"Tutorial", GUI::ESystemIcon::Guide);
			submenu->addItem(L"Report a bug", GUI::ESystemIcon::Web);
			submenu->addSeparator();
			submenu->addItem(L"Development Fund", GUI::ESystemIcon::Web);
		}

		void CEditor::saveLayout(const std::string& data)
		{
			CWindowConfig::saveExtraData(data.c_str());
		}

		void CEditor::initDefaultLayout()
		{
			float width = m_dockPanel->width();
			float height = m_dockPanel->height();
			float w, h;

			w = width;
			h = round(height * 0.8f);
			GUI::CDockableWindow *scene = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			scene->setCaption(L"Scene");
			m_dockPanel->dockChildWindow(scene, NULL, GUI::CDockPanel::DockCenter);
			m_dockPanel->recurseLayout();

			w = round(width * 0.7f);
			h = round(height * 0.3f);
			GUI::CDockableWindow *asset = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			asset->setCaption(L"Assets");
			m_dockPanel->dockChildWindow(asset, NULL, GUI::CDockPanel::DockBottom);
			m_dockPanel->recurseLayout();

			w = round(width * 0.35f);
			h = round(height * 0.3f);
			GUI::CDockableWindow *preview = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			preview->setCaption(L"Preview");
			m_dockPanel->dockChildWindow(preview, asset->getCurrentDockTab(), GUI::CDockPanel::DockTargetRight);
			m_dockPanel->recurseLayout();

			w = width * 0.3f;
			h = height;
			GUI::CDockableWindow *property = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			property->setCaption(L"Property");
			m_dockPanel->dockChildWindow(property, NULL, GUI::CDockPanel::DockRight);
			m_dockPanel->recurseLayout();

			initWorkspace(scene, scene->getCaption());
			initWorkspace(asset, asset->getCaption());
			initWorkspace(preview, preview->getCaption());
			initWorkspace(property, property->getCaption());
		}

		void CEditor::initWorkspace(GUI::CDockableWindow *window, const std::wstring& workspace)
		{
			if (workspace == L"Scene")
			{
				m_workspaces.push_back(new CSpaceScene(window, this));
			}
			if (workspace == L"GUI Design")
			{

			}
			if (workspace == L"Animation")
			{

			}
			else if (workspace == L"Assets")
			{
				m_workspaces.push_back(new CSpaceAssets(window, this));
			}
			else if (workspace == L"Preview")
			{

			}
			else if (workspace == L"Property")
			{

			}
		}

		void CEditor::removeWorkspace(CSpace *space)
		{
			std::list<CSpace*>::iterator i = std::find(m_workspaces.begin(), m_workspaces.end(), space);
			if (i != m_workspaces.end())
			{
				delete (*i);
				m_workspaces.erase(i);
			}
		}

		void CEditor::initSessionLayout(const std::string& data)
		{
			io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();
			io::IReadFile *file = fs->createMemoryReadFile(data.c_str(), data.length(), "data");
			io::IXMLReader *xmlRead = fs->createXMLReader(file);
			if (xmlRead == NULL)
				return;

			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					core::stringw nodeName = xmlRead->getNodeName();

					if (core::stringw(L"canvas") == nodeName)
					{
						readBound(xmlRead, m_canvas);
					}
					else if (core::stringw(L"dock-panel") == nodeName)
					{
						// dock layout
						readBound(xmlRead, m_dockPanel);
						readDockLayout(xmlRead, m_dockPanel);
					}
				}
				break;
				case io::EXN_ELEMENT_END:
				{
				}
				break;
				case io::EXN_TEXT:
				{
					// std::wstring text = xmlRead->getNodeData();
				}
				break;
				}
			}

			xmlRead->drop();
			file->drop();
		}

		void CEditor::readBound(io::IXMLReader* xml, GUI::CBase *base)
		{
			char ansiData[64];
			const wchar_t *valueX = xml->getAttributeValue(L"x");
			const wchar_t *valueY = xml->getAttributeValue(L"y");
			const wchar_t *valueW = xml->getAttributeValue(L"w");
			const wchar_t *valueH = xml->getAttributeValue(L"h");

			if (valueX == NULL || valueY == NULL || valueW == NULL || valueH == NULL)
				return;

			float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;

			if (valueX != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueX, ansiData);
				x = (float)atof(ansiData);
			}

			if (valueY != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueY, ansiData);
				y = (float)atof(ansiData);
			}

			if (valueW != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueW, ansiData);
				w = (float)atof(ansiData);
			}

			if (valueH != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueH, ansiData);
				h = (float)atof(ansiData);
			}

			base->setBounds(x, y, w, h);
		}

		void CEditor::readDockLayout(io::IXMLReader* xmlRead, GUI::CDockPanel *panel)
		{
			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					core::stringw nodeName = xmlRead->getNodeName();

					if (nodeName == L"horizontal")
					{
						readSpliterLayout(xmlRead, panel, panel->getMainSpliter(), true);
					}
					else if (nodeName == L"vertical")
					{
						readSpliterLayout(xmlRead, panel, panel->getMainSpliter(), false);
					}
					else if (nodeName == L"window")
					{
						const wchar_t *name;
						name = xmlRead->getAttributeValue(L"name");
						if (name != NULL)
						{
							GUI::CDockableWindow *win = new GUI::CDockableWindow(panel, 0, 0, 1, 1);
							readBound(xmlRead, win);
							win->setCaption(name);

							initWorkspace(win, name);
						}
					}
				}
				break;
				case io::EXN_ELEMENT_END:
				{
					core::stringw nodeName = xmlRead->getNodeName();

					if (core::stringw(L"dock-panel") == nodeName)
					{
						return;
					}
				}
				break;
				}
			}
		}

		void CEditor::readSpliterLayout(io::IXMLReader* xmlRead, GUI::CDockPanel *panel, GUI::CSplitter *spliter, bool isHorizontal)
		{
			u32 count;
			u32 weakRow = 0;
			u32 weakCol = 0;

			char ansiData[64];
			const wchar_t *valueCount;

			readBound(xmlRead, spliter);

			valueCount = xmlRead->getAttributeValue(L"count");
			if (valueCount != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
				count = (u32)atoi(ansiData);
			}

			valueCount = xmlRead->getAttributeValue(L"weak-row");
			if (valueCount != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
				weakRow = (u32)atoi(ansiData);
			}

			valueCount = xmlRead->getAttributeValue(L"weak-col");
			if (valueCount != NULL)
			{
				CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
				weakCol = (u32)atoi(ansiData);
			}

			if (isHorizontal)
				spliter->setNumberRowCol(1, count);
			else
				spliter->setNumberRowCol(count, 1);

			spliter->setWeakRow(weakRow);
			spliter->setWeakCol(weakCol);

			u32 col = 0;
			u32 row = 0;

			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					core::stringw nodeName = xmlRead->getNodeName();
					if (nodeName == L"col")
					{
						float width = 0;
						valueCount = xmlRead->getAttributeValue(L"width");
						if (valueCount != NULL)
						{
							CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
							width = (float)atof(ansiData);
						}

						spliter->setColWidth(col, width);
					}
					if (nodeName == L"row")
					{
						float height = 0;
						valueCount = xmlRead->getAttributeValue(L"height");
						if (valueCount != NULL)
						{
							CStringImp::convertUnicodeToUTF8(valueCount, ansiData);
							height = (float)atof(ansiData);
						}

						spliter->setRowHeight(row, height);
					}
					if (nodeName == L"horizontal")
					{
						GUI::CSplitter *newSpliter = new GUI::CSplitter(spliter);
						spliter->setControl(newSpliter, row, col);
						readSpliterLayout(xmlRead, panel, newSpliter, true);
					}
					else if (nodeName == L"vertical")
					{
						GUI::CSplitter *newSpliter = new GUI::CSplitter(spliter);
						spliter->setControl(newSpliter, row, col);
						readSpliterLayout(xmlRead, panel, newSpliter, false);
					}
					if (nodeName == L"docktab")
					{
						GUI::CDockTabControl *docktab = new GUI::CDockTabControl(spliter, panel);
						readBound(xmlRead, docktab);

						spliter->setControl(docktab, row, col);
						readDockTab(xmlRead, docktab);
					}
				}
				break;
				case io::EXN_ELEMENT_END:
				{
					core::stringw nodeName = xmlRead->getNodeName();

					if (core::stringw(L"horizontal") == nodeName && isHorizontal)
					{
						spliter->saveUserExpectedSize();
						return;
					}
					else if (core::stringw(L"vertical") == nodeName && !isHorizontal)
					{
						spliter->saveUserExpectedSize();
						return;
					}
					else if (nodeName == L"col" && isHorizontal)
					{
						col++;
					}
					else if (nodeName == L"row" && !isHorizontal)
					{
						row++;
					}
				}
				break;
				}
			}
		}

		void CEditor::readDockTab(io::IXMLReader* xmlRead, GUI::CDockTabControl *tabcontrol)
		{
			const wchar_t *currentTab = xmlRead->getAttributeValue(L"current");

			std::wstring current;
			GUI::CDockableWindow *currentWin = NULL;
			if (currentTab != NULL)
				current = currentTab;

			while (xmlRead->read())
			{
				switch (xmlRead->getNodeType())
				{
				case io::EXN_ELEMENT:
				{
					core::stringw nodeName = xmlRead->getNodeName();
					if (nodeName == L"window")
					{
						const wchar_t *name;
						name = xmlRead->getAttributeValue(L"name");
						if (name != NULL)
						{
							GUI::CDockableWindow *win = new GUI::CDockableWindow(m_dockPanel, 0, 0, 1, 1);
							win->setCaption(name);
							tabcontrol->dockWindow(win);

							initWorkspace(win, name);

							if (CStringImp::comp(name, current.c_str()) == 0)
								currentWin = win;
						}
					}
				}
				case io::EXN_ELEMENT_END:
				{
					core::stringw nodeName = xmlRead->getNodeName();
					if (core::stringw(L"docktab") == nodeName)
					{
						if (currentWin != NULL)
							tabcontrol->setCurrentWindow(currentWin);
						return;
					}
				}
				break;
				}
			}
		}
	}
}