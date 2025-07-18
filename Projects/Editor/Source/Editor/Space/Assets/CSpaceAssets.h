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

#include "SkylichtEngine.h"
#include "Editor/Space/CSpace.h"
#include "AssetManager/CAssetManager.h"
#include "CTreeFSController.h"
#include "CListFSController.h"
#include "CContextMenuFS.h"
#include "CContextMenuAdd.h"
#include "CSearchAssetController.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceAssets : public CSpace
		{
		protected:
			GUI::CTreeControl* m_treeFS;
			CTreeFSController* m_treeFSController;

			GUI::CBase* m_listContainer;
			GUI::CListBase* m_listFS;
			CListFSController* m_listFSController;

			CContextMenuFS* m_contextMenuFS;
			CContextMenuAdd* m_contextMenuAdd;
			CSearchAssetController* m_searchController;

			GUI::CButton* m_btnAdd;
			GUI::CButton* m_btnViewList;
			GUI::CButton* m_btnViewThumbnail;

			GUI::CButton* m_btnNewWin;
			GUI::CButton* m_btnLockUnlock;

			GUI::CTextBox* m_inputSearch;
			GUI::CLabel* m_labelSearch;
			GUI::CIconButton* m_buttonCancelSearch;

			bool m_isLock;
			int m_currentView;

		public:
			CSpaceAssets(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceAssets();

			virtual void update();

			virtual void refresh();

			CTreeFSController* getTreeController()
			{
				return m_treeFSController;
			}

			CListFSController* getListController()
			{
				return m_listFSController;
			}

			inline bool isLock()
			{
				return m_isLock;
			}

		protected:

			void onChangeView(GUI::CBase* base);

			void onNewWindow(GUI::CBase* base);

			void onLockUnLock(GUI::CBase* lock);

		};
	}
}