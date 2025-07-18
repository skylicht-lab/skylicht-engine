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
#include "CListFSController.h"
#include "CTreeFSController.h"
#include "AssetManager/CAssetManager.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpace;
		class CContextMenuFS
		{
		protected:
			CSpace* m_space;

			GUI::CCanvas* m_canvas;

			GUI::CTreeControl* m_treeFS;
			CTreeFSController* m_treeFSController;

			GUI::CListBase* m_listFS;
			CListFSController* m_listFSController;

			GUI::CMenu* m_contextMenu;

			GUI::CMenuItem* m_open;
			GUI::CMenuItem* m_showExplorer;
			GUI::CMenuSeparator* m_space1;
			GUI::CMenuItem* m_newFolder;
			GUI::CMenuSeparator* m_space2;
			GUI::CMenuItem* m_delete;
			GUI::CMenuItem* m_rename;
			GUI::CMenuItem* m_copyPath;
			GUI::CMenuItem* m_duplicate;

			GUI::CBase* m_ownerControl;
			GUI::CBase* m_selected;
			std::string m_selectedPath;

			CAssetManager* m_assetManager;
			GUI::CMessageBox* m_msgBox;

		public:
			CContextMenuFS(GUI::CCanvas* canvas,
				CSpace* space,
				GUI::CTreeControl* tree,
				CTreeFSController* treeFSController,
				GUI::CListBase* list,
				CListFSController* listFSController);

			void setListUI(GUI::CListBase* listUI);

			virtual ~CContextMenuFS();

			void OnTreeContextMenu(GUI::CBase* row);

			void OnListContextMenu(GUI::CBase* row);

			void OnCommand(GUI::CBase* item);

			void OnCopyPath(GUI::CBase* item);
		};
	}
}