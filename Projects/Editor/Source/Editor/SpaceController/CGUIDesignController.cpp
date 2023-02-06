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
#include "Editor/CEditor.h"
#include "Editor/Space/GUIDesign/CSpaceGUIDesign.h"
#include "GUI/Utils/CDragAndDrop.h"
#include "CGUIDesignController.h"
#include "CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		CGUIDesignController::CGUIDesignController() :
			m_rootNode(NULL),
			m_spaceHierarchy(NULL),
			m_spaceDesign(NULL)
		{
			CAssetManager::getInstance()->registerFileLoader("gui", this);
		}

		CGUIDesignController::~CGUIDesignController()
		{
			CAssetManager::getInstance()->unRegisterFileLoader("gui", this);

			if (m_rootNode)
				delete m_rootNode;
		}

		void CGUIDesignController::initContextMenu(GUI::CCanvas* canvas)
		{
			m_canvas = canvas;
		}

		void CGUIDesignController::rebuildGUIHierachy()
		{
			CScene* scene = CSceneController::getInstance()->getScene();
			if (!scene)
				return;

			if (m_rootNode)
				delete m_rootNode;

			CGameObject* guiCanvas = scene->searchObjectInChild(L"GUICanvas");
			CCanvas* canvas = guiCanvas->getComponent<CCanvas>();

			CGUIElement* root = canvas->getRootElement();
			m_rootNode = new CGUIHierachyNode(NULL);
			m_rootNode->setTagData(root, CGUIHierachyNode::Canvas);
			m_rootNode->setName(root->getNameW().c_str());

			rebuildGUIHierachy(root, m_rootNode);

			if (m_spaceHierarchy)
				m_spaceHierarchy->setTreeNode(m_rootNode);
		}

		void CGUIDesignController::rebuildGUIHierachy(CGUIElement* parent, CGUIHierachyNode* parentNode)
		{
			std::vector<CGUIElement*>& childs = parent->getChilds();
			for (CGUIElement* element : childs)
			{
				CGUIHierachyNode* node = parentNode->addChild();
				node->setTagData(element, CGUIHierachyNode::GUIElement);
				node->setName(element->getNameW().c_str());

				rebuildGUIHierachy(element, node);
			}
		}

		void CGUIDesignController::loadFile(const std::string& path)
		{
			CEditor* editor = CEditor::getInstance();

			// Show GUI Design window
			CSpace* space = editor->getWorkspaceByName(std::wstring(L"GUI Design"));
			if (!space)
				space = editor->openWorkspace(std::wstring(L"GUI Design"));
			else
			{
				GUI::CDockableWindow* dockWindow = dynamic_cast<GUI::CDockableWindow*>(space->getWindow());
				if (dockWindow != NULL)
				{
					GUI::CDockTabControl* dockTab = dockWindow->getCurrentDockTab();
					if (dockTab != NULL)
					{
						// activate
						dockTab->setCurrentWindow(dockWindow);
						GUI::CDragAndDrop::cancel();
					}
				}
			}

			if (space)
			{
				CSpaceGUIDesign* spaceGUI = dynamic_cast<CSpaceGUIDesign*>(space);
				spaceGUI->openGUI(path.c_str());
			}

			// Show GUI Hierarchy window
			space = editor->getWorkspaceByName(std::wstring(L"GUI Hierarchy"));
			if (!space)
				space = editor->openWorkspace(std::wstring(L"GUI Hierarchy"));
			else
			{
				GUI::CDockableWindow* dockWindow = dynamic_cast<GUI::CDockableWindow*>(space->getWindow());
				if (dockWindow != NULL)
				{
					GUI::CDockTabControl* dockTab = dockWindow->getCurrentDockTab();
					if (dockTab != NULL)
						dockTab->setCurrentWindow(dockWindow);
				}
			}
		}
	}
}