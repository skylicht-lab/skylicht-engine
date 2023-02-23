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
#include "Editor/SpaceController/CPropertyController.h"
#include "GUI/Utils/CDragAndDrop.h"
#include "CGUIDesignController.h"
#include "CSceneController.h"
#include "Selection/CSelection.h"

namespace Skylicht
{
	namespace Editor
	{
		CGUIDesignController::CGUIDesignController() :
			m_rootNode(NULL),
			m_spaceHierarchy(NULL),
			m_spaceDesign(NULL),
			m_contextMenu(NULL)
		{
			CAssetManager::getInstance()->registerFileLoader("gui", this);
		}

		CGUIDesignController::~CGUIDesignController()
		{
			CAssetManager::getInstance()->unRegisterFileLoader("gui", this);

			if (m_rootNode)
				delete m_rootNode;

			if (m_contextMenu)
				delete m_contextMenu;
		}

		void CGUIDesignController::initContextMenu(GUI::CCanvas* canvas)
		{
			m_canvas = canvas;
			m_contextMenu = new CContextMenuGUIElement(canvas);
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
			CGUIElement* fullRect = root->getChilds()[0];

			m_rootNode = new CGUIHierachyNode(NULL);
			m_rootNode->setTagData(fullRect, CGUIHierachyNode::Canvas);
			m_rootNode->setName(fullRect->getNameW().c_str());

			setNodeEvent(m_rootNode);

			rebuildGUIHierachy(fullRect, m_rootNode);

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

				setNodeEvent(node);

				rebuildGUIHierachy(element, node);
			}
		}

		void CGUIDesignController::createGUINode(CGUIHierachyNode* parent, const std::wstring& command)
		{
			CGUIElement* parentNode = (CGUIElement*)parent->getTagData();
			CGUIElement* newNode = NULL;

			core::rectf r = parentNode->getRect();
			SColor c(255, 255, 255, 255);

			if (command == L"GUI Element")
			{
				newNode = parentNode->getCanvas()->createElement(parentNode, r);
			}
			else if (command == L"GUI Rect")
			{
				newNode = parentNode->getCanvas()->createRect(parentNode, r, c);
			}
			else if (command == L"GUI Image")
			{
				newNode = parentNode->getCanvas()->createImage(parentNode, r);
			}
			else if (command == L"GUI Text")
			{
			}
			else if (command == L"GUI Mask")
			{
				newNode = parentNode->getCanvas()->createMask(parentNode, r);
			}

			if (newNode)
			{
				CGUIHierachyNode* node = parent->addChild();
				node->setName(newNode->getNameW().c_str());
				node->setTagData(newNode, CGUIHierachyNode::GUIElement);

				setNodeEvent(node);

				if (m_spaceHierarchy != NULL)
					m_spaceHierarchy->addToTreeNode(node);
			}
		}

		void CGUIDesignController::setNodeEvent(CGUIHierachyNode* node)
		{
			node->OnUpdate = std::bind(&CGUIDesignController::onUpdateNode, this, std::placeholders::_1);
			node->OnSelected = std::bind(&CGUIDesignController::onSelectNode, this, std::placeholders::_1, std::placeholders::_2);
		}

		void CGUIDesignController::onUpdateNode(CGUIHierachyNode* node)
		{

		}

		void CGUIDesignController::onSelectNode(CGUIHierachyNode* node, bool selected)
		{
			CSelection* selection = CSelection::getInstance();

			// remove last observer
			CSelectObject* selectedObject = selection->getLastSelected();
			if (selectedObject != NULL)
				selectedObject->removeAllObserver();

			CGUIElement* obj = (CGUIElement*)node->getTagData();

			// Set property & event
			CPropertyController* propertyController = CPropertyController::getInstance();
			if (selected)
			{
				selectedObject = selection->addSelect(obj);
				propertyController->setProperty(selectedObject);

				// add new observer
				selectedObject->addObserver(this);

			}
			else
			{
				propertyController->setProperty(NULL);
				selection->unSelect(obj);
			}
		}

		void CGUIDesignController::onContextMenu(CGUIHierachyNode* node)
		{
			m_contextMenu->onContextMenu(m_spaceHierarchy, node);
		}

		void CGUIDesignController::onNotify(ISubject* subject, IObserver* from)
		{
			CSelectObject* selected = dynamic_cast<CSelectObject*>(subject);
			if (selected != NULL && from != this)
			{

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