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
#include "Graphics2D/CCanvas.h"
#include "Graphics2D/CGUIExporter.h"
#include "Graphics2D/CGUIImporter.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CGUIDesignController);

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
			m_guiCanvas = guiCanvas->getComponent<CCanvas>();

			CGUIElement* root = m_guiCanvas->getRootElement();
			CGUIElement* fullRect;

			if (root->getChilds().size() == 0)
			{
				fullRect = m_guiCanvas->createRect(SColor(255, 0, 0, 0));
				fullRect->setDock(EGUIDock::DockFill);
				fullRect->setName("Canvas");
			}
			else
			{
				fullRect = root->getChilds()[0];
				fullRect->invalidate();
			}

			root->setDock(EGUIDock::NoDock);
			root->setRect(core::rectf(0.0f, 0.0f, 1920.0f, 1080.0f));

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
			if (r.getWidth() > 200.0f)
				r.LowerRightCorner.X = 200.0f;
			if (r.getHeight() > 200.0f)
				r.LowerRightCorner.Y = 200.0f;

			SColor c(255, 255, 255, 255);

			std::wstring nameHint;

			if (command == L"GUI Element")
			{
				nameHint = L"GUI";
				newNode = parentNode->getCanvas()->createElement(parentNode, r);
			}
			else if (command == L"GUI Rect")
			{
				nameHint = L"Rect";
				newNode = parentNode->getCanvas()->createRect(parentNode, r, c);
			}
			else if (command == L"GUI Image")
			{
				nameHint = L"Img";
				newNode = parentNode->getCanvas()->createImage(parentNode, r);
			}
			else if (command == L"GUI Sprite")
			{
				nameHint = L"Sprite";
				CGUISprite* sprite = parentNode->getCanvas()->createSprite(parentNode, r, NULL);
				sprite->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"skylicht-icon",
					"SampleGUI/!Sprites/skylicht-icon.png"
				);
				newNode = sprite;
			}
			else if (command == L"GUI Text")
			{
				nameHint = L"Txt";
				r.LowerRightCorner.Y = 50.0f;
				CGUIText* textNode = parentNode->getCanvas()->createText(parentNode, r, NULL);
				textNode->setText("Enter text here");
				textNode->setFontSource("SampleGUI/Fonts/Roboto.font");
				newNode = textNode;
			}
			else if (command == L"GUI Mask")
			{
				nameHint = L"Mask";
				newNode = parentNode->getCanvas()->createMask(parentNode, r);
			}
			else if (command == L"GUI Layout")
			{
				nameHint = L"Layout";
				newNode = parentNode->getCanvas()->createLayout(parentNode, r);
			}
			else if (command == L"GUI Fit Sprite")
			{
				r.LowerRightCorner.X = 200.0f;
				r.LowerRightCorner.Y = 200.0f;

				nameHint = L"Sprite";
				CGUIFitSprite* fitSprite = parentNode->getCanvas()->createFitSprite(parentNode, r, NULL);
				fitSprite->setFrameSource(
					"SampleGUI/SampleGUI.spritedata", 
					"popup",
					"SampleGUI/!Sprites/popup.png"
				);
				fitSprite->setAnchor(CGUIFitSprite::AnchorAll, 20.0, 20.0f, 20.0f, 20.0f);

				newNode = fitSprite;
			}
			else if (command == L"UI Button" || command == L"UI Button (Shiny)")
			{
				r.LowerRightCorner.X = 180.0f;
				r.LowerRightCorner.Y = 70.0f;

				nameHint = L"Btn";
				newNode = parentNode->getCanvas()->createElement(parentNode, r);

				CGUIFitSprite* bg = parentNode->getCanvas()->createFitSprite(newNode, r, NULL);
				bg->setName(L".background");
				bg->setFrameSource(
					"SampleGUI/SampleGUI.spritedata", 
					"button-small",
					"SampleGUI/!Sprites/button-small.png");
				bg->setDock(EGUIDock::DockFill);
				bg->setAnchor(CGUIFitSprite::AnchorAll, 35.0, 35.0f, 35.0f, 35.0f);

				if (command == L"UI Button (Shiny)")
					bg->setMaterialSource("BuiltIn/Textures/Shiny.mat");

				CGUIText* text = parentNode->getCanvas()->createText(newNode, r, NULL);
				text->setName(L".text");
				text->setText(L"Button");
				text->setDock(EGUIDock::DockFill);
				text->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
				text->setFontSource("SampleGUI/Fonts/Roboto-Bold.font");
			}

			if (newNode)
			{
				std::wstring newName = nameHint + L" " + newNode->getNameW();
				newNode->setName(newName.c_str());

				CGUIHierachyNode* node = parent->addChild();
				node->setName(newName.c_str());
				node->setTagData(newNode, CGUIHierachyNode::GUIElement);

				setNodeEvent(node);

				if (newNode->getChilds().size() > 0)
				{
					rebuildGUIHierachy(newNode, node);
				}

				if (m_spaceHierarchy != NULL)
					m_spaceHierarchy->addToTreeNode(node);
			}
		}

		void CGUIDesignController::deselectAllOnHierachy()
		{
			CScene* scene = CSceneController::getInstance()->getScene();
			if (scene)
			{
				CGameObject* guiCanvas = scene->searchObjectInChild(L"GUICanvas");
				CCanvas* canvas = guiCanvas->getComponent<CCanvas>();

				std::vector<CSelectObject*>& listSelected = CSelection::getInstance()->getSelected();
				for (CSelectObject* obj : listSelected)
				{
					if (obj->getType() == CSelectObject::GUIElement)
					{
						CGUIElement* gui = canvas->getGUIByID(obj->getID().c_str());
						if (gui)
							gui->setDrawBorder(false);
					}
				}
			}

			if (m_spaceHierarchy)
				m_spaceHierarchy->deselectAll();
		}

		CGUIHierachyNode* CGUIDesignController::deselectOnHierachy(CGUIElement* element)
		{
			CGUIHierachyNode* node = m_rootNode->getNodeByTag(element);
			if (node != NULL)
			{
				GUI::CTreeNode* treeNode = node->getGUINode();
				treeNode->setSelected(false);
			}
			return node;
		}

		CGUIHierachyNode* CGUIDesignController::selectOnHierachy(CGUIElement* element)
		{
			CGUIHierachyNode* node = m_rootNode->getNodeByTag(element);
			if (node != NULL)
			{
				GUI::CTreeNode* treeNode = node->getGUINode();
				treeNode->setSelected(true);
				m_spaceHierarchy->scrollToNode(treeNode);
			}
			return node;
		}

		void CGUIDesignController::onDelete()
		{
			std::vector<CGUIElement*> selelectList;
			std::vector<CGUIElement*> deleteList;

			std::vector<CSelectObject*>& selected = CSelection::getInstance()->getAllSelected();
			CPropertyController::getInstance()->setProperty(NULL);
			for (CSelectObject* sel : selected)
			{
				if (sel->getType() == CSelectObject::GUIElement)
				{
					CGUIElement* gui = m_guiCanvas->getGUIByID(sel->getID().c_str());
					if (gui)
					{
						selelectList.push_back(gui);
					}
				}
			}

			std::sort(selelectList.begin(), selelectList.end(), [](CGUIElement*& a, CGUIElement*& b)
				{
					return a->getDepth() < b->getDepth();
				});

			CGUIElement* root = m_guiCanvas->getRootElement();
			CGUIElement* fullRect = root->getChilds()[0];

			for (CGUIElement* sel : selelectList)
			{
				if (sel == fullRect)
					continue;

				bool addToDeleteList = true;
				for (CGUIElement* del : deleteList)
				{
					if (sel->isChild(del))
					{
						addToDeleteList = false;
						break;
					}
				}

				if (addToDeleteList)
				{
					deleteList.push_back(sel);
				}
			}

			for (CGUIElement* del : deleteList)
			{
				del->remove();
			}

			if (deleteList.size() > 0)
			{
				CSelection::getInstance()->clear();
				rebuildGUIHierachy();
			}
		}

		void CGUIDesignController::setNodeEvent(CGUIHierachyNode* node)
		{
			node->OnUpdate = std::bind(&CGUIDesignController::onUpdateNode, this, std::placeholders::_1);
			node->OnSelected = std::bind(&CGUIDesignController::onSelectNode, this, std::placeholders::_1, std::placeholders::_2);
		}

		void CGUIDesignController::refresh()
		{

		}

		void CGUIDesignController::onUpdateNode(CGUIHierachyNode* node)
		{
			// Update name
			CGUIElement* obj = (CGUIElement*)node->getTagData();
			obj->setName(node->getName().c_str());

			// Update property
			CSelection* selection = CSelection::getInstance();
			CSelectObject* selectedObject = selection->getLastSelected();
			if (selectedObject != NULL)
			{
				CPropertyController* propertyController = CPropertyController::getInstance();
				propertyController->setProperty(selectedObject);
			}
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
			if (selected != NULL && from != this && selected->getType() == CSelectObject::GUIElement)
			{
				const std::string& id = selected->getID();

				CScene* scene = CSceneController::getInstance()->getScene();
				if (!scene)
					return;

				CGameObject* guiCanvas = scene->searchObjectInChild(L"GUICanvas");
				CCanvas* canvas = guiCanvas->getComponent<CCanvas>();
				CGUIElement* gui = canvas->getGUIByID(id.c_str());
				if (gui)
				{
					CGUIHierachyNode* guiNode = m_spaceHierarchy->getController()->getNodeByObject(gui);
					if (guiNode != NULL)
					{
						// Update new name on Hierarchy
						guiNode->setName(gui->getNameW().c_str());
					}
				}
			}
		}

		void CGUIDesignController::save(const char* path)
		{
			// Save the canvas infomation to file
			if (CGUIExporter::save(path, m_guiCanvas))
			{
				m_guiFilePath = path;
			}
		}

		void CGUIDesignController::loadFile(const std::string& path)
		{
			CEditor* editor = CEditor::getInstance();

			// clear selection
			deselectAllOnHierachy();

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

				m_guiFilePath = path;
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

			// refresh editor
			CEditor::getInstance()->refresh();
		}
	
		void CGUIDesignController::onCopy()
		{
		
		}
		
		void CGUIDesignController::onPaste()
		{
			
		}
		
		void CGUIDesignController::onDuplicate()
		{
			
		}
		
		void CGUIDesignController::onCut()
		{
			
		}
	}
}
