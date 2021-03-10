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
#include "CListFSController.h"
#include "Utils/CPath.h"

namespace Skylicht
{
	namespace Editor
	{
		CListFSController::CListFSController(GUI::CListBox* list) :
			m_renameItem(NULL)
		{
			m_assetManager = CAssetManager::getInstance();

			m_listFS = list;
			m_listFS->OnKeyPress = std::bind(
				&CListFSController::OnKeyPress,
				this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3
			);

			std::vector<SFileInfo> files;
			m_assetManager->getRoot(files);
			add("", files);
		}

		CListFSController::~CListFSController()
		{

		}

		void CListFSController::OnKeyPress(GUI::CBase* control, int key, bool press)
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
						BIND_LISTENER(&CListFSController::OnRename, this),
						BIND_LISTENER(&CListFSController::OnCancelRename, this)
					);
				}
			}
		}

		void CListFSController::OnRename(GUI::CBase* control)
		{
			m_listFS->focus();
		}

		void CListFSController::OnCancelRename(GUI::CBase* control)
		{
			m_listFS->focus();
		}

		void CListFSController::add(const std::string& currentFolder, std::vector<SFileInfo>& files)
		{
			m_listFS->removeAllItem();

			if (currentFolder.size() > 0 &&
				currentFolder != m_assetManager->getAssetFolder())
			{
				GUI::CListRowItem* item = m_listFS->addItem(L"..", GUI::ESystemIcon::Folder);

				std::string parent = CPath::getFolderPath(currentFolder);
				item->tagString(parent);
				item->tagBool(true);
				item->OnDoubleLeftMouseClick = BIND_LISTENER(&CListFSController::OnFileOpen, this);
			}

			for (SFileInfo& f : files)
			{
				GUI::CListRowItem* item;

				if (f.IsFolder)
					item = m_listFS->addItem(f.NameW.c_str(), GUI::ESystemIcon::Folder);
				else
					item = m_listFS->addItem(f.NameW.c_str(), GUI::ESystemIcon::File);

				item->tagString(f.FullPath);
				item->tagBool(f.IsFolder);
				item->OnDoubleLeftMouseClick = BIND_LISTENER(&CListFSController::OnFileOpen, this);
			}

			m_listFS->setScrollVertical(0.0f);
		}

		void CListFSController::OnFileOpen(GUI::CBase* node)
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

					add(fullPath, files);

					// expandTreeFolder(fullPath);
				}
				else
				{
					// shell open the file
				}
			}
		}
	}
}