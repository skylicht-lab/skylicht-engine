/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
#include "CSpaceGUIHierarchy.h"

#include "Editor/SpaceController/CGUIDesignController.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceGUIHierarchy::CSpaceGUIHierarchy(GUI::CWindow* window, CEditor* editor) :
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

			m_hierarchyController = new CGUIHierarchyController(window->getCanvas(), m_tree, m_editor);

			CGUIDesignController::getInstance()->setSpaceHierarchy(this);
			CGUIDesignController::getInstance()->rebuildGUIHierachy();
		}

		CSpaceGUIHierarchy::~CSpaceGUIHierarchy()
		{
			CGUIDesignController* controller = CGUIDesignController::getInstance();
			if (controller->getSpaceHierarchy() == this)
				controller->setSpaceHierarchy(NULL);

			delete m_hierarchyController;
		}

		void CSpaceGUIHierarchy::update()
		{

		}

		void CSpaceGUIHierarchy::deselectAll()
		{
			m_tree->deselectAll();
		}

		void CSpaceGUIHierarchy::setTreeNode(CGUIHierachyNode* node)
		{
			m_hierarchyController->setTreeNode(node);
		}

		void CSpaceGUIHierarchy::addToTreeNode(CGUIHierachyNode* node)
		{
		}

		void CSpaceGUIHierarchy::rename(CGUIHierachyNode* node)
		{
		}

		void CSpaceGUIHierarchy::scrollToNode(GUI::CTreeNode* node)
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