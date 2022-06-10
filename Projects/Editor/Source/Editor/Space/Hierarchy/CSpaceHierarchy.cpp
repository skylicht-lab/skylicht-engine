/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CSpaceHierarchy.h"

#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceHierarchy::CSpaceHierarchy(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor)
		{
			GUI::CToolbar* toolbar = new GUI::CToolbar(window);

			m_btnAdd = toolbar->addButton(L"Add", GUI::ESystemIcon::Plus);

			m_inputSearch = new GUI::CTextBox(toolbar);
			m_inputSearch->setWidth(200.0f);
			m_inputSearch->showIcon();
			m_inputSearch->setStringHint(L"Search");
			toolbar->addControl(m_inputSearch, true);

			GUI::CBase* treeContainer = new GUI::CBase(window);
			treeContainer->dock(GUI::EPosition::Fill);

			GUI::CBase* searchInfo = new GUI::CBase(treeContainer);
			searchInfo->setHeight(20.0f);
			searchInfo->dock(GUI::EPosition::Top);
			searchInfo->enableRenderFillRect(true);
			searchInfo->setFillRectColor(GUI::CThemeConfig::WindowInnerColor);
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

			m_tree = new GUI::CTreeControl(treeContainer);
			m_tree->dock(GUI::EPosition::Fill);
			m_tree->setMultiSelected(true);

			m_hierarchyController = new CHierarchyController(window->getCanvas(), m_tree, m_editor);
			m_hierarchyContextMenu = new CHierachyContextMenu(m_tree);

			CSceneController* sceneController = CSceneController::getInstance();
			sceneController->setSpaceHierarchy(this);

			if (CSceneController::getInstance()->getScene() != NULL)
				sceneController->reinitHierachyData();
		}

		CSpaceHierarchy::~CSpaceHierarchy()
		{
			CSceneController* sceneController = CSceneController::getInstance();
			if (sceneController->getSpaceHierarchy() == this)
				sceneController->removeAllHierarchyNodes();

			sceneController->setSpaceHierarchy(NULL);

			delete m_hierarchyController;
			delete m_hierarchyContextMenu;
		}

		void CSpaceHierarchy::update()
		{

		}

		void CSpaceHierarchy::deselectAll()
		{
			m_tree->deselectAll();
		}

		void CSpaceHierarchy::deleteHierarchyNode()
		{
			m_hierarchyController->deleteHierarchyData();
		}

		void CSpaceHierarchy::setTreeNode(CHierachyNode* node)
		{
			m_hierarchyController->setTreeNode(node);
		}

		void CSpaceHierarchy::addToTreeNode(CHierachyNode* node)
		{
			m_hierarchyController->addToTreeNode(node);
		}

		void CSpaceHierarchy::rename(CHierachyNode* node)
		{
			m_hierarchyController->rename(node->getGUINode());
		}

		void CSpaceHierarchy::scrollToNode(GUI::CTreeNode* node)
		{
			// expand on tree control
			GUI::CTreeNode* parent = node->getParentNode();
			while (parent != m_tree)
			{
				parent->expand(true);
				parent = parent->getParentNode();
			}

			// scroll to this item
			m_tree->getScrollControl()->scrollToItem(node);
		}
	}
}