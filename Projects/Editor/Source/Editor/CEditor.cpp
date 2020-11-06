/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyRight notice and this permission notice shall be included in all copies or substantial portions of the Software.

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

namespace Skylicht
{
	namespace Editor
	{
		CEditor::CEditor()
		{
			u32 x, y, w, h;
			bool maximize = false;
			std::string data;
			CWindowConfig::loadConfigAndExtraData(x, y, w, h, maximize, data);

			m_canvas = GUI::CGUIContext::getRoot();
			m_canvas->OnSaveDockLayout = std::bind(&CEditor::saveLayout, this, std::placeholders::_1);

			m_dockPanel = new GUI::CDockPanel(m_canvas);
			m_dockPanel->dock(GUI::EPosition::Fill);

			m_canvas->recurseLayout();

			if (data.length() > 0)
			{
				initSessionLayout(data);
			}
			else
			{
				initDefaultLayout();
			}
		}

		CEditor::~CEditor()
		{
			m_canvas->notifySaveDockLayout();
			m_canvas->update();
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
			asset->setCaption(L"Asset");
			m_dockPanel->dockChildWindow(asset, NULL, GUI::CDockPanel::DockBottom);
			m_dockPanel->recurseLayout();

			w = round(width * 0.35f);
			h = round(height * 0.3f);
			GUI::CDockableWindow *console = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			console->setCaption(L"Console");
			m_dockPanel->dockChildWindow(console, asset->getCurrentDockTab(), GUI::CDockPanel::DockTargetRight);
			m_dockPanel->recurseLayout();

			w = width * 0.3f;
			h = height;
			GUI::CDockableWindow *property = new GUI::CDockableWindow(m_dockPanel, 0.0f, 0.0f, w, h);
			property->setCaption(L"Property");
			m_dockPanel->dockChildWindow(property, NULL, GUI::CDockPanel::DockRight);
			m_dockPanel->recurseLayout();
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