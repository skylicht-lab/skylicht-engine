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

#include "Selection/CSelection.h"

#include "Scene/CSceneExporter.h"
#include "Scene/CSceneImporter.h"

#include "AssetManager/CAssetImporter.h"
#include "AssetManager/CAssetManager.h"

#include "Handles/CHandles.h"
#include "Selection/CSelecting.h"
#include "CopyPaste/CCopyPaste.h"

#include "Entity/CEntityHandler.h"
#include "Entity/CEntityHandleData.h"

#include "ResourceSettings/MeshExportSettings.h"

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
			m_gizmos(NULL)
		{
			m_transformGizmos = new CTransformGizmos();
			m_worldTransformDataGizmos = new CWorldTransformDataGizmos();

			CAssetManager::getInstance()->registerFileLoader("scene", this);
		}

		CSceneController::~CSceneController()
		{
			delete m_contextMenuScene;

			delete m_transformGizmos;
			delete m_worldTransformDataGizmos;

			delete m_history;

			removeAllHierarchyNodes();

			CAssetManager::getInstance()->unRegisterFileLoader("scene", this);
		}

		void CSceneController::removeAllHierarchyNodes()
		{
			if (m_spaceHierarchy != NULL)
			{
				m_spaceHierarchy->deleteHierarchyNode();
				m_hierachyNode = NULL;
			}
		}

		void CSceneController::initContextMenu(GUI::CCanvas* canvas)
		{
			m_canvas = canvas;
			m_contextMenuScene = new CContextMenuScene(canvas);
		}

		bool CSceneController::needSave()
		{
			return true;
		}

		void CSceneController::save(const char* path)
		{
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
			bool modify = true;
			if (modify)
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
			m_scenePath = path;

			// clear current scene gui
			CSelection::getInstance()->clear();
			CSelecting::getInstance()->end();

			CPropertyController::getInstance()->setProperty(NULL);

			CHandles::getInstance()->end();
			CHandles::getInstance()->setNullRenderer();

			m_gizmos = NULL;

			delete m_history;
			m_history = NULL;

			if (m_spaceHierarchy != NULL)
			{
				m_spaceHierarchy->deleteHierarchyNode();
				m_hierachyNode = NULL;
			}

			// create new scene
			m_scene = m_spaceScene->initNullScene();

			// set scene name
			std::string sceneName = CPath::getFileName(path);
			m_scene->setName(sceneName.c_str());

			// init load scene space
			m_spaceScene->enableRender(false);
			m_spaceScene->getEditor()->initLoadSceneGUI(path.c_str());

			// init history
			m_history = new CSceneHistory(m_scene);
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

		void CSceneController::deleteScene()
		{
			if (m_scene)
				delete m_scene;

			m_scene = NULL;
		}

		void CSceneController::setScene(CScene* scene)
		{
			if (m_spaceHierarchy != NULL)
			{
				m_spaceHierarchy->deleteHierarchyNode();
				m_hierachyNode = NULL;
			}

			m_scene = scene;

			if (m_scene == NULL)
				return;

			CHandles::getInstance()->refresh();

			m_scene->update();

			setZone(m_scene->getZone(0));

			reinitHierachyData();

			// reinit history
			if (m_history)
			{
				delete m_history;
				m_history = NULL;
			}
			m_history = new CSceneHistory(m_scene);
		}

		void CSceneController::reinitHierachyData()
		{
			m_hierachyNode = new CHierachyNode(NULL);
			m_hierachyNode->setName(m_scene->getName());
			m_hierachyNode->setIcon(GUI::ESystemIcon::Collection);
			m_hierachyNode->setTagData(m_scene, CHierachyNode::Scene);

			setNodeEvent(m_hierachyNode);

			buildHierarchyData();

			if (m_spaceHierarchy != NULL)
				m_spaceHierarchy->setTreeNode(m_hierachyNode);

			m_focusNode = NULL;
			m_contextNode = NULL;
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
				m_spaceHierarchy->setTreeNode(m_hierachyNode);
		}

		void CSceneController::buildHierarchyData()
		{
			ArrayZone* zones = m_scene->getAllZone();
			for (CZone* zone : *zones)
			{
				CHierachyNode* node = buildHierarchyData(zone, m_hierachyNode);
			}
		}

		CHierachyNode* CSceneController::buildHierarchyData(CGameObject* object, CHierachyNode* parentNode)
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
					buildHierarchyData(childObject, node);
				}
			}
			else
			{
				// add child
				node = parentNode->addChild();
				node->setName(object->getName());
				node->setIcon(GUI::ESystemIcon::Res3D);
				node->setTagData(object, CHierachyNode::GameObject);

				rebuildHierarchyEntityData(object, node);
			}

			if (node != NULL)
				setNodeEvent(node);

			return node;
		}

		void CSceneController::updateTreeNode(CGameObject* object)
		{
			if (m_spaceHierarchy != NULL)
			{
				m_spaceHierarchy->getController()->updateTreeNode(object);
			}
		}

		void CSceneController::rebuildHierarchyEntityData(CGameObject* object, CHierachyNode* parentNode)
		{
			parentNode->removeAll(CHierachyNode::Entity);

			CEntityHandler* entityHandler = object->getComponent<CEntityHandler>();
			if (entityHandler == NULL)
				return;

			std::vector<CEntity*>& entities = entityHandler->getEntities();
			for (CEntity* entity : entities)
			{
				CHierachyNode* node = parentNode->addChild();
				CWorldTransformData* worldData = entity->getData<CWorldTransformData>();

				node->setName(CStringImp::convertUTF8ToUnicode(worldData->Name.c_str()).c_str());
				node->setIcon(GUI::ESystemIcon::Poly);
				node->setTagData(entity, CHierachyNode::Entity);

				setNodeEvent(node);
			}
		}

		void CSceneController::update()
		{
			if (m_gizmos)
			{
				m_gizmos->onGizmos();
			}
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
			if (m_gizmos)
				m_gizmos->refresh();

			// Set property & event
			CPropertyController* propertyController = CPropertyController::getInstance();

			// Update property
			propertyController->setProperty(selectedObject);

			// Register observer because we removed
			selectedObject->addObserver(this);
		}

		void CSceneController::setGizmos(CGizmos* gizmos)
		{
			if (m_gizmos != gizmos)
			{
				if (m_gizmos)
					m_gizmos->onRemove();

				if (gizmos)
					gizmos->onEnable();

				m_gizmos = gizmos;
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
			node->setIcon(GUI::ESystemIcon::Folder);
			node->setTagData(zone, CHierachyNode::Zone);

			setNodeEvent(node);

			if (m_spaceHierarchy != NULL)
				m_spaceHierarchy->addToTreeNode(node);
		}

		CGameObject* CSceneController::createEmptyObject(CContainerObject* parent)
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
					m_spaceHierarchy->addToTreeNode(node);
			}

			return newObject;
		}

		CGameObject* CSceneController::createContainerObject(CContainerObject* parent)
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
					m_spaceHierarchy->addToTreeNode(node);
			}

			return newObject;
		}

		void CSceneController::createResourceComponent(const std::string& path, CGameObject* gameObject)
		{
			std::string shortPath = CAssetManager::getInstance()->getShortPath(path.c_str());

			std::string fileExt = CPath::getFileNameExt(path);
			fileExt = CStringImp::toLower(fileExt);

			if (fileExt == "dae" || fileExt == "smesh")
			{
				std::string defaultMaterial;
				std::string meta = path + ".meta";

				if (fileExt == "dae")
				{
					MeshExportSettings* setting = new MeshExportSettings();
					if (setting->load(meta.c_str()))
						defaultMaterial = setting->DefaultMaterial.get();
					delete setting;
				}

				CRenderMesh* renderMesh = gameObject->addComponent<CRenderMesh>();
				renderMesh->initFromMeshFile(shortPath.c_str());

				if (!defaultMaterial.empty())
					renderMesh->initMaterialFromFile(defaultMaterial.c_str());

				CIndirectLighting* indirectLighting = gameObject->addComponent<CIndirectLighting>();
				indirectLighting->setAutoSH(true);
				indirectLighting->setIndirectLightingType(CIndirectLighting::SH9);
			}
		}

		void CSceneController::deselectAllOnHierachy()
		{
			m_spaceHierarchy->deselectAll();
		}

		CHierachyNode* CSceneController::deselectOnHierachy(CGameObject* gameObject)
		{
			CHierachyNode* node = m_hierachyNode->getNodeByTag(gameObject);
			if (node != NULL)
			{
				GUI::CTreeNode* treeNode = node->getGUINode();
				treeNode->setSelected(false);
			}
			return node;
		}

		CHierachyNode* CSceneController::deselectOnHierachy(CEntity* entity)
		{
			CHierachyNode* node = m_hierachyNode->getNodeByTag(entity);
			if (node != NULL)
			{
				GUI::CTreeNode* treeNode = node->getGUINode();
				treeNode->setSelected(false);
			}
			return node;
		}

		CHierachyNode* CSceneController::selectOnHierachy(CGameObject* gameObject)
		{
			CHierachyNode* node = m_hierachyNode->getNodeByTag(gameObject);
			if (node != NULL)
			{
				GUI::CTreeNode* treeNode = node->getGUINode();
				treeNode->setSelected(true);
				m_spaceHierarchy->scrollToNode(treeNode);
			}
			return node;
		}

		CHierachyNode* CSceneController::selectOnHierachy(CEntity* entity)
		{
			CHierachyNode* node = m_hierachyNode->getNodeByTag(entity);
			if (node != NULL)
			{
				GUI::CTreeNode* treeNode = node->getGUINode();
				treeNode->setSelected(true);
				m_spaceHierarchy->scrollToNode(treeNode);
			}
			return node;
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

				// update list entity
				rebuildHierarchyEntityData(obj, node);

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
			CSelection* selection = CSelection::getInstance();

			if (node->isTagGameObject())
			{
				CGameObject* obj = (CGameObject*)node->getTagData();

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

					// add gizmos
					setGizmos(m_transformGizmos);

					// add new observer
					selectedObject->addObserver(this);
				}
				else
				{
					selection->unSelect(obj);
					propertyController->setProperty(NULL);
				}
			}
			else if (node->isTagEntity())
			{
				CEntity* entity = (CEntity*)node->getTagData();

				// remove last observer
				CSelectObject* selectedObject = selection->getLastSelected();
				if (selectedObject != NULL)
					selectedObject->removeAllObserver();

				// Set property & event
				CPropertyController* propertyController = CPropertyController::getInstance();
				if (selected)
				{
					selectedObject = selection->addSelect(entity);
					propertyController->setProperty(selectedObject);

					// add gizmos
					setGizmos(m_worldTransformDataGizmos);

					// add new observer
					selectedObject->addObserver(this);
				}
				else
				{
					selection->unSelect(entity);
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

		void CSceneController::onDelete()
		{
			CSelection* selection = CSelection::getInstance();
			std::vector<CSelectObject*>& selected = selection->getAllSelected();

			// loop and delete all selected
			for (CSelectObject* selectObject : selected)
			{
				CSelectObject::ESelectType type = selectObject->getType();

				if (type == CSelectObject::GameObject)
				{
					CGameObject* gameObject = m_scene->searchObjectInChildByID(selectObject->getID().c_str());
					if (gameObject != NULL)
					{
						// remove gameobject
						gameObject->remove();

						// remove gui hierachy
						if (m_hierachyNode != NULL)
						{
							CHierachyNode* node = m_hierachyNode->getNodeByTag(gameObject);
							if (node != NULL)
								node->remove();
						}
					}
				}
				else if (type == CSelectObject::Entity)
				{
					CEntity* entity = m_scene->getEntityManager()->getEntityByID(selectObject->getID().c_str());
					if (entity != NULL)
					{
						// delete entity
						CEntityHandleData* data = entity->getData<CEntityHandleData>();

						CEntityHandler* handler = data->Handler;
						handler->removeEntity(entity);

						// remove gui hierachy
						if (m_spaceHierarchy != NULL)
							m_spaceHierarchy->getController()->updateTreeNode(handler->getGameObject());
					}
				}
			}

			selection->clear();
			CPropertyController::getInstance()->setProperty(NULL);
		}

		void CSceneController::onCopy()
		{
			CSelection* selection = CSelection::getInstance();
			std::vector<CSelectObject*>& selected = selection->getAllSelected();

			std::vector<CGameObject*> listGameObject;

			for (CSelectObject* selectObject : selected)
			{
				CSelectObject::ESelectType type = selectObject->getType();
				if (type == CSelectObject::GameObject)
				{
					CGameObject* gameObject = m_scene->searchObjectInChildByID(selectObject->getID().c_str());
					if (gameObject != NULL)
					{
						listGameObject.push_back(gameObject);
					}
				}
			}

			CCopyPaste::getInstance()->copy(listGameObject);
		}

		void CSceneController::onPaste()
		{
			CSelection* selection = CSelection::getInstance();
			CCopyPaste* copyPaste = CCopyPaste::getInstance();

			CSelectObject* lastSelected = selection->getLastSelected();
			if (lastSelected == NULL)
				return;

			if (lastSelected->getType() == CSelectObject::GameObject)
			{
				CGameObject* gameObject = m_scene->searchObjectInChildByID(lastSelected->getID().c_str());
				if (gameObject != NULL)
				{
					CContainerObject* container = dynamic_cast<CContainerObject*>(gameObject);
					if (container != NULL)
						CCopyPaste::getInstance()->paste(container);
					else
					{
						container = dynamic_cast<CContainerObject*>(gameObject->getParent());
						CCopyPaste::getInstance()->paste(container);
					}
				}
			}
			else if (lastSelected->getType() == CSelectObject::Entity)
			{
				CEntity* entity = m_scene->getEntityManager()->getEntityByID(lastSelected->getID().c_str());
				if (entity != NULL)
				{
					CEntityHandleData* data = entity->getData<CEntityHandleData>();

					CGameObject* gameObject = data->Handler->getGameObject();
					if (gameObject != NULL)
					{
						CContainerObject* container = dynamic_cast<CContainerObject*>(gameObject);
						if (container != NULL)
							CCopyPaste::getInstance()->paste(container);
						else
						{
							container = dynamic_cast<CContainerObject*>(gameObject->getParent());
							CCopyPaste::getInstance()->paste(container);
						}

						m_spaceHierarchy->getController()->updateTreeNode(container);
					}
				}
			}
		}

		void CSceneController::onDuplicate()
		{
			onCopy();
			onPaste();
		}

		void CSceneController::onUndo()
		{
			m_history->undo();
		}

		void CSceneController::onRedo()
		{
			m_history->redo();
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