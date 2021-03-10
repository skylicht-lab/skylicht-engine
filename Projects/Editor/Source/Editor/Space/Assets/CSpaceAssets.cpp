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
#include "CSpaceAssets.h"
#include "Utils/CStringImp.h"
#include "GUI/Input/CInput.h"
#include "GUI/Clipboard/CClipboard.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceAssets::CSpaceAssets(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_contextMenuOwner(NULL),
			m_selectedItem(NULL)
		{
			m_assetManager = CAssetManager::getInstance();

			GUI::CToolbar* toolbar = new GUI::CToolbar(window);

			toolbar->addButton(L"Add", GUI::ESystemIcon::Plus);
			toolbar->addButton(GUI::ESystemIcon::Setting, true);
			toolbar->addSpace(true);
			m_search = new GUI::CTextBox(toolbar);
			m_search->setWidth(200.0f);
			m_search->showIcon();
			m_search->setStringHint(L"Search");
			toolbar->addControl(m_search, true);

			GUI::CSplitter* spliter = new GUI::CSplitter(window);
			spliter->dock(GUI::EPosition::Fill);
			spliter->setNumberRowCol(1, 2);

			m_treeFS = new GUI::CTreeControl(spliter);
			m_treeFS->OnItemContextMenu = BIND_LISTENER(&CSpaceAssets::OnTreeContextMenu, this);
			m_treeFSController = new CTreeFSController(m_treeFS);

			m_listFS = new GUI::CListBox(spliter);
			m_listFS->OnItemContextMenu = BIND_LISTENER(&CSpaceAssets::OnListContextMenu, this);
			m_listFSController = new CListFSController(m_listFS);

			spliter->setControl(m_treeFS, 0, 0);
			spliter->setControl(m_listFS, 0, 1);

			spliter->setColWidth(0, 300.0f);
			spliter->setWeakCol(1);

			m_contextMenu = new GUI::CMenu(window->getCanvas());
			m_contextMenu->setHidden(true);
			m_contextMenu->OnCommand = BIND_LISTENER(&CSpaceAssets::OnCommand, this);

			m_menuOpen = m_contextMenu->addItem(L"Open");
			m_contextMenu->addItem(L"Show in Explorer");
			m_contextMenu->addSeparator();
			m_contextMenu->addItem(L"Delete");
			m_contextMenu->addItem(L"Rename", L"F2");
			m_contextMenu->addItem(L"Copy path", L"SHIFT + C");
			m_contextMenu->addItem(L"Duplicate", L"CTRL + D");

			m_listFS->addAccelerator("SHIFT + C", BIND_LISTENER(&CSpaceAssets::OnCopyPath, this));
			m_treeFS->addAccelerator("SHIFT + C", BIND_LISTENER(&CSpaceAssets::OnCopyPath, this));
		}

		CSpaceAssets::~CSpaceAssets()
		{
			delete m_treeFSController;
			delete m_listFSController;
		}

		void CSpaceAssets::OnTreeContextMenu(GUI::CBase* row)
		{
			GUI::CTreeRowItem* rowItem = dynamic_cast<GUI::CTreeRowItem*>(row);
			if (rowItem != NULL)
			{
				GUI::CTreeNode* node = rowItem->getNode();
				if (node != NULL)
				{
					m_contextMenuOwner = node->getRoot();
					m_selectedItem = node;
					m_selectedPath = node->getTagString();
					m_menuOpen->setHidden(true);
					m_contextMenu->open(GUI::CInput::getInput()->getMousePosition());
				}
			}
		}

		void CSpaceAssets::OnListContextMenu(GUI::CBase* row)
		{
			GUI::CListRowItem* rowItem = dynamic_cast<GUI::CListRowItem*>(row);
			if (rowItem != NULL)
			{
				m_contextMenuOwner = rowItem->getListBox();
				m_selectedItem = rowItem;
				m_selectedPath = rowItem->getTagString();
				m_menuOpen->setHidden(false);
				m_contextMenu->open(GUI::CInput::getInput()->getMousePosition());
			}
		}

		void CSpaceAssets::OnCommand(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();
			if (label == L"Open")
			{
				if (m_contextMenuOwner == m_listFS)
					m_listFSController->OnFileOpen(m_selectedItem);
			}
			else if (label == L"Show in Explorer")
			{

			}
			else if (label == L"Delete")
			{

			}
			else if (label == L"Rename")
			{

			}
			else if (label == L"Copy path")
			{
				wchar_t* text = new wchar_t[m_selectedPath.size() + 1];
				CStringImp::convertUTF8ToUnicode(m_selectedPath.c_str(), text);
				GUI::CClipboard::get()->copyTextToClipboard(text);
				delete[]text;
			}
			else if (label == L"Duplicate")
			{

			}
		}

		void CSpaceAssets::OnCopyPath(GUI::CBase* item)
		{
			wchar_t* text = NULL;

			if (m_listFS->isFocussed())
			{
				GUI::CListRowItem* row = m_listFS->getSelected();
				std::string path = row->getTagString();
				if (path.empty() == false)
				{
					text = new wchar_t[path.size() + 1];
					CStringImp::convertUTF8ToUnicode(path.c_str(), text);
				}
			}
			else if (m_treeFS->isFocussed())
			{
				GUI::CTreeNode* node = m_treeFS->getChildSelected();
				std::string path = node->getTagString();
				if (path.empty() == false)
				{
					text = new wchar_t[path.size() + 1];
					CStringImp::convertUTF8ToUnicode(path.c_str(), text);
				}
			}

			if (text != NULL)
			{
				GUI::CClipboard::get()->copyTextToClipboard(text);
				delete[]text;
			}
		}
	}
}