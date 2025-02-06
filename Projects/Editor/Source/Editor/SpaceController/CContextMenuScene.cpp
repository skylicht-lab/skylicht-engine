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
#include "CContextMenuScene.h"
#include "CAssetCreateController.h"
#include "CSceneController.h"
#include "Selection/CSelection.h"
#include "Editor/SpaceController/CPropertyController.h"
#include "Editor/SpaceController/CAssetPropertyController.h"
#include "GUI/Input/CInput.h"
#include "Entity/CEntityHandleData.h"

#include "SceneExporter/CExportRenderMesh.h"
#include "SceneExporter/CExportCollider.h"

namespace Skylicht
{
	namespace Editor
	{
		CContextMenuScene::CContextMenuScene(GUI::CCanvas* canvas) :
			m_canvas(canvas),
			m_scene(NULL),
			m_zone(NULL),
			m_spaceHierarchy(NULL),
			m_contextNode(NULL)
		{
			// scene
			m_contextMenuScene = new GUI::CMenu(canvas);
			m_contextMenuScene->setHidden(true);
			m_contextMenuScene->OnCommand = BIND_LISTENER(&CContextMenuScene::OnContextMenuCommand, this);
			m_contextMenuScene->addItem(L"Add Zone", GUI::ESystemIcon::Collection);

			// game object
			m_contextMenuGameObject = new GUI::CMenu(canvas);
			m_contextMenuGameObject->setHidden(true);
			m_contextMenuGameObject->OnCommand = BIND_LISTENER(&CContextMenuScene::OnContextMenuCommand, this);

			addTemplateItem(m_contextMenuGameObject);
			addExportItem(m_contextMenuGameObject);
			m_contextMenuGameObject->addSeparator();
			m_contextMenuGameObject->addItem(L"Rename", L"F2");
			m_contextMenuGameObject->addSeparator();
			m_contextMenuGameObject->addItem(L"Copy", GUI::ESystemIcon::Copy, L"Ctrl + C");
			m_contextMenuGameObject->addItem(L"Paste", GUI::ESystemIcon::Paste, L"Ctrl + V");
			m_contextMenuGameObject->addItem(L"Duplicate", GUI::ESystemIcon::Duplicate, L"Ctrl + D");
			m_contextMenuGameObject->addItem(L"Cut", L"Ctrl + X");
			m_contextMenuGameObject->addSeparator();
			m_contextMenuGameObject->addItem(L"Delete", GUI::ESystemIcon::Trash);

			// container
			m_contextMenuContainer = new GUI::CMenu(canvas);
			m_contextMenuContainer->setHidden(true);
			m_contextMenuContainer->OnCommand = BIND_LISTENER(&CContextMenuScene::OnContextMenuCommand, this);
			m_setCurrentZoneItem = m_contextMenuContainer->addItem(L"Set as Current Zone");
			m_spaceZone = m_contextMenuContainer->addSeparator();
			m_setCurrentZoneItem->setHidden(true);
			m_spaceZone->setHidden(true);

			// entity
			m_contextMenuEntity = new GUI::CMenu(canvas);
			m_contextMenuEntity->addItem(L"Delete", GUI::ESystemIcon::Trash);
			m_contextMenuEntity->OnCommand = BIND_LISTENER(&CContextMenuScene::OnContextMenuCommand, this);

			GUI::CMenuItem* add = m_contextMenuContainer->addItem(L"Add");
			GUI::CMenu* submenu = add->getMenu();
			submenu->addItem(L"Empty Object", GUI::ESystemIcon::Res3D);
			submenu->addItem(L"Container Object", GUI::ESystemIcon::Folder);
			submenu->OnCommand = BIND_LISTENER(&CContextMenuScene::OnContextMenuCommand, this);

			addTemplateItem(m_contextMenuContainer);
			addExportItem(m_contextMenuContainer);
			m_contextMenuContainer->addSeparator();
			m_contextMenuContainer->addItem(L"Rename", L"F2");
			m_contextMenuContainer->addSeparator();
			m_contextMenuContainer->addItem(L"Copy", GUI::ESystemIcon::Copy);
			m_contextMenuContainer->addItem(L"Paste", GUI::ESystemIcon::Paste);
			m_contextMenuContainer->addSeparator();
			m_contextMenuContainer->addItem(L"Delete", GUI::ESystemIcon::Trash);
		}

		CContextMenuScene::~CContextMenuScene()
		{

		}

		void CContextMenuScene::addTemplateItem(GUI::CMenu* menu)
		{
			GUI::CMenuItem* templateMenu = menu->addItem(L"Template");

			GUI::CMenu* submenu = templateMenu->getMenu();
			submenu->addItem(L"Create", GUI::ESystemIcon::ResFile);
			submenu->addItem(L"Select");
			submenu->addItem(L"Revert");
			submenu->addItem(L"Apply");
			submenu->addItem(L"Unpack");
			submenu->OnCommand = BIND_LISTENER(&CContextMenuScene::OnContextMenuTemplateCommand, this);
		}

		void CContextMenuScene::addExportItem(GUI::CMenu* menu)
		{
			GUI::CMenuItem* templateMenu = menu->addItem(L"Export");

			GUI::CMenu* submenu = templateMenu->getMenu();
			submenu->addItem(L"Render Mesh to .OBJ", GUI::ESystemIcon::Export);
			submenu->addItem(L"Collider to .OBJ", GUI::ESystemIcon::Export);
			submenu->OnCommand = BIND_LISTENER(&CContextMenuScene::OnContextMenuExportCommand, this);
		}

		bool CContextMenuScene::onContextMenu(CSpaceHierarchy* spaceHierachy, CHierachyNode* node, CScene* scene, CZone* zone)
		{
			GUI::CInput* input = GUI::CInput::getInput();
			GUI::SPoint mousePos = input->getMousePosition();

			m_contextNode = node;
			m_spaceHierarchy = spaceHierachy;

			if (node->isTagScene())
			{
				m_canvas->closeMenu();
				m_contextMenuScene->open(mousePos);
				return true;
			}
			else if (node->isTagGameObject())
			{
				CGameObject* gameObject = (CGameObject*)node->getTagData();
				if (gameObject)
				{
					m_canvas->closeMenu();

					CContainerObject* container = dynamic_cast<CContainerObject*>(gameObject);
					if (container != NULL)
					{
						CZone* zone = dynamic_cast<CZone*>(gameObject);
						if (zone != NULL)
						{
							m_setCurrentZoneItem->setHidden(false);
							m_spaceZone->setHidden(false);

							if (CSceneController::getInstance()->getZone() == zone)
								m_setCurrentZoneItem->setIcon(GUI::ESystemIcon::Check);
							else
								m_setCurrentZoneItem->setIcon(GUI::ESystemIcon::None);
						}
						else
						{
							m_setCurrentZoneItem->setHidden(true);
							m_spaceZone->setHidden(true);
						}

						showHideTemplateItem(m_contextMenuContainer, gameObject);

						m_contextMenuContainer->open(mousePos);
					}
					else
					{
						showHideTemplateItem(m_contextMenuGameObject, gameObject);

						m_contextMenuGameObject->open(mousePos);
					}

					return true;
				}
			}
			else if (node->isTagEntity())
			{
				m_canvas->closeMenu();
				m_contextMenuEntity->open(mousePos);
				return true;
			}
			return false;
		}

		void CContextMenuScene::showHideTemplateItem(GUI::CMenu* menu, CGameObject* go)
		{
			GUI::CMenuItem* item = NULL;

			if (go->getTypeName() == "CZone")
			{
				item = menu->getItemByPath(L"Template");
				item->setHidden(true);
				return;
			}
			else
			{
				item = menu->getItemByPath(L"Template");
				item->setHidden(false);
			}

			bool isTemplate = go->isTemplateAsset();

			item = menu->getItemByPath(L"Template/Create");
			if (item)
				item->setHidden(isTemplate ? true : false);

			item = menu->getItemByPath(L"Template/Select");
			if (item)
				item->setHidden(isTemplate ? false : true);

			item = menu->getItemByPath(L"Template/Revert");
			if (item)
				item->setHidden(isTemplate ? false : true);

			item = menu->getItemByPath(L"Template/Apply");
			if (item)
				item->setHidden(isTemplate ? false : true);

			item = menu->getItemByPath(L"Template/Unpack");
			if (item)
				item->setHidden(isTemplate ? false : true);
		}

		void CContextMenuScene::OnContextMenuCommand(GUI::CBase* sender)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(sender);
			if (menuItem == NULL)
				return;

			const std::wstring& command = menuItem->getLabel();

			CSceneController* sceneController = CSceneController::getInstance();

			CHierachyNode* contextNode = sceneController->getContextNode();
			CGameObject* contextObject = NULL;
			CScene* contextScene = NULL;
			CEntity* contextEntity = NULL;

			if (contextNode->isTagGameObject())
				contextObject = (CGameObject*)contextNode->getTagData();
			else if (contextNode->isTagScene())
				contextScene = (CScene*)contextNode->getTagData();
			else if (contextNode->isTagEntity())
				contextEntity = (CEntity*)contextNode->getTagData();

			if (contextObject == NULL &&
				contextScene == NULL &&
				contextEntity == NULL)
				return;

			if (command == L"Copy")
			{
				CSceneController::getInstance()->onCopy();
			}
			else if (command == L"Paste")
			{
				CSceneController::getInstance()->onPaste();
			}
			else if (command == L"Duplicate")
			{
				CSceneController::getInstance()->onDuplicate();
			}
			else if (command == L"Cut")
			{
				CSceneController::getInstance()->onCut();
			}

			if (contextObject != NULL)
			{
				if (command == L"Empty Object")
				{
					CContainerObject* container = dynamic_cast<CContainerObject*>(contextObject);
					if (container != NULL)
						sceneController->createEmptyObject(container);
				}
				else if (command == L"Container Object")
				{
					CContainerObject* container = dynamic_cast<CContainerObject*>(contextObject);
					if (container != NULL)
						sceneController->createContainerObject(container);
				}
				else if (command == L"Delete")
				{
					CSelection::getInstance()->unSelect(contextObject);
					CPropertyController::getInstance()->setProperty(NULL);

					sceneController->getHistory()->saveDeleteHistory(contextObject);
					sceneController->onDeleteObject(contextObject);

					contextObject->remove();
					contextNode->remove();
					sceneController->clearContextNode();
				}
				else if (command == L"Rename")
				{
					m_spaceHierarchy->rename(contextNode);
				}
				else if (command == L"Set as Current Zone")
				{
					CSceneController::getInstance()->setZone((CZone*)contextObject);
				}
			}
			else if (contextScene)
			{
				if (command == L"Add Zone")
				{
					CSceneController::getInstance()->createZone();
				}
			}
			else if (contextEntity)
			{
				if (command == L"Delete")
				{
					CSelection::getInstance()->unSelect(contextEntity);
					CPropertyController::getInstance()->setProperty(NULL);

					// delete entity
					CEntityHandleData* data = GET_ENTITY_DATA(contextEntity, CEntityHandleData);
					if (data)
					{
						data->Handler->removeEntity(contextEntity);

						// remove GUI
						contextNode->remove();
						sceneController->clearContextNode();
					}
				}
			}

		}

		void CContextMenuScene::OnContextMenuTemplateCommand(GUI::CBase* sender)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(sender);
			if (menuItem == NULL)
				return;

			const std::wstring& command = menuItem->getLabel();

			CSceneController* sceneController = CSceneController::getInstance();

			CHierachyNode* contextNode = sceneController->getContextNode();

			CGameObject* contextObject = NULL;
			if (contextNode->isTagGameObject())
				contextObject = (CGameObject*)contextNode->getTagData();

			if (contextObject == NULL)
				return;

			if (command == L"Create")
				sceneController->onCreateTemplate(contextObject);
			else if (command == L"Select")
			{
				if (contextObject->isTemplateAsset())
					CAssetPropertyController::getInstance()->browseAsset(contextObject->getTemplateAsset());
			}
			else if (command == L"Apply")
				sceneController->onApplyTemplate(contextObject);
			else if (command == L"Revert")
				sceneController->onRevertTemplate(contextObject);
			else if (command == L"Unpack")
				sceneController->onUnpackTemplate(contextObject);
		}

		void CContextMenuScene::OnContextMenuExportCommand(GUI::CBase* sender)
		{
			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(sender);
			if (menuItem == NULL)
				return;

			if (!m_contextNode->isTagGameObject())
				return;

			CGameObject* gameObject = (CGameObject*)m_contextNode->getTagData();
			if (!gameObject)
				return;

			const std::wstring& command = menuItem->getLabel();

			if (command == L"Render Mesh to .OBJ")
			{
				CExportRenderMesh exporter;
				exporter.addGameObject(gameObject);

				CEntity** entities = exporter.getPrefab()->getEntities();
				u32 numEntity = exporter.getPrefab()->getNumEntities();

				CAssetManager* assetMgr = CAssetManager::getInstance();
				CAssetCreateController* assetCreate = CAssetCreateController::getInstance();

				std::string pattern = std::string("/") + std::string(gameObject->getNameA());
				pattern += "_%02d.obj";

				std::string output = assetMgr->generateAssetPath(pattern.c_str(), assetCreate->getCurrentFolder().c_str());

				if (CMeshManager::getInstance()->exportModel(entities, numEntity, output.c_str()))
					assetCreate->importAndSelect(output.c_str());
			}
			else if (command == L"Collider to .OBJ")
			{
				CExportCollider exporter;

				core::aabbox3df bbox;
				exporter.getBBox(gameObject, bbox);
				exporter.addGameObject(gameObject, bbox);

				CEntity** entities = exporter.getPrefab()->getEntities();
				u32 numEntity = exporter.getPrefab()->getNumEntities();

				CAssetManager* assetMgr = CAssetManager::getInstance();
				CAssetCreateController* assetCreate = CAssetCreateController::getInstance();

				std::string pattern = std::string("/") + std::string(gameObject->getNameA());
				pattern += "_%02d.obj";

				std::string output = assetMgr->generateAssetPath(pattern.c_str(), assetCreate->getCurrentFolder().c_str());

				if (CMeshManager::getInstance()->exportModel(entities, numEntity, output.c_str()))
					assetCreate->importAndSelect(output.c_str());
			}
		}
	}
}
