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
			m_renameNode(NULL),
			m_contextMenuOwner(NULL),
			m_selectedItem(NULL)
		{
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

			m_folder = new GUI::CTreeControl(spliter);
			m_folder->OnKeyPress = std::bind(
				&CSpaceAssets::OnTreeKeyPress,
				this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3
			);

			m_folder->OnItemContextMenu = BIND_LISTENER(&CSpaceAssets::OnTreeContextMenu, this);

			m_assetManager = CAssetManager::getInstance();

			m_root = m_folder->addNode(L"Assets", GUI::ESystemIcon::OpenFolder);
			m_root->tagString(m_assetManager->getAssetFolder());
			m_root->OnExpand = BIND_LISTENER(&CSpaceAssets::OnTreeNodeExpand, this);
			m_root->OnCollapse = BIND_LISTENER(&CSpaceAssets::OnTreeNodeCollapse, this);
			m_root->OnSelectChange = BIND_LISTENER(&CSpaceAssets::OnTreeNodeSelected, this);

			m_root->setAlwayShowExpandButton(true);
			m_root->expand(false);

			std::vector<SFileInfo> files;

			// add root to tree folder
			m_assetManager->getRoot(files);
			addTreeFolder(m_root, files);

			// column spliter
			spliter->setControl(m_folder, 0, 0);

			m_listFiles = new GUI::CListBox(spliter);
			m_listFiles->OnKeyPress = std::bind(
				&CSpaceAssets::OnListKeyPress,
				this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3
			);
			m_listFiles->OnItemContextMenu = BIND_LISTENER(&CSpaceAssets::OnListContextMenu, this);

			spliter->setControl(m_listFiles, 0, 1);

			addListFolder("", files);

			spliter->setColWidth(0, 300.0f);
			spliter->setWeakCol(1);

			m_contextMenu = new GUI::CMenu(window->getCanvas());
			m_contextMenu->setHidden(true);
			m_contextMenu->OnCommand = BIND_LISTENER(&CSpaceAssets::OnCommand, this);

			m_openMenuItem = m_contextMenu->addItem(L"Open");
			m_contextMenu->addItem(L"Show in Explorer");
			m_contextMenu->addSeparator();
			m_contextMenu->addItem(L"Delete");
			m_contextMenu->addItem(L"Rename", L"F2");
			m_contextMenu->addItem(L"Copy path", L"SHIFT + C");
			m_contextMenu->addItem(L"Duplicate", L"CTRL + D");

			m_listFiles->addAccelerator("SHIFT + C", BIND_LISTENER(&CSpaceAssets::OnCopyPath, this));
			m_folder->addAccelerator("SHIFT + C", BIND_LISTENER(&CSpaceAssets::OnCopyPath, this));
		}

		CSpaceAssets::~CSpaceAssets()
		{

		}

		void CSpaceAssets::addTreeFolder(GUI::CTreeNode* node, std::vector<SFileInfo>& files)
		{
			for (const SFileInfo& f : files)
			{
				if (f.IsFolder == true)
				{
					GUI::CTreeNode* childNode = node->addNode(f.NameW.c_str(), GUI::ESystemIcon::Folder);
					childNode->tagString(f.FullPath);
					childNode->OnExpand = BIND_LISTENER(&CSpaceAssets::OnTreeNodeExpand, this);
					childNode->OnCollapse = BIND_LISTENER(&CSpaceAssets::OnTreeNodeCollapse, this);
					childNode->OnSelectChange = BIND_LISTENER(&CSpaceAssets::OnTreeNodeSelected, this);

					if (m_assetManager->isFolderEmpty(f.FullPath.c_str()) == false)
						childNode->setAlwayShowExpandButton(true);
				}
			}
		}

		void CSpaceAssets::OnTreeNodeExpand(GUI::CBase* node)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(node);
			if (treeNode != NULL)
			{
				treeNode->setIcon(GUI::ESystemIcon::OpenFolder);

				const std::string& fullPath = treeNode->getTagString();

				std::vector<SFileInfo> files;
				m_assetManager->getFolder(fullPath.c_str(), files);

				addTreeFolder(treeNode, files);
			}
		}

		void CSpaceAssets::OnTreeNodeCollapse(GUI::CBase* node)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(node);
			if (treeNode != NULL)
			{
				treeNode->setIcon(GUI::ESystemIcon::Folder);
				treeNode->removeAllTreeNode();
			}
		}

		void CSpaceAssets::OnTreeNodeSelected(GUI::CBase* node)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(node);
			if (treeNode != NULL)
			{
				treeNode->setIcon(GUI::ESystemIcon::OpenFolder);

				const std::string& fullPath = treeNode->getTagString();

				std::vector<SFileInfo> files;
				m_assetManager->getFolder(fullPath.c_str(), files);

				addListFolder(fullPath, files);
			}
		}

		void CSpaceAssets::addListFolder(const std::string& currentFolder, std::vector<SFileInfo>& files)
		{
			m_listFiles->removeAllItem();

			if (currentFolder.size() > 0 &&
				currentFolder != m_assetManager->getAssetFolder())
			{
				GUI::CListRowItem* item = m_listFiles->addItem(L"..", GUI::ESystemIcon::Folder);

				std::string parent = CPath::getFolderPath(currentFolder);
				item->tagString(parent);
				item->tagBool(true);
				item->OnDoubleLeftMouseClick = BIND_LISTENER(&CSpaceAssets::OnFileOpen, this);
			}

			for (SFileInfo& f : files)
			{
				GUI::CListRowItem* item;

				if (f.IsFolder)
					item = m_listFiles->addItem(f.NameW.c_str(), GUI::ESystemIcon::Folder);
				else
					item = m_listFiles->addItem(f.NameW.c_str(), GUI::ESystemIcon::File);

				item->tagString(f.FullPath);
				item->tagBool(f.IsFolder);
				item->OnDoubleLeftMouseClick = BIND_LISTENER(&CSpaceAssets::OnFileOpen, this);
			}

			m_listFiles->setScrollVertical(0.0f);
		}

		void CSpaceAssets::OnFileOpen(GUI::CBase* node)
		{
			GUI::CListRowItem* rowNode = dynamic_cast<GUI::CListRowItem*>(node);
			if (rowNode != NULL)
			{
				const std::string& fullPath = rowNode->getTagString();

				bool isFolder = rowNode->getTagBool();
				if (isFolder == true)
				{
					// browse folder
					std::vector<SFileInfo> files;
					m_assetManager->getFolder(fullPath.c_str(), files);

					addListFolder(fullPath, files);

					expandTreeFolder(fullPath);
				}
				else
				{
					// shell open the file
				}
			}
		}

		void CSpaceAssets::expandTreeFolder(const std::string& folder)
		{
			std::string shortPath = m_assetManager->getShortPath(folder.c_str());
			if (shortPath.size() > 0)
			{
				std::vector<std::string> result;
				CStringImp::splitString(shortPath.c_str(), "/", result);

				GUI::CTreeNode* node = m_root;

				wchar_t wname[512];

				for (u32 i = 0, n = result.size(); i < n; i++)
				{
					const std::string name = result[i];
					CStringImp::convertUTF8ToUnicode(name.c_str(), wname);

					GUI::CTreeNode* child = node->getChildNodeByLabel(wname);
					if (child == NULL)
						break;

					if (child->haveChild())
						child->expand(false);
					else
						child->expand(true);

					node = child;
				}

				if (node != NULL)
				{
					node->forceLayout();
					m_folder->getScrollControl()->scrollToItem(node);
				}
			}
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
					m_openMenuItem->setHidden(true);
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
				m_openMenuItem->setHidden(false);
				m_contextMenu->open(GUI::CInput::getInput()->getMousePosition());
			}
		}

		void CSpaceAssets::OnTreeKeyPress(GUI::CBase* control, int key, bool press)
		{
			GUI::CTreeControl* tree = dynamic_cast<GUI::CTreeControl*>(control);
			if (tree == NULL)
				return;

			if (key == GUI::KEY_F2)
			{
				GUI::CTreeNode* node = tree->getChildSelected();
				if (node != NULL)
				{
					m_renameNode = node;

					node->getTextEditHelper()->beginEdit(
						BIND_LISTENER(&CSpaceAssets::OnTreeRename, this),
						BIND_LISTENER(&CSpaceAssets::OnTreeCancelRename, this)
					);
				}
			}
		}

		void CSpaceAssets::OnListKeyPress(GUI::CBase* control, int key, bool press)
		{
			GUI::CListBox* list = dynamic_cast<GUI::CListBox*>(control);
			if (list == NULL)
				return;

			if (key == GUI::KEY_F2)
			{
				GUI::CListRowItem* node = list->getSelected();
				if (node != NULL)
				{
					m_renameItem = node;

					node->getTextEditHelper()->beginEdit(
						BIND_LISTENER(&CSpaceAssets::OnListRename, this),
						BIND_LISTENER(&CSpaceAssets::OnListCancelRename, this)
					);
				}
			}
		}

		void CSpaceAssets::OnTreeRename(GUI::CBase* control)
		{
			m_folder->focus();
		}

		void CSpaceAssets::OnTreeCancelRename(GUI::CBase* control)
		{
			m_folder->focus();
		}

		void CSpaceAssets::OnListRename(GUI::CBase* control)
		{
			m_listFiles->focus();
		}

		void CSpaceAssets::OnListCancelRename(GUI::CBase* control)
		{
			m_listFiles->focus();
		}

		void CSpaceAssets::OnCommand(GUI::CBase* item)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(item);
			const std::wstring& label = menuItem->getLabel();
			if (label == L"Open")
			{
				if (m_contextMenuOwner == m_listFiles)
					OnFileOpen(m_selectedItem);
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

			if (m_listFiles->isFocussed())
			{
				GUI::CListRowItem* row = m_listFiles->getSelected();
				std::string path = row->getTagString();
				if (path.empty() == false)
				{
					text = new wchar_t[path.size() + 1];
					CStringImp::convertUTF8ToUnicode(path.c_str(), text);
				}
			}
			else if (m_folder->isFocussed())
			{
				GUI::CTreeNode* node = m_folder->getChildSelected();
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