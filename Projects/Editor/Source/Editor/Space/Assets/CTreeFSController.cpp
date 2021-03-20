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
#include "CTreeFSController.h"
#include "CListFSController.h"

#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		CTreeFSController::CTreeFSController(GUI::CTreeControl* treeFS) :
			m_treeFS(treeFS),
			m_renameNode(NULL),
			m_nodeAssets(NULL),
			m_listController(NULL)
		{
			m_assetManager = CAssetManager::getInstance();

			m_treeFS->OnKeyPress = std::bind(
				&CTreeFSController::OnKeyPress,
				this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3
			);

			m_nodeAssets = m_treeFS->addNode(L"Assets", GUI::ESystemIcon::OpenFolder);
			m_nodeAssets->tagString(m_assetManager->getAssetFolder());
			m_nodeAssets->OnExpand = BIND_LISTENER(&CTreeFSController::OnExpand, this);
			m_nodeAssets->OnCollapse = BIND_LISTENER(&CTreeFSController::OnCollapse, this);
			m_nodeAssets->OnSelectChange = BIND_LISTENER(&CTreeFSController::OnSelected, this);

			m_nodeAssets->setAlwayShowExpandButton(true);
			m_nodeAssets->expand(false);

			std::vector<SFileInfo> files;
			m_assetManager->getRoot(files);
			add(m_nodeAssets, files);
		}

		CTreeFSController::~CTreeFSController()
		{

		}

		void CTreeFSController::add(GUI::CTreeNode* node, std::vector<SFileInfo>& files)
		{
			for (const SFileInfo& f : files)
			{
				if (f.IsFolder == true)
				{
					GUI::CTreeNode* childNode = node->addNode(f.NameW.c_str(), GUI::ESystemIcon::Folder);
					childNode->tagString(f.FullPath);
					childNode->OnExpand = BIND_LISTENER(&CTreeFSController::OnExpand, this);
					childNode->OnCollapse = BIND_LISTENER(&CTreeFSController::OnCollapse, this);
					childNode->OnSelectChange = BIND_LISTENER(&CTreeFSController::OnSelected, this);

					if (m_assetManager->isFolderEmpty(f.FullPath.c_str()) == false)
						childNode->setAlwayShowExpandButton(true);
				}
			}
		}

		void CTreeFSController::OnExpand(GUI::CBase* node)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(node);
			if (treeNode != NULL)
			{
				treeNode->setIcon(GUI::ESystemIcon::OpenFolder);

				const std::string& fullPath = treeNode->getTagString();

				std::vector<SFileInfo> files;
				m_assetManager->getFolder(fullPath.c_str(), files);

				add(treeNode, files);
			}
		}

		void CTreeFSController::OnCollapse(GUI::CBase* node)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(node);
			if (treeNode != NULL)
			{
				treeNode->setIcon(GUI::ESystemIcon::Folder);
				treeNode->removeAllTreeNode();
			}
		}

		void CTreeFSController::OnSelected(GUI::CBase* node)
		{
			GUI::CTreeNode* treeNode = dynamic_cast<GUI::CTreeNode*>(node);
			if (treeNode != NULL)
			{
				treeNode->setIcon(GUI::ESystemIcon::OpenFolder);

				const std::string& fullPath = treeNode->getTagString();

				std::vector<SFileInfo> files;
				m_assetManager->getFolder(fullPath.c_str(), files);

				if (m_listController != NULL)
					m_listController->add(fullPath, files);
			}
		}

		void CTreeFSController::expand(const std::string& folder)
		{
			std::string shortPath = m_assetManager->getShortPath(folder.c_str());
			if (shortPath.size() > 0)
			{
				std::vector<std::string> result;
				CStringImp::splitString(shortPath.c_str(), "/", result);

				GUI::CTreeNode* node = m_nodeAssets;

				wchar_t wname[512];

				for (u32 i = 0, n = (u32)result.size(); i < n; i++)
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
					m_nodeAssets->deselectAll(false);
					node->forceLayout();
					node->setSelected(true, false);
					m_treeFS->getScrollControl()->scrollToItem(node);
				}
			}
		}

		void CTreeFSController::OnKeyPress(GUI::CBase* control, int key, bool press)
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
						BIND_LISTENER(&CTreeFSController::OnRename, this),
						BIND_LISTENER(&CTreeFSController::OnCancelRename, this)
					);
				}
			}
		}

		void CTreeFSController::OnRename(GUI::CBase* control)
		{
			m_treeFS->focus();
		}

		void CTreeFSController::OnCancelRename(GUI::CBase* control)
		{
			m_treeFS->focus();
		}
	}
}