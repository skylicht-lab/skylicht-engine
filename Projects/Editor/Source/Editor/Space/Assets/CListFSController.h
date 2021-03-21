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

#pragma once

#include "GUI/GUI.h"
#include "AssetManager/CAssetManager.h"

namespace Skylicht
{
	namespace Editor
	{
		class CTreeFSController;

		class CListFSController
		{
		protected:
			GUI::CCanvas* m_canvas;

			GUI::CListBox* m_listFS;

			GUI::CListRowItem* m_renameItem;
			
			std::wstring m_renameRevert;

			CTreeFSController* m_treeController;

			CAssetManager* m_assetManager;

			std::string m_currentFolder;

			GUI::CMessageBox* m_msgBox;

		public:
			CListFSController(GUI::CCanvas* canvas, GUI::CListBox* list);

			virtual ~CListFSController();

			void OnKeyPress(GUI::CBase* control, int key, bool press);

			void OnRename(GUI::CBase* control);

			void OnCancelRename(GUI::CBase* control);

			void OnFileOpen(GUI::CBase* node);

			void refresh();

			void setTreeController(CTreeFSController* treeController)
			{
				m_treeController = treeController;
			}

			void rename(GUI::CListRowItem* node);

			void removePath(const char* path);

			void scrollAndSelectPath(const char* path);

		public:

			void add(const std::string& currentFolder, std::vector<SFileInfo>& files);
		};
	}
}