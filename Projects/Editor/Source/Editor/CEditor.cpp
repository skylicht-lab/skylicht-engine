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
			GUI::CDockableWindow *scene = new GUI::CDockableWindow(m_dockPanel, 20.0f, 20.0f, 900.0f, 600.0f);
			scene->setCaption(L"Scene");

			GUI::CDockableWindow *particle = new GUI::CDockableWindow(m_dockPanel, 40.0f, 40.0f, 300.0f, 600.0f);
			particle->setCaption(L"Particle");

			GUI::CDockableWindow *animation = new GUI::CDockableWindow(m_dockPanel, 60.0f, 60.0f, 900.0f, 300.0f);
			animation->setCaption(L"Animation");

			GUI::CDockableWindow *console = new GUI::CDockableWindow(m_dockPanel, 80.0f, 80.0f, 900.0f, 300.0f);
			console->setCaption(L"Console");

			GUI::CDockableWindow *property = new GUI::CDockableWindow(m_dockPanel, 100.0f, 100.0f, 300.0f, 600.0f);
			property->setCaption(L"Property");

			GUI::CDockableWindow *asset = new GUI::CDockableWindow(m_dockPanel, 120.0f, 120.0f, 900.0f, 300.0f);
			asset->setCaption(L"Asset");

			GUI::CDockableWindow *window2 = new GUI::CDockableWindow(m_dockPanel, 400.0f, 100.0f, 600.0f, 480.0f);
			GUI::CWindow *window3 = new GUI::CWindow(window2, 20.0f, 20.0f, 300.0f, 240.0f);
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
						readSpliterLayout(xmlRead, panel->getMainSpliter(), true);
					}
					else if (nodeName == L"vertical")
					{
						readSpliterLayout(xmlRead, panel->getMainSpliter(), false);
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

		void CEditor::readSpliterLayout(io::IXMLReader* xmlRead, GUI::CSplitter *spliter, bool isHorizontal)
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
						readSpliterLayout(xmlRead, newSpliter, true);
					}
					else if (nodeName == L"vertical")
					{
						GUI::CSplitter *newSpliter = new GUI::CSplitter(spliter);
						spliter->setControl(newSpliter, row, col);
						readSpliterLayout(xmlRead, newSpliter, false);
					}
					if (nodeName == L"docktab")
					{
						GUI::CDockTabControl *docktab = new GUI::CDockTabControl(spliter);
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
			std::wstring current = xmlRead->getAttributeValue(L"current");
			GUI::CDockableWindow *currentWin = NULL;

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