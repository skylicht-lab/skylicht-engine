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

#include "SkylichtEngine.h"
#include "AssetManager/CAssetManager.h"
#include "Editor/Space/CSpace.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceAssets : public CSpace
		{
		protected:
			GUI::CTreeControl* m_folder;

			GUI::CTreeNode* m_root;

			GUI::CTreeNode* m_renameNode;

			GUI::CListBox* m_listFiles;

			GUI::CTextBox* m_search;

			GUI::CListRowItem* m_renameItem;

			CAssetManager* m_assetManager;

		public:
			CSpaceAssets(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceAssets();

			void OnTreeNodeExpand(GUI::CBase* node);

			void OnTreeNodeCollapse(GUI::CBase* node);

			void OnTreeNodeSelected(GUI::CBase* node);

			void OnFileOpen(GUI::CBase* node);

			void OnTreeNodeKeyPress(GUI::CBase* control, int key, bool press);

			void OnTreeRename(GUI::CBase* control);

			void OnTreeCancelRename(GUI::CBase* control);

			void OnListKeyPress(GUI::CBase* control, int key, bool press);

			void OnListRename(GUI::CBase* control);

			void OnListCancelRename(GUI::CBase* control);

		protected:

			void expandTreeFolder(const std::string& folder);

			void addTreeFolder(GUI::CTreeNode* node, std::vector<SFileInfo>& files);

			void addListFolder(const std::string& currentFolder, std::vector<SFileInfo>& files);
		};
	}
}