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
#include "CopyPaste/CCopyPasteUI.h"
#include "GUI/Utils/CDragAndDrop.h"
#include "CGUIDesignController.h"
#include "Selection/CSelection.h"
#include "Graphics2D/CCanvas.h"
#include "Graphics2D/CGUIFactory.h"
#include "Graphics2D/CGUIExporter.h"
#include "Graphics2D/CGUIImporter.h"

#include "Graphics2D/GUI/CGUIImage.h"
#include "Graphics2D/GUI/CGUISprite.h"
#include "Graphics2D/GUI/CGUIFitSprite.h"
#include "ParticleSystem/CGUIParticle.h"

#include "ResourceSettings/CTextureSettings.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CGUIDesignController);

		CGUIDesignController::CGUIDesignController() :
			m_rootNode(NULL),
			m_spaceHierarchy(NULL),
			m_spaceDesign(NULL),
			m_contextMenu(NULL),
			m_canvas(NULL),
			m_guiCanvas(NULL),
			m_guiCamera(NULL),
			m_scene(NULL),
			m_history(NULL)
		{
			CAssetManager::getInstance()->registerFileLoader("gui", this);
		}

		CGUIDesignController::~CGUIDesignController()
		{
			CAssetManager::getInstance()->unRegisterFileLoader("gui", this);

			if (m_history)
				delete m_history;

			if (m_rootNode)
				delete m_rootNode;

			if (m_contextMenu)
				delete m_contextMenu;
		}

		void CGUIDesignController::setScene(CScene* scene)
		{
			m_scene = scene;
			m_guiCanvas = NULL;

			if (m_scene)
			{
				CGameObject* guiCanvas = m_scene->searchObjectInChild(L"GUICanvas");
				m_guiCanvas = guiCanvas->getComponent<CCanvas>();

				CGameObject* cameraObj = scene->searchObjectInChild(L"GUICamera");
				m_guiCamera = cameraObj->getComponent<CCamera>();
			}
		}

		void CGUIDesignController::deleteScene()
		{
			if (m_scene)
			{
				delete m_scene;
				m_scene = NULL;
				m_guiCanvas = NULL;
				m_guiCamera = NULL;
			}
		}

		void CGUIDesignController::initContextMenu(GUI::CCanvas* canvas)
		{
			m_canvas = canvas;
			m_contextMenu = new CContextMenuGUIElement(canvas);
		}

		void CGUIDesignController::rebuildGUIHierachy()
		{
			if (!m_guiCanvas)
				return;

			if (m_rootNode)
				delete m_rootNode;

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

			if (m_guiFilePath.empty())
				root->setRect(core::rectf(0.0f, 0.0f, 1920.0f, 1080.0f));

			m_rootNode = new CGUIHierachyNode(NULL);
			m_rootNode->setTagData(fullRect, CGUIHierachyNode::Canvas);
			m_rootNode->setName(fullRect->getNameW().c_str());

			setNodeEvent(m_rootNode);

			rebuildGUIHierachy(fullRect, m_rootNode);

			if (m_spaceHierarchy)
				m_spaceHierarchy->setTreeNode(m_rootNode);
		}

		void CGUIDesignController::initHistory()
		{
			if (m_history)
			{
				delete m_history;
				m_history = NULL;
			}

			m_history = new CGUIEditorHistory(m_guiCanvas);
		}

		void CGUIDesignController::rebuildGUIHierachy(CGUIElement* parent, CGUIHierachyNode* parentNode)
		{
			std::vector<CGUIElement*>& childs = parent->getChilds();
			for (CGUIElement* element : childs)
			{
				CGUIHierachyNode* node = parentNode->addChild();
				node->setTagData(element, CGUIHierachyNode::GUIElement);

				CGUIHierarchyController::updateObjectToUI(element, node);

				setNodeEvent(node);

				rebuildGUIHierachy(element, node);
			}
		}

		void CGUIDesignController::createGUINode(CGUIHierachyNode* parent, const std::wstring& command)
		{
			CGUIElement* parentNode = (CGUIElement*)parent->getTagData();
			CGUIElement* newNode = NULL;

			core::rectf r = parentNode->getRect();

			r.UpperLeftCorner.X = 0.0f;
			r.UpperLeftCorner.Y = 0.0f;
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
			else if (command == L"GUI Elipse")
			{
				nameHint = L"Elipse";
				newNode = parentNode->getCanvas()->createElipse(parentNode, r, c);
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
			else if (command == L"GUI Particle")
			{
				nameHint = L"Particle";
				newNode = CGUIFactory::getInstance()->createGUI("CGUIParticle", parentNode);
				if (newNode)
				{
					newNode->setRect(r);

					Particle::CGUIParticle* guiParticle = dynamic_cast<Particle::CGUIParticle*>(newNode);
					if (guiParticle)
						guiParticle->setParticle("SampleGUI/Particle/SampleGUI.particle", true);
				}
			}
			else if (command == L"UI Button" || command == L"UI Button (Shiny)")
			{
				r.LowerRightCorner.X = 180.0f;
				r.LowerRightCorner.Y = 70.0f;

				nameHint = L"Btn";
				newNode = parentNode->getCanvas()->createElement(parentNode, r);

				CGUIFitSprite* bg = parentNode->getCanvas()->createFitSprite(newNode, r, NULL);
				bg->setName(L"Background");
				bg->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"button-small",
					"SampleGUI/!Sprites/button-small.png");
				bg->setDock(EGUIDock::DockFill);
				bg->setAnchor(CGUIFitSprite::AnchorAll, 35.0, 35.0f, 35.0f, 35.0f);

				if (command == L"UI Button (Shiny)")
					bg->setMaterialSource("BuiltIn/Textures/Shiny.mat");

				CGUIText* text = parentNode->getCanvas()->createText(newNode, r, NULL);
				text->setName(L"Text");
				text->setText(L"Button");
				text->setDock(EGUIDock::DockFill);
				text->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
				text->setFontSource("SampleGUI/Fonts/Roboto-Bold.font");
			}
			else if (command == L"UI Glow Border")
			{
				r.LowerRightCorner.X = 200.0f;
				r.LowerRightCorner.Y = 200.0f;

				nameHint = L"Border";

				CGUIFitSprite* bg = parentNode->getCanvas()->createFitSprite(parentNode, r, NULL);
				bg->setName(L"Background");
				bg->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"border_outglow",
					"SampleGUI/!Sprites/border_outglow.png");
				bg->setDock(EGUIDock::NoDock);
				bg->setAnchor(CGUIFitSprite::AnchorAll, 30.0, 30.0f, 30.0f, 30.0f);
				bg->setMaterialSource("BuiltIn/Textures/GlowBorder.mat");

				newNode = bg;
			}
			else if (command == L"UI Icon Button")
			{
				r.LowerRightCorner.X = 64.0f;
				r.LowerRightCorner.Y = 64.0f;

				nameHint = L"Btn";
				newNode = parentNode->getCanvas()->createElement(parentNode, r);

				CGUISprite* bg = parentNode->getCanvas()->createSprite(newNode, r, NULL);
				bg->setName(L"Background");
				bg->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"switch-circle-disable",
					"SampleGUI/!Sprites/switch-circle-disable.png");
				bg->setRect(core::rectf(0.0f, 0.0f, 46.0f, 46.0f));
				bg->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);

				CGUISprite* icon = parentNode->getCanvas()->createSprite(newNode, r, NULL);
				icon->setName(L"Icon");
				icon->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"triangle-l",
					"SampleGUI/!Sprites/triangle-l.png");
				icon->setRect(core::rectf(0.0f, 0.0f, 14.0f, 18.0f));
				icon->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
			}
			else if (command == L"UI Slider")
			{
				r.LowerRightCorner.X = 200.0f;
				r.LowerRightCorner.Y = 60.0f;

				nameHint = L"Slider";
				newNode = parentNode->getCanvas()->createElement(parentNode, r);

				CGUIFitSprite* bg = parentNode->getCanvas()->createFitSprite(newNode, r, NULL);
				bg->setName(L"Background");
				bg->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"slider-background",
					"SampleGUI/!Sprites/slider-background.png");
				bg->setDock(EGUIDock::DockFill);
				bg->setMargin(SMargin(0.0f, 15.0f, 0.0f, 15.0f));
				bg->setAnchor(CGUIFitSprite::AnchorAll, 15.0f, 15.0f, 15.0f, 15.0f);

				CGUIFitSprite* fillBar = parentNode->getCanvas()->createFitSprite(newNode, r, NULL);
				fillBar->setName(L"FillBar");
				fillBar->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"slider-selected",
					"SampleGUI/!Sprites/slider-selected.png");
				fillBar->setDock(EGUIDock::DockFill);
				fillBar->setMargin(SMargin(0.0f, 15.0f, 0.0f, 15.0f));
				fillBar->setAnchor(CGUIFitSprite::AnchorAll, 15.0f, 15.0f, 15.0f, 15.0f);

				r.LowerRightCorner.X = 40.0f;
				r.LowerRightCorner.Y = 40.0f;
				CGUISprite* handle = parentNode->getCanvas()->createSprite(newNode, r, NULL);
				handle->setName(L"Handle");
				handle->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"slider-circle",
					"SampleGUI/!Sprites/slider-circle.png");
				handle->setPosition(core::vector3df(180.0f, 10.0f, 0.0f));
			}
			else if (command == L"UI Input")
			{
				r.LowerRightCorner.X = 220.0f;
				r.LowerRightCorner.Y = 60.0f;

				nameHint = L"Input";
				newNode = parentNode->getCanvas()->createElement(parentNode, r);

				CGUIFitSprite* bg = parentNode->getCanvas()->createFitSprite(newNode, r, NULL);
				bg->setName(L"Background");
				bg->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"input",
					"SampleGUI/!Sprites/input.png");
				bg->setDock(EGUIDock::DockFill);
				bg->setAnchor(CGUIFitSprite::AnchorAll, 15.0f, 15.0f, 15.0f, 15.0f);

				CGUIText* text = parentNode->getCanvas()->createText(newNode, r, NULL);
				text->setName(L"Text");
				text->setText(L"Enter text here");
				text->setDock(EGUIDock::DockFill);
				text->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
				text->setMargin(SMargin(5.0f, 5.0f, 5.0f, 5.0f));
				text->setFontSource("SampleGUI/Fonts/Roboto.font");
			}
			else if (command == L"UI List")
			{
				r.LowerRightCorner.X = 220.0f;
				r.LowerRightCorner.Y = 180.0f;

				nameHint = L"List";
				newNode = parentNode->getCanvas()->createElement(parentNode, r);

				r.UpperLeftCorner.Y = 0.0f;
				r.LowerRightCorner.Y = 60.0f;

				CGUIElement* item = parentNode->getCanvas()->createElement(newNode, r);
				item->setName(L"Item");

				CGUIText* text = parentNode->getCanvas()->createText(item, r, NULL);
				text->setName(L"Text");
				text->setText(L"Item name");
				text->setDock(EGUIDock::DockFill);
				text->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
				text->setMargin(SMargin(5.0f, 5.0f, 5.0f, 5.0f));
				text->setFontSource("SampleGUI/Fonts/Roboto.font");
			}
			else if (command == L"UI Checkbox")
			{
				r.LowerRightCorner.X = 64.0f;
				r.LowerRightCorner.Y = 64.0f;

				nameHint = L"CB";
				newNode = parentNode->getCanvas()->createElement(parentNode, r);

				r.LowerRightCorner.X = 56.0f;
				r.LowerRightCorner.Y = 56.0f;

				CGUISprite* background = parentNode->getCanvas()->createSprite(newNode, r, NULL);
				background->setName(L"Background");
				background->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"checkbox",
					"SampleGUI/!Sprites/checkbox.png");
				background->setAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);

				r.UpperLeftCorner.X = -12.0f;
				r.UpperLeftCorner.Y = -12.0f;
				r.LowerRightCorner.X = 12.0f;
				r.LowerRightCorner.Y = 12.0f;
				CGUISprite* checked = parentNode->getCanvas()->createSprite(newNode, r, NULL);
				checked->setName(L"Checked");
				checked->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"checkbox-center",
					"SampleGUI/!Sprites/checkbox-center.png");
				checked->setStretch(true);
				checked->setPosition(core::vector3df(32.0f, 32.0f, 0.0f));
			}
			else if (command == L"UI Switch")
			{
				r.LowerRightCorner.X = 124.0f;
				r.LowerRightCorner.Y = 64.0f;

				nameHint = L"Btn";
				newNode = parentNode->getCanvas()->createElement(parentNode, r);

				CGUISprite* background = parentNode->getCanvas()->createSprite(newNode, r, NULL);
				background->setName(L"Background");
				background->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"switch-off",
					"SampleGUI/!Sprites/switch-off.png");

				r.LowerRightCorner.X = 46.0f;
				r.LowerRightCorner.Y = 46.0f;
				CGUISprite* handle = parentNode->getCanvas()->createSprite(newNode, r, NULL);
				handle->setName(L"HandleOff");
				handle->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"switch-circle",
					"SampleGUI/!Sprites/switch-circle.png");
				handle->setPosition(core::vector3df(10.0f, 9.0f, 0.0f));

				handle = parentNode->getCanvas()->createSprite(newNode, r, NULL);
				handle->setName(L"HandleOn");
				handle->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"switch-circle",
					"SampleGUI/!Sprites/switch-circle.png");
				handle->setPosition(core::vector3df(68.0f, 9.0f, 0.0f));
				handle->setVisible(false);
			}
			else if (command == L"UI ProgressBar")
			{
				r.LowerRightCorner.X = 220.0f;
				r.LowerRightCorner.Y = 45.0f;

				nameHint = L"Progress";
				newNode = parentNode->getCanvas()->createElement(parentNode, r);

				CGUIFitSprite* bg = parentNode->getCanvas()->createFitSprite(newNode, r, NULL);
				bg->setName(L"Background");
				bg->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"loading-background",
					"SampleGUI/!Sprites/loading-background.png");
				bg->setDock(EGUIDock::DockFill);
				bg->setAnchor(CGUIFitSprite::AnchorAll, 15.0f, 15.0f, 15.0f, 15.0f);

				CGUIFitSprite* progress = parentNode->getCanvas()->createFitSprite(newNode, r, NULL);
				progress->setName(L"Loading");
				progress->setDock(EGUIDock::DockFill);
				progress->setFrameSource(
					"SampleGUI/SampleGUI.spritedata",
					"loading",
					"SampleGUI/!Sprites/loading.png");
				progress->setAnchor(CGUIFitSprite::AnchorAll, 15.0f, 15.0f, 15.0f, 15.0f);
				progress->setMaterialSource("SampleGUI/Textures/ProgressBar.mat");
			}

			if (newNode)
			{
				std::wstring newName = nameHint + L" " + newNode->getNameW();
				newNode->setName(newName.c_str());

				createGUINode(parent, newNode);
			}
		}

		void CGUIDesignController::createGUINode(CGUIHierachyNode* parent, CGUIElement* node)
		{
			CGUIHierachyNode* guiNode = parent->addChild();
			guiNode->setName(node->getNameW().c_str());
			guiNode->setTagData(node, CGUIHierachyNode::GUIElement);

			setNodeEvent(guiNode);

			if (node->getChilds().size() > 0)
			{
				rebuildGUIHierachy(node, guiNode);
			}

			if (m_spaceHierarchy != NULL)
				m_spaceHierarchy->addToTreeNode(guiNode);

			// add history create
			std::vector<CGUIElement*> listObjects;
			listObjects.push_back(node);
			node->getAllChilds(listObjects);
			m_history->saveCreateHistory(listObjects);
		}

		void CGUIDesignController::deselectAllOnHierachy(bool callEvent)
		{
			if (m_guiCanvas)
			{
				std::vector<CSelectObject*>& listSelected = CSelection::getInstance()->getAllSelected();
				for (CSelectObject* obj : listSelected)
				{
					if (obj->getType() == CSelectObject::GUIElement)
					{
						CGUIElement* gui = m_guiCanvas->getGUIByID(obj->getID().c_str());
						if (gui)
							gui->setDrawBorder(false);
					}
				}
			}

			if (m_spaceHierarchy)
				m_spaceHierarchy->deselectAll(callEvent);
		}

		CGUIHierachyNode* CGUIDesignController::deselectOnHierachy(CGUIElement* element)
		{
			CGUIHierachyNode* node = m_rootNode->getNodeByTag(element);
			if (node != NULL)
			{
				GUI::CTreeNode* treeNode = node->getGUINode();
				if (treeNode)
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
				if (treeNode)
				{
					treeNode->setSelected(true);
					if (m_spaceHierarchy)
						m_spaceHierarchy->scrollToNode(treeNode);
				}
			}
			return node;
		}

		void CGUIDesignController::removeGUIElement(CGUIElement* node)
		{
			node->remove();

			CSelection::getInstance()->clear();
			rebuildGUIHierachy();
		}

		void CGUIDesignController::onHistoryModifyObject(CGUIElement* node)
		{
			if (m_spaceHierarchy)
				m_spaceHierarchy->getController()->updateTreeNode(node);
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
			CGUIElement* rootCanvas = root->getChilds()[0];

			for (CGUIElement* sel : selelectList)
			{
				if (sel == rootCanvas)
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

			// add history
			std::vector<CGUIElement*> saveHistory;
			for (CGUIElement* del : deleteList)
			{
				saveHistory.push_back(del);
				del->getAllChilds(saveHistory);
			}
			m_history->saveDeleteHistory(saveHistory);

			for (CGUIElement* del : deleteList)
			{
				CGUIHierachyNode* hierachyNode = m_rootNode->getNodeByTag(del);
				if (hierachyNode)
					hierachyNode->remove();

				del->remove();
			}

			if (deleteList.size() > 0)
			{
				CSelection::getInstance()->clear();
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

			CGUIElement* obj = (CGUIElement*)node->getTagData();

			// Set property & event
			CPropertyController* propertyController = CPropertyController::getInstance();
			if (selected)
			{
				CSelectObject* selectedObject = selection->addSelect(obj);
				propertyController->setProperty(selectedObject);

				// add new observer
				selectedObject->addObserver(this);

				m_history->addSelectHistory();
			}
			else
			{
				propertyController->setProperty(NULL);
				selection->unSelect(obj);
			}
		}

		void CGUIDesignController::onMoveStructure(CGUIElement* element, CGUIElement* parent, CGUIElement* before)
		{
			bool behind = true;
			if (before == NULL)
			{
				std::vector<CGUIElement*>& childs = parent->getChilds();
				if (childs.size() > 0)
				{
					before = childs[0];
					behind = false;
				}
			}

			if (before)
			{
				CGUIHierachyNode* parentNode = m_rootNode->getNodeByTag(parent);
				CGUIHierachyNode* beforeNode = m_rootNode->getNodeByTag(before);
				CGUIHierachyNode* node = m_rootNode->getNodeByTag(element);

				if (parentNode && node)
				{
					node->removeGUI();
					node->nullGUI();
					parentNode->bringToNext(node, beforeNode, behind);
					if (m_spaceHierarchy != NULL)
					{
						m_spaceHierarchy->addToTreeNode(node);
						node->getGUINode()->bringNextToControl(beforeNode ? beforeNode->getGUINode() : NULL, behind);
					}
				}
				parent->bringToNext(element, before, behind);
			}
			else
			{
				CGUIHierachyNode* container = m_rootNode->getNodeByTag(parent);
				CGUIHierachyNode* node = m_rootNode->getNodeByTag(element);
				if (node)
				{
					node->removeGUI();
					node->nullGUI();
					if (container)
					{
						container->bringToChild(node);
						if (m_spaceHierarchy != NULL)
							m_spaceHierarchy->addToTreeNode(node);
					}
				}
				parent->bringToChild(element);
			}
		}

		void CGUIDesignController::applySelected(std::vector<CSelectObject*> ids)
		{
			std::vector<CSelectObject*> listIds;

			for (CSelectObject* id : ids)
			{
				if (id->getType() == CSelectObject::GUIElement)
				{
					CGUIElement* gui = m_guiCanvas->getGUIByID(id->getID().c_str());
					if (gui)
					{
						gui->setDrawBorder(true);

						selectOnHierachy(gui, false);
						listIds.push_back(id);

						m_history->beginSaveHistory(gui);
					}
				}
			}

			CSelection::getInstance()->applySelected(listIds);
		}

		CGUIHierachyNode* CGUIDesignController::selectOnHierachy(CGUIElement* gui, bool callEvent)
		{
			CGUIHierachyNode* node = m_rootNode->getNodeByTag(gui);
			if (node != NULL)
			{
				GUI::CTreeNode* treeNode = node->getGUINode();
				if (treeNode)
				{
					treeNode->setSelected(true, callEvent);

					if (m_spaceHierarchy)
						m_spaceHierarchy->scrollToNode(treeNode);
				}
			}
			return node;
		}

		void CGUIDesignController::onContextMenu(CGUIHierachyNode* node)
		{
			if (m_spaceHierarchy)
				m_contextMenu->onContextMenu(m_spaceHierarchy, node);
		}

		void CGUIDesignController::onNotify(ISubject* subject, IObserver* from)
		{
			CSelectObject* selected = dynamic_cast<CSelectObject*>(subject);
			if (selected != NULL && from != this && selected->getType() == CSelectObject::GUIElement)
			{
				const std::string& id = selected->getID();

				if (!m_guiCanvas)
					return;

				CGUIElement* gui = m_guiCanvas->getGUIByID(id.c_str());
				if (gui)
				{
					CGUIHierachyNode* node = m_rootNode->getNodeByTag(gui);
					if (node)
						CGUIHierarchyController::updateObjectToUI(gui, node);
				}
			}
		}

		void CGUIDesignController::newGUI()
		{
			m_guiFilePath = "";
		}

		bool CGUIDesignController::needSave()
		{
			return true;
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
				m_guiFilePath = path;

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

			// refresh editor
			CEditor::getInstance()->refresh();
		}

		void CGUIDesignController::onCopy()
		{
			if (!m_guiCanvas)
				return;

			CSelection* selection = CSelection::getInstance();
			std::vector<CSelectObject*>& selected = selection->getAllSelected();

			std::vector<CGUIElement*> listUI;

			for (CSelectObject* selectObject : selected)
			{
				CSelectObject::ESelectType type = selectObject->getType();
				if (type == CSelectObject::GUIElement)
				{
					CGUIElement* gui = m_guiCanvas->getGUIByID(selectObject->getID().c_str());
					if (gui)
					{
						if (gui == m_guiCanvas->getRootElement())
							continue;
						if (gui == m_rootNode->getTagData())
							continue;
						else
							listUI.push_back(gui);
					}
				}
			}

			CCopyPasteUI::getInstance()->copy(listUI);
		}

		void CGUIDesignController::onPaste()
		{
			if (!m_guiCanvas)
				return;

			CSelection* selection = CSelection::getInstance();
			CCopyPasteUI* copyPaste = CCopyPasteUI::getInstance();

			std::vector<CGUIElement*> newUIs;

			CSelectObject* lastSelected = selection->getLastSelected();
			if (lastSelected == NULL)
			{
				CCopyPasteUI::getInstance()->paste(m_guiCanvas->getRootElement());
			}
			else
			{
				if (lastSelected->getType() == CSelectObject::GUIElement)
				{
					CGUIElement* gui = m_guiCanvas->getGUIByID(lastSelected->getID().c_str());
					if (gui)
					{
						CCopyPasteUI::getInstance()->paste(gui->getParent());
					}
					else
					{
						CCopyPasteUI::getInstance()->paste(m_guiCanvas->getRootElement());
					}
				}
			}

			selection->clear();
			selection->addSelect(newUIs);
		}

		void CGUIDesignController::onDuplicate()
		{
			onCopy();
			onPaste();
		}

		void CGUIDesignController::onCut()
		{
			onCopy();
			onDelete();
		}

		void CGUIDesignController::onUndo()
		{
			m_history->undo();
		}

		void CGUIDesignController::onRedo()
		{
			m_history->redo();
		}

		std::string CGUIDesignController::getSpritePath(const char* framePath)
		{
			std::string meta = framePath;
			meta += ".meta";

			std::string result;

			CTextureSettings* t = new CTextureSettings();
			if (t->load(meta.c_str()))
				result = t->SpritePath.get();

			delete t;
			return result;
		}

		void CGUIDesignController::syncGUID()
		{
			if (m_guiCanvas)
			{
				CAssetManager* assetMgr = CAssetManager::getInstance();

				std::vector<CGUISprite*> listSprite = m_guiCanvas->getElementsInChild<CGUISprite>(false);
				for (CGUISprite* s : listSprite)
				{
					SFileNode* frame = assetMgr->getFileNodeByGUID(s->getFrameId());
					if (frame)
					{
						std::string spritePath;
						SFileNode* sprite = assetMgr->getFileNodeByGUID(s->getSpriteId());
						if (!sprite)
							spritePath = getSpritePath(frame->Path.c_str());
						else
							spritePath = sprite->Path;

						if (!spritePath.empty())
						{
							std::string frameName = CPath::getFileNameNoExt(frame->Path);
							s->setFrameSource(spritePath.c_str(), frameName.c_str(), frame->Path.c_str());
						}
					}
				}

				std::vector<CGUIFitSprite*> listFitSprite = m_guiCanvas->getElementsInChild<CGUIFitSprite>(false);
				for (CGUIFitSprite* s : listFitSprite)
				{
					SFileNode* frame = assetMgr->getFileNodeByGUID(s->getFrameId());
					if (frame)
					{
						std::string spritePath;
						SFileNode* sprite = assetMgr->getFileNodeByGUID(s->getSpriteId());
						if (!sprite)
							spritePath = getSpritePath(frame->Path.c_str());
						else
							spritePath = sprite->Path;

						if (!spritePath.empty())
						{
							std::string frameName = CPath::getFileNameNoExt(frame->Path);
							s->setFrameSource(spritePath.c_str(), frameName.c_str(), frame->Path.c_str());
						}
					}
				}

				std::vector<CGUIImage*> listImages = m_guiCanvas->getElementsInChild<CGUIImage>(false);
				for (CGUIImage* i : listImages)
				{
					SFileNode* img = assetMgr->getFileNodeByGUID(i->getResourceId());
					if (img)
						i->setImageResource(img->Path.c_str(), img->GUID.c_str());
				}

				std::vector<CGUIText*> listTexts = m_guiCanvas->getElementsInChild<CGUIText>(false);
				for (CGUIText* t : listTexts)
				{
					SFileNode* font = assetMgr->getFileNodeByGUID(t->getFontId().c_str());
					if (font)
						t->setFontSource(font->Path.c_str());
				}
			}
		}

		void CGUIDesignController::doSpriteChange(const char* resource)
		{
			if (m_guiCanvas)
			{
				std::vector<CGUISprite*> listSprite = m_guiCanvas->getElementsInChild<CGUISprite>(false);
				for (CGUISprite* s : listSprite)
					s->reloadSpriteFrame();

				std::vector<CGUIFitSprite*> listFitSprite = m_guiCanvas->getElementsInChild<CGUIFitSprite>(false);
				for (CGUIFitSprite* s : listFitSprite)
					s->reloadSpriteFrame();
			}
		}

		void CGUIDesignController::doReplaceTexture(ITexture* oldTexture, ITexture* newTexture)
		{
			if (m_guiCanvas)
			{
				std::vector<CGUIImage*> listImages = m_guiCanvas->getElementsInChild<CGUIImage>(false);
				for (CGUIImage* img : listImages)
				{
					if (img->getImage() == oldTexture)
						img->setImage(newTexture);
				}
			}
		}
	}
}
