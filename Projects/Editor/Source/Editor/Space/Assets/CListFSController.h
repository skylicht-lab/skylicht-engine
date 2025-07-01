/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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

#pragma once

#include "GUI/GUI.h"
#include "AssetManager/CAssetManager.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpace;

		class CTreeFSController;

		class CSearchAssetController;

		class CListFSController
		{
		protected:
			CSpace* m_space;

			GUI::CCanvas* m_canvas;

			GUI::CListBase* m_listFS;

			GUI::CButton* m_renameItem;

			std::wstring m_renameRevert;

			CTreeFSController* m_treeController;

			CSearchAssetController* m_searchController;

			CAssetManager* m_assetManager;

			std::string m_currentFolder;

			std::string m_selectSearchPath;

			std::string m_selectPath;

			GUI::CMessageBox* m_msgBox;

			GUI::CListItemBase* m_newFolderItem;

			bool m_searching;

		public:
			CListFSController(GUI::CCanvas* canvas, CSpace* space, GUI::CListBase* list);

			virtual ~CListFSController();

			void setListUI(GUI::CListBase* list);

			void OnPress(GUI::CBase* item);

			void OnSelected(GUI::CBase* item);

			void OnKeyPress(GUI::CBase* control, int key, bool press);

			void OnFileOpen(GUI::CBase* node);

			void refresh();

			void browse(const char* folder);

			void setCurrentFolder(const char* folder);

			inline void setTreeController(CTreeFSController* treeController)
			{
				m_treeController = treeController;
			}

			inline void setSearchController(CSearchAssetController* searchController)
			{
				m_searchController = searchController;
			}

			void rename(GUI::CButton* node);

			void initDragDrop(GUI::CButton* item);

			void OnRename(GUI::CBase* control);

			void OnCancelRename(GUI::CBase* control);

			void removePath(const char* path);

			GUI::CBase* scrollAndSelectPath(const char* path);

			void newFolder(const char* parent);

			void OnRenameFolder(GUI::CBase* control);

			void OnCancelRenameFolder(GUI::CBase* control);

			inline const std::string& getCurrentFolder()
			{
				return m_currentFolder;
			}

			void enableSearching(bool b)
			{
				m_searching = b;
				if (b == true)
					m_selectSearchPath = "";
			}

			inline bool isSearching()
			{
				return m_searching;
			}

			const std::string& getSelectPath()
			{
				return m_selectPath;
			}

		public:

			void add(const std::string& currentFolder, std::vector<SFileInfo>& files, bool scrollToBegin);

			GUI::ESystemIcon getFileIcon(const std::string& name, GUI::SGUIColor& color);
		};
	}
}