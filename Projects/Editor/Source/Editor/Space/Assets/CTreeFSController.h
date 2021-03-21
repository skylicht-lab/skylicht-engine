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
		class CListFSController;

		class CTreeFSController
		{
		protected:
			GUI::CCanvas* m_canvas;

			GUI::CTreeControl* m_treeFS;

			CAssetManager* m_assetManager;

			GUI::CTreeNode* m_nodeAssets;

			GUI::CTreeNode* m_renameNode;

			CListFSController* m_listController;

			GUI::CMessageBox* m_msgBox;
		public:
			CTreeFSController(GUI::CCanvas* canvas, GUI::CTreeControl* treeFS);

			virtual ~CTreeFSController();

			void OnExpand(GUI::CBase* node);

			void OnCollapse(GUI::CBase* node);

			void OnSelected(GUI::CBase* node);

			void OnKeyPress(GUI::CBase* control, int key, bool press);

			void OnRename(GUI::CBase* control);

			void OnCancelRename(GUI::CBase* control);

			void setListController(CListFSController* listController)
			{
				m_listController = listController;
			}

			void rename(GUI::CTreeNode* node);

			void removePath(const char* path);

			bool removePath(GUI::CTreeNode* node, const char* path);

		public:
			void expand(const std::string& folder);

			void add(GUI::CTreeNode* node, std::vector<SFileInfo>& files);
		};
	}
}