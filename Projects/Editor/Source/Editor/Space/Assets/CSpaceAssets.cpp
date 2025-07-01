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

#include "pch.h"
#include "CSpaceAssets.h"
#include "Utils/CStringImp.h"

#include "Editor/SpaceController/CAssetPropertyController.h"
#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceAssets::CSpaceAssets(GUI::CWindow* window, CEditor* editor) :
			m_isLock(false),
			m_currentView(0),
			CSpace(window, editor)
		{
			GUI::CToolbar* toolbar = new GUI::CToolbar(window);

			m_btnAdd = toolbar->addButton(L"Add", GUI::ESystemIcon::Plus);

			toolbar->addSpace();
			m_btnViewList = toolbar->addButton(L"", GUI::ESystemIcon::ViewList);
			m_btnViewList->setIsToggle(true);
			m_btnViewList->setToggle(true);
			m_btnViewThumbnail = toolbar->addButton(L"", GUI::ESystemIcon::ViewThumbnail);
			m_btnViewThumbnail->setIsToggle(true);

			m_btnNewWin = toolbar->addButton(L"", GUI::ESystemIcon::Windows, true);
			m_btnNewWin->OnPress = BIND_LISTENER(&CSpaceAssets::onNewWindow, this);

			m_btnLockUnlock = toolbar->addButton(L"", GUI::ESystemIcon::Unlock, true);
			m_btnLockUnlock->setIsToggle(true);
			m_btnLockUnlock->OnPress = BIND_LISTENER(&CSpaceAssets::onLockUnLock, this);

			m_inputSearch = new GUI::CTextBox(toolbar);
			m_inputSearch->setWidth(200.0f);
			m_inputSearch->showIcon();
			m_inputSearch->setStringHint(L"Search");
			toolbar->addControl(m_inputSearch, true);

			GUI::CSplitter* spliter = new GUI::CSplitter(window);
			spliter->dock(GUI::EPosition::Fill);
			spliter->setNumberRowCol(1, 2);

			m_treeFS = new GUI::CTreeControl(spliter);
			m_treeFSController = new CTreeFSController(window->getCanvas(), this, m_treeFS);

			m_listContainer = new GUI::CBase(spliter);

			GUI::CBase* searchInfo = new GUI::CBase(m_listContainer);
			searchInfo->setHeight(20.0f);
			searchInfo->dock(GUI::EPosition::Top);
			searchInfo->enableRenderFillRect(true);
			searchInfo->setFillRectColor(GUI::ThemeConfig::WindowInnerColor);
			searchInfo->setHidden(true);

			m_labelSearch = new GUI::CLabel(searchInfo);
			m_labelSearch->setString("Search asset: ");
			m_labelSearch->setPadding(GUI::SPadding(5.0f, 3.0f, 0.0f, 0.0f));
			m_labelSearch->dock(GUI::EPosition::Left);
			m_labelSearch->sizeToContents();

			m_buttonCancelSearch = new GUI::CIconButton(searchInfo);
			m_buttonCancelSearch->setMargin(GUI::SMargin(5.0f, 0.0f, 0.0f, 0.0f));
			m_buttonCancelSearch->setIcon(GUI::ESystemIcon::Close);
			m_buttonCancelSearch->dock(GUI::EPosition::Left);

			m_listFS = new GUI::CListBox(m_listContainer);
			m_listFS->dock(GUI::EPosition::Fill);

			m_listFSController = new CListFSController(window->getCanvas(), this, m_listFS);
			m_listFSController->setTreeController(m_treeFSController);

			m_searchController = new CSearchAssetController(m_inputSearch, searchInfo, m_labelSearch, m_buttonCancelSearch, m_listFSController);

			m_treeFSController->setListController(m_listFSController);
			m_treeFSController->setSearchController(m_searchController);

			m_listFSController->setSearchController(m_searchController);

			spliter->setControl(m_treeFS, 0, 0);
			spliter->setControl(m_listContainer, 0, 1);

			spliter->setColWidth(0, 300.0f);
			spliter->setWeakCol(1);

			m_contextMenuFS = new CContextMenuFS(window->getCanvas(), this, m_treeFS, m_treeFSController, m_listFS, m_listFSController);
			m_contextMenuAdd = new CContextMenuAdd(window->getCanvas(), m_listFSController);

			m_btnAdd->OnDown = [contextMenu = m_contextMenuAdd](GUI::CBase* sender)
				{
					contextMenu->popupMenu((GUI::CButton*)sender);
				};

			m_btnViewList->OnDown = BIND_LISTENER(&CSpaceAssets::onChangeView, this);
			m_btnViewThumbnail->OnDown = BIND_LISTENER(&CSpaceAssets::onChangeView, this);
		}

		CSpaceAssets::~CSpaceAssets()
		{
			delete m_treeFSController;
			delete m_listFSController;
			delete m_contextMenuFS;
			delete m_contextMenuAdd;
			delete m_searchController;
		}

		void CSpaceAssets::update()
		{
			m_searchController->update();
		}

		void CSpaceAssets::refresh()
		{
			m_listFSController->refresh();
			m_treeFSController->refresh();
		}

		void CSpaceAssets::onChangeView(GUI::CBase* base)
		{
			std::vector<GUI::CButton*> btns;
			btns.push_back(m_btnViewList);
			btns.push_back(m_btnViewThumbnail);

			int view = 0;
			int i = 0;

			for (GUI::CButton* b : btns)
			{
				if (b == base)
				{
					b->setToggle(true);
					view = i;
				}
				else
					b->setToggle(false);

				i++;
			}

			if (m_currentView != view)
			{
				m_currentView = view;
				m_listFS->remove();

				if (base == m_btnViewList)
					m_listFS = new GUI::CListBox(m_listContainer);
				else if (base == m_btnViewThumbnail)
					m_listFS = new GUI::CThumbnailView(m_listContainer, 96.0f + 24.0f, 96.0f);

				m_listFS->dock(GUI::EPosition::Fill);
				m_listFSController->setListUI(m_listFS);

				m_listContainer->invalidate();
				m_listContainer->recurseLayout();

				m_listFSController->refresh();

				const std::string& selectPath = m_listFSController->getSelectPath();
				if (!selectPath.empty())
					m_listFSController->scrollAndSelectPath(selectPath.c_str());
			}
		}

		void CSpaceAssets::onNewWindow(GUI::CBase* base)
		{
			CEditor::getInstance()->openWorkspace(L"Assets");
		}

		void CSpaceAssets::onLockUnLock(GUI::CBase* lock)
		{
			m_isLock = !m_isLock;
			m_btnLockUnlock->setToggle(m_isLock);
			m_btnLockUnlock->setIcon(m_isLock ? GUI::ESystemIcon::Lock : GUI::ESystemIcon::Unlock);
		}
	}
}