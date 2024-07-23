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
#include "CSceneController.h"
#include "Selection/CSelection.h"
#include "Editor/SpaceController/CPropertyController.h"
#include "GUI/Input/CInput.h"
#include "Entity/CEntityHandleData.h"

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

						m_contextMenuContainer->open(mousePos);
					}
					else
						m_contextMenuGameObject->open(mousePos);

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
	}
}
