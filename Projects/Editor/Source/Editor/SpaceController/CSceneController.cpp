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

#include "CSceneController.h"
#include "CPropertyController.h"
#include "CCollisionController.h"

#include "Selection/CSelection.h"

#include "Scene/CSceneExporter.h"
#include "Scene/CSceneImporter.h"

#include "AssetManager/CAssetImporter.h"
#include "AssetManager/CAssetManager.h"

#include "Handles/CHandles.h"

namespace Skylicht
{
	namespace Editor
	{
		CSceneController::CSceneController() :
			m_spaceHierarchy(NULL),
			m_spaceScene(NULL),
			m_scene(NULL),
			m_zone(NULL),
			m_hierachyNode(NULL),
			m_canvas(NULL),
			m_history(NULL),
			m_focusNode(NULL),
			m_contextNode(NULL),
			m_contextMenuScene(NULL),
			m_modify(false)
		{
			CAssetManager::getInstance()->registerFileLoader("scene", this);
		}

		CSceneController::~CSceneController()
		{
			delete m_contextMenuScene;

			if (m_hierachyNode != NULL)
				delete m_hierachyNode;

			CAssetManager::getInstance()->unRegisterFileLoader("scene", this);
		}

		void CSceneController::initContextMenu(GUI::CCanvas* canvas)
		{
			m_canvas = canvas;
			m_contextMenuScene = new CContextMenuScene(canvas);
		}

		bool CSceneController::needSave()
		{
			if (m_scenePath.empty())
				return true;

			return m_modify;
		}

		void CSceneController::save(const char* path)
		{
			m_modify = false;
			m_scenePath = path;

			CSceneExporter::exportScene(m_scene, path);

			std::string sceneName = CPath::getFileNameNoExt(path);
			m_hierachyNode->setName(CStringImp::convertUTF8ToUnicode(sceneName.c_str()).c_str());
			m_hierachyNode->refreshGUI();

			CAssetImporter importer;
			importer.add(path);
			importer.importAll();

			m_spaceScene->getEditor()->refresh();
		}

		void CSceneController::loadFile(const std::string& path)
		{
			if (m_modify)
			{
				GUI::CMessageBox* msgBox = new GUI::CMessageBox(m_canvas, GUI::CMessageBox::YesNoCancel);
				msgBox->setMessage(L"Do you want to save current scene?", m_scene->getName());
				msgBox->OnYes = [&, p = path](GUI::CBase* button) {
					if (m_scenePath.empty() == true)
					{
						std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();
						GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(m_canvas, GUI::COpenSaveDialog::Save, assetFolder.c_str(), assetFolder.c_str(), "scene;*");
						dialog->OnSave = [&](std::string path)
						{
							save(path.c_str());
						};
					}
					else
					{
						save(m_scenePath.c_str());
						doLoadScene(p);
					}
				};
				msgBox->OnNo = [&, p = path](GUI::CBase* button) {
					doLoadScene(p);
				};
			}
			else
			{
				doLoadScene(path);
			}
		}

		void CSceneController::doLoadScene(const std::string& path)
		{
			m_modify = false;
			m_scenePath = path;

			// clear current scene gui
			CSelection::getInstance()->clear();
			CPropertyController::getInstance()->setProperty(NULL);
			CCollisionController::getInstance()->clear();

			CHandles::getInstance()->end();
			CHandles::getInstance()->setNullRenderer();

			if (m_hierachyNode != NULL)
			{
				delete m_hierachyNode;
				m_hierachyNode = NULL;
			}

			m_gizmos.clear();

			// create new scene
			m_scene = m_spaceScene->initNullScene();

			// set scene name
			std::string sceneName = CPath::getFileName(path);
			m_scene->setName(sceneName.c_str());

			// init load scene space
			m_spaceScene->enableRender(false);
			m_spaceScene->getEditor()->initLoadSceneGUI(path.c_str());
		}

		void CSceneController::doFinishLoadScene()
		{
			CZone* zone = m_scene->getZone(0);

			if (zone != NULL && !zone->isEditorObject())
			{
				m_spaceScene->initEditorObject(zone);
			}
			else
			{
				m_spaceScene->initEditorObject(m_scene->createZone());
			}

			setScene(m_scene);

			m_spaceScene->enableRender(true);
		}

		void CSceneController::setScene(CScene* scene)
		{
			if (m_hierachyNode != NULL)
			{
				delete m_hierachyNode;
				m_hierachyNode = NULL;
			}

			m_scene = scene;

			if (m_scene == NULL)
				return;

			m_scene->update();

			setZone(m_scene->getZone(0));

			m_hierachyNode = new CHierachyNode(NULL);
			m_hierachyNode->setName(m_scene->getName());
			m_hierachyNode->setIcon(GUI::ESystemIcon::Collection);
			m_hierachyNode->setTagData(m_scene, CHierachyNode::Scene);

			setNodeEvent(m_hierachyNode);

			buildHierarchyNodes();

			if (m_spaceHierarchy != NULL)
				m_spaceHierarchy->setHierarchyNode(m_hierachyNode);

			m_focusNode = NULL;
			m_contextNode = NULL;

			CHandles::getInstance()->refresh();
		}

		void CSceneController::setZone(CZone* zone)
		{
			ArrayZone* zones = m_scene->getAllZone();
			for (CZone* z : *zones)
			{
				if (z == zone)
				{
					m_zone = z;
					return;
				}
			}
		}

		void CSceneController::setSpaceScene(CSpaceScene* scene)
		{
			m_spaceScene = scene;
		}

		void CSceneController::setSpaceHierarchy(CSpaceHierarchy* hierarchy)
		{
			m_spaceHierarchy = hierarchy;
			if (m_spaceHierarchy != NULL && m_hierachyNode != NULL)
				m_spaceHierarchy->setHierarchyNode(m_hierachyNode);
		}

		void CSceneController::buildHierarchyNodes()
		{
			ArrayZone* zones = m_scene->getAllZone();
			for (CZone* zone : *zones)
			{
				CHierachyNode* node = buildHierarchyNodes(zone, m_hierachyNode);
			}
		}

		CHierachyNode* CSceneController::buildHierarchyNodes(CGameObject* object, CHierachyNode* parentNode)
		{
			if (object->isEditorObject())
				return NULL;

			CContainerObject* container = dynamic_cast<CContainerObject*>(object);
			CZone* zone = dynamic_cast<CZone*>(object);

			CHierachyNode* node = NULL;

			if (container != NULL)
			{
				// add container child
				node = parentNode->addChild();
				node->setName(object->getName());

				if (zone != NULL)
				{
					node->setTagData(object, CHierachyNode::Zone);
					node->setIcon(GUI::ESystemIcon::Folder);
				}
				else
				{
					node->setTagData(object, CHierachyNode::Container);
					node->setIcon(GUI::ESystemIcon::Folder);
				}

				// enable color on current zone
				// if (m_zone == object)
				//	node->EnableColor(true);

				ArrayGameObject* childs = container->getChilds();
				for (CGameObject* childObject : *childs)
				{
					buildHierarchyNodes(childObject, node);
				}
			}
			else
			{
				// add child
				node = parentNode->addChild();
				node->setName(object->getName());
				node->setIcon(GUI::ESystemIcon::Res3D);
				node->setTagData(object, CHierachyNode::GameObject);
			}

			if (node != NULL)
				setNodeEvent(node);

			return node;
		}

		void CSceneController::update()
		{
			for (CGizmos* g : m_gizmos)
			{
				g->onGizmos();
			}

			CCollisionController::getInstance()->getBBCollision()->drawDebug();
		}

		void CSceneController::refresh()
		{
			CSelection* selection = CSelection::getInstance();

			CSelectObject* selectedObject = selection->getLastSelected();
			if (selectedObject != NULL)
				selectedObject->removeAllObserver();
			else
				return;

			// refresh gizmos
			for (CGizmos* g : m_gizmos)
				g->refresh();

			// Set property & event
			CPropertyController* propertyController = CPropertyController::getInstance();

			// Update property
			propertyController->setProperty(selectedObject);

			// Register observer because we removed
			selectedObject->addObserver(this);
		}

		void CSceneController::addGizmos(CGizmos* gizmos)
		{
			if (std::find(m_gizmos.begin(), m_gizmos.end(), gizmos) != m_gizmos.end())
			{
				gizmos->onEnable();
				return;
			}

			gizmos->onEnable();
			m_gizmos.push_back(gizmos);
		}

		void CSceneController::removeGizmos(CGizmos* gizmos)
		{
			std::vector<CGizmos*>::iterator i = std::find(m_gizmos.begin(), m_gizmos.end(), gizmos);
			if (i != m_gizmos.end())
			{
				gizmos->onRemove();
				m_gizmos.erase(i);
			}
		}

		void CSceneController::onCommand(const std::wstring& objectType)
		{
			if (m_scene == NULL || m_zone == NULL)
				return;

			if (objectType == L"Zone")
				createZone();
			if (objectType == L"Empty Object")
				createEmptyObject(NULL);
			else if (objectType == L"Container Object")
				createContainerObject(NULL);
		}

		void CSceneController::onContextMenu(CHierachyNode* node)
		{
			if (m_canvas == NULL)
				return;

			if (m_contextMenuScene->onContextMenu(m_spaceHierarchy, node, m_scene, m_zone))
				m_contextNode = node;
		}

		void CSceneController::createZone()
		{
			CZone* zone = m_scene->createZone();

			CHierachyNode* node = m_hierachyNode->addChild();
			node->setName(zone->getName());
			node->setIcon(GUI::ESystemIcon::Collection);
			node->setTagData(zone, CHierachyNode::Zone);

			setNodeEvent(node);

			if (m_spaceHierarchy != NULL)
				m_spaceHierarchy->add(node);
		}

		void CSceneController::createEmptyObject(CContainerObject* parent)
		{
			CContainerObject* p = parent == NULL ? m_zone : parent;

			CGameObject* newObject = p->createEmptyObject();

			p->getZone()->updateAddRemoveObject();
			p->getZone()->updateIndexSearchObject();

			CHierachyNode* parentNode = m_hierachyNode->getNodeByTag(p);
			if (parentNode != NULL)
			{
				CHierachyNode* node = parentNode->addChild();
				node->setName(newObject->getName());
				node->setIcon(GUI::ESystemIcon::Res3D);
				node->setTagData(newObject, CHierachyNode::GameObject);

				setNodeEvent(node);

				if (m_spaceHierarchy != NULL)
					m_spaceHierarchy->add(node);
			}
		}

		void CSceneController::createContainerObject(CContainerObject* parent)
		{
			CContainerObject* p = parent == NULL ? m_zone : parent;

			CGameObject* newObject = p->createContainerObject();
			p->getZone()->updateAddRemoveObject();
			p->getZone()->updateIndexSearchObject();

			CHierachyNode* parentNode = m_hierachyNode->getNodeByTag(p);
			if (parentNode != NULL)
			{
				CHierachyNode* node = parentNode->addChild();
				node->setName(newObject->getName());
				node->setIcon(GUI::ESystemIcon::Folder);
				node->setTagData(newObject, CHierachyNode::Container);

				setNodeEvent(node);

				if (m_spaceHierarchy != NULL)
					m_spaceHierarchy->add(node);
			}
		}

		void CSceneController::setNodeEvent(CHierachyNode* node)
		{
			node->OnUpdate = std::bind(&CSceneController::onUpdateNode, this, std::placeholders::_1);
			node->OnSelected = std::bind(&CSceneController::onSelectNode, this, std::placeholders::_1, std::placeholders::_2);
		}

		void CSceneController::onUpdateNode(CHierachyNode* node)
		{
			if (node->isTagGameObject())
			{
				// change object name
				CGameObject* obj = (CGameObject*)node->getTagData();
				obj->setName(node->getName().c_str());

				// notify object changed
				CSelectObject* selectedObject = CSelection::getInstance()->getSelected(obj);
				if (selectedObject != NULL)
					selectedObject->notify(this);
			}
			else if (node->isTagScene())
			{
				// change scene name
				CScene* scene = (CScene*)node->getTagData();
				scene->setName(node->getName().c_str());
			}
		}

		void CSceneController::onSelectNode(CHierachyNode* node, bool selected)
		{
			m_focusNode = node;

			if (node->isTagGameObject())
			{
				CGameObject* obj = (CGameObject*)node->getTagData();
				CSelection* selection = CSelection::getInstance();

				// remove last observer
				CSelectObject* selectedObject = selection->getLastSelected();
				if (selectedObject != NULL)
					selectedObject->removeAllObserver();

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
					selection->unSelect(obj);
					propertyController->setProperty(NULL);
				}
			}
		}

		void CSceneController::onObjectChange(CGameObject* object)
		{
			CHierachyNode* node = NULL;
			if (m_focusNode->getTagData() == object)
				node = m_focusNode;

			if (node == NULL)
				m_hierachyNode->getNodeByTag(object);

			if (node != NULL)
				node->getGUINode()->setText(object->getName());
		}

		void CSceneController::onNotify(ISubject* subject, IObserver* from)
		{
			CSelectObject* selected = dynamic_cast<CSelectObject*>(subject);
			if (selected != NULL && from != this)
			{
				if (selected->getType() == CSelectObject::GameObject)
				{
					CGameObject* obj = m_scene->searchObjectInChildByID(selected->getID().c_str());
					if (obj != NULL)
						onObjectChange(obj);
				}
			}
		}
	}
}