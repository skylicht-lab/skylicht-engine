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
#include "CAssetCreateController.h"
#include "CParticleController.h"

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

#include "SpriteDraw/CSprite.h"

#include "ResourceSettings/CMeshExportSettings.h"

#if BUILD_SKYLICHT_PHYSIC
#include "PhysicsEngine/CPhysicsEngine.h"
#endif

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CSceneController);

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
			m_particleGizmos = new CParticleGizmos();

			CAssetManager::getInstance()->registerFileLoader("scene", this);
		}

		CSceneController::~CSceneController()
		{
			delete m_contextMenuScene;

			delete m_transformGizmos;
			delete m_worldTransformDataGizmos;
			delete m_particleGizmos;

			if (m_history)
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

		void CSceneController::newScene()
		{
			bool modify = true;
			if (modify)
			{
				GUI::CMessageBox* msgBox = new GUI::CMessageBox(m_canvas, GUI::CMessageBox::YesNoCancel);
				msgBox->setMessage(L"Do you want to save current scene?", m_scene->getName());
				msgBox->OnYes = [&](GUI::CBase* button) {
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
						doNewScene();
					}
					};
				msgBox->OnNo = [&](GUI::CBase* button) {
					doNewScene();
					};
			}
			else
			{
				doNewScene();
			}
		}

		void CSceneController::doNewScene()
		{
			m_scenePath.clear();

			// clear current scene gui
			CSelection::getInstance()->clear();
			CSelecting::getInstance()->end();

			CPropertyController::getInstance()->setProperty(NULL);

			CHandles::getInstance()->end();
			CHandles::getInstance()->setNullRenderer();

			m_gizmos = NULL;

			if (m_history)
			{
				delete m_history;
				m_history = NULL;
			}

			if (m_spaceHierarchy != NULL)
			{
				m_spaceHierarchy->deleteHierarchyNode();
				m_hierachyNode = NULL;
			}

			deleteScene();

			if (m_spaceScene == NULL)
			{
				return;
			}

			m_spaceScene->initDefaultScene();
		}

		void CSceneController::doLoadScene(const std::string& path)
		{
			// clear current scene gui
			CSelection::getInstance()->clear();
			CSelecting::getInstance()->end();

			CPropertyController::getInstance()->setProperty(NULL);

			CHandles::getInstance()->end();
			CHandles::getInstance()->setNullRenderer();

			m_gizmos = NULL;

			if (m_history)
			{
				delete m_history;
				m_history = NULL;
			}

			if (m_spaceHierarchy != NULL)
			{
				m_spaceHierarchy->deleteHierarchyNode();
				m_hierachyNode = NULL;
			}

			if (m_spaceScene == NULL)
			{
				deleteScene();
				return;
			}

			m_scenePath = path;

			// create new scene
			m_scene = m_spaceScene->initNullScene();

			m_spaceScene->enableRender(false);
			m_spaceScene->getEditor()->initLoadSceneGUI(path.c_str());

			// init history
			m_history = new CSceneHistory(m_scene);
		}

		void CSceneController::doFinishLoadScene()
		{
			if (!m_scenePath.empty())
			{
				std::string sceneName = CPath::getFileName(m_scenePath);
				m_scene->setName(sceneName.c_str());
			}

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

		void CSceneController::doMeshChange(const char* resource)
		{
			CZone* zone = m_scene->getZone(0);

			CHierarchyController* hierachyController = NULL;
			if (m_spaceHierarchy)
				hierachyController = m_spaceHierarchy->getController();

			std::vector<CRenderMesh*> listMesh = zone->getComponentsInChild<CRenderMesh>(false);
			for (CRenderMesh* render : listMesh)
			{
				if (render->getMeshResource() == resource)
				{
					render->initFromMeshFile(resource);

					if (hierachyController)
						hierachyController->updateTreeNode(render->getGameObject());
				}
			}

			core::array<core::matrix4> instancingTransform;

			std::vector<CRenderMeshInstancing*> listMeshInstancing = zone->getComponentsInChild<CRenderMeshInstancing>(false);
			for (CRenderMeshInstancing* render : listMeshInstancing)
			{
				if (render->getMeshResource() == resource)
				{
					render->getEntitiesTransforms(instancingTransform);
					render->initFromMeshFile(resource);
					for (u32 i = 0, n = instancingTransform.size(); i < n; i++)
						render->spawn(instancingTransform[i]);

					if (hierachyController)
						hierachyController->updateTreeNode(render->getGameObject());
				}
			}
		}

		void CSceneController::doMaterialChange(const char* resource)
		{
			CZone* zone = m_scene->getZone(0);

			std::vector<CRenderMesh*> listRenderMesh = zone->getComponentsInChild<CRenderMesh>(false);
			for (CRenderMesh* render : listRenderMesh)
			{
				if (render->getMaterialResource() == resource)
					render->initMaterialFromFile(resource);
			}

			std::vector<CRenderMeshInstancing*> listMeshInstancing = zone->getComponentsInChild<CRenderMeshInstancing>(false);
			for (CRenderMeshInstancing* render : listMeshInstancing)
			{
				if (render->getMaterialResource() == resource)
					render->initMaterialFromFile(resource);
			}
		}

		void CSceneController::deleteScene()
		{
			if (m_scene)
				delete m_scene;

			removeAllHierarchyNodes();

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
			if (m_hierachyNode)
				removeAllHierarchyNodes();

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
			CHierachyNode* node = NULL;

			if (container != NULL)
			{
				// add container child
				node = parentNode->addChild();

				if (container->getTypeName() == "CZone")
				{
					node->setTagData(object, CHierachyNode::Zone);
					node->setIcon(GUI::ESystemIcon::Folder);
				}
				else
				{
					node->setTagData(object, CHierachyNode::Container);
					node->setIcon(GUI::ESystemIcon::Folder);
				}

				CHierarchyController::updateObjectToUI(object, node);

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

				node->setIcon(GUI::ESystemIcon::Res3D);
				node->setTagData(object, CHierachyNode::GameObject);

				CHierarchyController::updateObjectToUI(object, node);

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

		void CSceneController::rebuildHierarchyData(CGameObject* object)
		{
			CHierachyNode* node = m_hierachyNode->getNodeByTag(object);
			if (node == NULL)
				return;

			node->removeAllChild();

			CContainerObject* container = dynamic_cast<CContainerObject*>(object);
			if (container)
			{
				if (container->getTypeName() == "CZone")
				{
					node->setTagData(object, CHierachyNode::Zone);
					node->setIcon(GUI::ESystemIcon::Folder);
				}
				else
				{
					node->setTagData(object, CHierachyNode::Container);
					node->setIcon(GUI::ESystemIcon::Folder);
				}

				CHierarchyController::updateObjectToUI(object, node);

				ArrayGameObject* childs = container->getChilds();
				for (CGameObject* childObject : *childs)
				{
					buildHierarchyData(childObject, node);
				}
			}
			else
			{
				node->setIcon(GUI::ESystemIcon::Res3D);
				node->setTagData(object, CHierachyNode::GameObject);

				node->setName(object->getName());

				rebuildHierarchyEntityData(object, node);
			}
		}

		void CSceneController::rebuildHierarchyEntityData(CGameObject* object, CHierachyNode* rootNode)
		{
			rootNode->removeAll(CHierachyNode::Entity);

			CEntityHandler* entityHandler = object->getComponent<CEntityHandler>();
			if (entityHandler == NULL)
				return;

			CEntity* root = object->getEntity();

			std::map<int, CHierachyNode*> treeNodes;
			treeNodes[root->getIndex()] = rootNode;

			core::array<CEntity*>& entities = entityHandler->getEntities();
			for (u32 i = 0, n = entities.size(); i < n; i++)
			{
				CEntity* entity = entities[i];

				CWorldTransformData* worldData = GET_ENTITY_DATA(entity, CWorldTransformData);
				CRenderMeshData* renderData = GET_ENTITY_DATA(entity, CRenderMeshData);

				CHierachyNode* parentNode = treeNodes[worldData->ParentIndex];

				CHierachyNode* node = parentNode->addChild();
				node->setName(CStringImp::convertUTF8ToUnicode(worldData->Name.c_str()).c_str());

				if (renderData != NULL)
					node->setIcon(GUI::ESystemIcon::Poly);
				else
					node->setIcon(GUI::ESystemIcon::Axis);

				node->setTagData(entity, CHierachyNode::Entity);

				treeNodes[entity->getIndex()] = node;

				setNodeEvent(node);
			}
		}

		void CSceneController::update()
		{
			CCamera* cam = NULL;
			CEditorCamera* editorCam = NULL;
			
			if (m_spaceScene && m_scene)
			{
				cam = m_spaceScene->getEditorCamera();
				if (cam)
					editorCam = cam->getGameObject()->getComponent<CEditorCamera>();
			}
			
			if (m_gizmos)
			{
				m_gizmos->onGizmos();
				
				if (editorCam)
				{
					core::vector3df gizmosPosition;
					if (m_gizmos->getPosition(gizmosPosition))
						editorCam->setPivotRotate(gizmosPosition);
					else
						editorCam->setPivotRotateDistance(5.0f);
				}
			}
			else
			{
				if (editorCam)
					editorCam->setPivotRotateDistance(5.0f);
			}

			updateSelectedRigidBody();
		}

		void CSceneController::updateSelectedRigidBody()
		{
#ifdef BUILD_SKYLICHT_PHYSIC
			Physics::CPhysicsEngine* physicsEngine = Physics::CPhysicsEngine::getInstance();

			std::vector<CSelectObject*>& objectsSelected = CSelection::getInstance()->getAllSelected();

			core::array<Physics::SRigidbodyData*>& bodies = physicsEngine->getBodies();
			for (u32 i = 0, n = bodies.size(); i < n; i++)
			{
				Physics::SRigidbodyData* body = bodies[i];

				std::string& objectId = body->Body->getGameObject()->getID();

				bool selected = false;

				for (CSelectObject* obj : objectsSelected)
				{
					if (obj->getID() == objectId)
					{
						selected = true;
						break;
					}
				}

				body->Body->setDrawDebug(selected);
			}
#endif
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

			CHandles::getInstance()->reset();

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

			CContainerObject* parent = NULL;
			CSelectObject* selectedObject = CSelection::getInstance()->getLastSelected();
			if (selectedObject)
			{
				if (selectedObject->getType() == CSelectObject::GameObject)
				{
					CGameObject* obj = m_scene->searchObjectInChildByID(selectedObject->getID().c_str());
					if (obj != NULL)
					{
						parent = dynamic_cast<CContainerObject*>(obj);
						if (parent == NULL)
							parent = dynamic_cast<CContainerObject*>(obj->getParent());
					}
				}
			}

			if (objectType == L"Zone")
			{
				createZone();
			}
			if (objectType == L"Empty Object")
			{
				createEmptyObject(parent);
			}
			else if (objectType == L"Container Object")
			{
				createContainerObject(parent);
			}
			else if (objectType == L"Skydome")
			{
				std::vector<std::string> components = { "CSkyDome" };
				createComponentObject("Skydome", components, parent);
			}
			else if (objectType == L"Skybox")
			{
				std::vector<std::string> components = { "CSkyBox" };
				createComponentObject("Skybox", components, parent);
			}
			else if (objectType == L"Sky")
			{
				std::vector<std::string> components = { "CSkySun" };
				createComponentObject("Sky", components, parent);
			}
			else if (objectType == L"Mesh")
			{
				std::vector<std::string> components = { "CRenderMesh", "CIndirectLighting" };
				createComponentObject("Mesh", components, parent);
			}
			else if (objectType == L"Mesh Instancing")
			{
				std::vector<std::string> components = { "CRenderMeshInstancing", "CIndirectLighting" };
				createComponentObject("Mesh Instancing", components, parent);
			}
			else if (objectType == L"Direction Light")
			{
				std::vector<std::string> components = { "CDirectionalLight" };
				CGameObject* go = createComponentObject("Direction Light", components, parent);
				go->getTransformEuler()->setRotation(core::vector3df(90.0f, 0.0f, 0.0f));
			}
			else if (objectType == L"Point Light")
			{
				std::vector<std::string> components = { "CPointLight" };
				createComponentObject("Point Light", components, parent);
			}
			else if (objectType == L"Spot Light")
			{
				std::vector<std::string> components = { "CSpotLight" };
				CGameObject* go = createComponentObject("Spotlight", components, parent);
				go->getTransformEuler()->setRotation(core::vector3df(90.0f, 0.0f, 0.0f));
			}
			else if (objectType == L"Reflection Probe")
			{
				std::vector<std::string> components = { "CReflectionProbe" };
				createComponentObject("Reflection Probe", components, parent);
			}
			else if (objectType == L"Light Probes")
			{
				std::vector<std::string> components = { "CLightProbes" };
				createComponentObject("Light Probes", components, parent);
			}
			else if (objectType == L"Cube")
			{
				std::vector<std::string> components = { "CCube" };
				createComponentObject("Cube", components, parent);
			}
			else if (objectType == L"Sphere")
			{
				std::vector<std::string> components = { "CSphere" };
				createComponentObject("Sphere", components, parent);
			}
			else if (objectType == L"Plane")
			{
				std::vector<std::string> components = { "CPlane" };
				createComponentObject("Plane", components, parent);
			}
			else if (objectType == L"Plane")
			{
				std::vector<std::string> components = { "CPlane" };
				createComponentObject("Plane", components, parent);
			}
			else if (objectType == L"Capsule")
			{
				std::vector<std::string> components = { "CCapsule", "CIndirectLighting" };
				createComponentObject("Capsule", components, parent);
			}
			else if (objectType == L"Cylinder")
			{
				std::vector<std::string> components = { "CCylinder", "CIndirectLighting" };
				createComponentObject("Cylinder", components, parent);
			}
			else if (objectType == L"Particle")
			{
				std::vector<std::string> components = { "CParticleComponent" };
				createComponentObject("Particle", components, parent);
			}
		}

		void CSceneController::onContextMenu(CHierachyNode* node)
		{
			if (m_canvas == NULL)
				return;

			if (m_contextMenuScene->onContextMenu(m_spaceHierarchy, node, m_scene, m_zone))
				m_contextNode = node;
		}

		void CSceneController::createZone(bool saveHistory)
		{
			CZone* zone = m_scene->createZone();

			CHierachyNode* node = m_hierachyNode->addChild();
			node->setName(zone->getName());
			node->setIcon(GUI::ESystemIcon::Folder);
			node->setTagData(zone, CHierachyNode::Zone);

			setNodeEvent(node);

			if (m_spaceHierarchy != NULL)
				m_spaceHierarchy->addToTreeNode(node);

			if (saveHistory)
				m_history->saveCreateHistory(zone);

			if (m_zone == NULL)
				setZone(zone);
		}

		void CSceneController::removeObject(CGameObject* gameObject)
		{
			// remove gui hierachy
			if (m_hierachyNode != NULL)
			{
				CHierachyNode* node = m_hierachyNode->getNodeByTag(gameObject);
				if (node != NULL)
					node->remove();
			}

			// remove gameobject
			onDeleteObject(gameObject);

			gameObject->remove();
		}

		CZone* CSceneController::createZoneObject(CObjectSerializable* data, CGameObject* before, bool saveHistory)
		{
			CZone* newObject = m_scene->createZone();

			// create zone data
			newObject->loadSerializable(data);
			newObject->startComponent();

			CHierachyNode* node = NULL;
			CHierachyNode* parentNode = m_hierachyNode;
			if (parentNode != NULL)
			{
				node = parentNode->addChild();
				node->setName(newObject->getName());
				node->setIcon(GUI::ESystemIcon::Folder);
				node->setTagData(newObject, CHierachyNode::Container);

				setNodeEvent(node);

				if (m_spaceHierarchy != NULL)
					m_spaceHierarchy->addToTreeNode(node);
			}

			if (before)
			{
				CHierachyNode* beforeNode = m_hierachyNode->getNodeByTag(before);
				if (beforeNode && node)
				{
					CHierachyNode* parentNode = node->getParent();
					parentNode->bringToNext(node, beforeNode, true);

					if (node->getGUINode())
						node->getGUINode()->bringNextToControl(beforeNode ? beforeNode->getGUINode() : NULL, true);
				}
				m_scene->bringToNext(newObject, (CZone*)before, true);
			}

			if (saveHistory)
				m_history->saveCreateHistory(newObject);

			m_scene->updateAddRemoveObject();
			m_scene->updateIndexSearchObject();

			return newObject;
		}

		CGameObject* CSceneController::createGameObject(CContainerObject* parent, CGameObject* before, CObjectSerializable* data, bool saveHistory)
		{
			CContainerObject* p = parent == NULL ? m_zone : parent;
			if (p == NULL)
				return NULL;

			CGameObject* newObject = p->createObject(data, false);

			p->getZone()->updateAddRemoveObject();
			p->getZone()->updateIndexSearchObject();

			CHierachyNode* node = NULL;
			CHierachyNode* parentNode = m_hierachyNode->getNodeByTag(p);
			if (parentNode != NULL)
			{
				node = buildHierarchyData(newObject, parentNode);
				if (m_spaceHierarchy != NULL)
					m_spaceHierarchy->addToTreeNode(node);
			}

			bool behind = true;
			if (before == NULL && p->getChilds()->size() > 0)
			{
				// insert at first
				ArrayGameObject* childs = p->getChilds();
				for (CGameObject* obj : *childs)
				{
					if (!obj->isEditorObject())
					{
						before = obj;
						behind = false;
						break;
					}
				}
			}

			if (before)
			{
				CHierachyNode* beforeNode = m_hierachyNode->getNodeByTag(before);
				if (beforeNode && node)
				{
					CHierachyNode* parentNode = node->getParent();
					parentNode->bringToNext(node, beforeNode, behind);
					if (node->getGUINode())
						node->getGUINode()->bringNextToControl(beforeNode ? beforeNode->getGUINode() : NULL, behind);
				}
				p->bringToNext(newObject, before, behind);
			}

			if (saveHistory)
				m_history->saveCreateHistory(newObject);

			return newObject;
		}

		CGameObject* CSceneController::createEmptyObject(CContainerObject* parent, bool saveHistory)
		{
			CContainerObject* p = parent == NULL ? m_zone : parent;
			if (p == NULL)
				return NULL;

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
				CHierarchyController::updateObjectToUI(newObject, node);

				if (m_spaceHierarchy != NULL)
					m_spaceHierarchy->addToTreeNode(node);
			}

			if (saveHistory)
				m_history->saveCreateHistory(newObject);

			return newObject;
		}

		CGameObject* CSceneController::createContainerObject(CContainerObject* parent, bool saveHistory)
		{
			CContainerObject* p = parent == NULL ? m_zone : parent;
			if (p == NULL)
				return NULL;

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
				CHierarchyController::updateObjectToUI(newObject, node);

				if (m_spaceHierarchy != NULL)
					m_spaceHierarchy->addToTreeNode(node);
			}

			if (saveHistory)
				m_history->saveCreateHistory(newObject);

			return newObject;
		}

		CGameObject* CSceneController::createComponentObject(const char* name, std::vector<std::string>& components, CContainerObject* parent, bool saveHistory)
		{
			CContainerObject* p = parent == NULL ? m_zone : parent;
			if (p == NULL)
				return NULL;

			CGameObject* newObject = p->createEmptyObject();

			std::string generateName = p->generateObjectName(name);
			newObject->setName(generateName.c_str());

			p->getZone()->updateAddRemoveObject();
			p->getZone()->updateIndexSearchObject();

			// add components
			for (std::string& comp : components)
				newObject->addComponentByTypeName(comp.c_str());

			CHierachyNode* parentNode = m_hierachyNode->getNodeByTag(p);
			if (parentNode != NULL)
			{
				CHierachyNode* node = parentNode->addChild();
				node->setName(newObject->getName());
				node->setIcon(GUI::ESystemIcon::Res3D);
				node->setTagData(newObject, CHierachyNode::GameObject);

				setNodeEvent(node);
				CHierarchyController::updateObjectToUI(newObject, node);

				rebuildHierarchyEntityData(newObject, node);

				if (m_spaceHierarchy != NULL)
					m_spaceHierarchy->addToTreeNode(node);
			}

			if (saveHistory)
				m_history->saveCreateHistory(newObject);

			return newObject;
		}

		void CSceneController::createResourceComponent(const std::string& path, CGameObject* gameObject)
		{
			std::string shortPath = CAssetManager::getInstance()->getShortPath(path.c_str());

			std::string fileExt = CPath::getFileNameExt(path);
			fileExt = CStringImp::toLower(fileExt);

			if (fileExt == "dae" ||
				fileExt == "obj" ||
				fileExt == "fbx" ||
				fileExt == "smesh")
			{
				std::string defaultMaterial;
				std::string meta = path + ".meta";
				bool useNormalMap = true;
				bool useUV2 = false;

				if (fileExt == "dae" ||
					fileExt == "obj" ||
					fileExt == "fbx")
				{
					CMeshExportSettings* setting = new CMeshExportSettings();
					if (setting->load(meta.c_str()))
					{
						defaultMaterial = setting->DefaultMaterial.get();
						useNormalMap = setting->UseNormalMap.get();
						useUV2 = setting->UseUV2.get();
					}
					delete setting;
				}

				CRenderMesh* renderMesh = gameObject->addComponent<CRenderMesh>();
				renderMesh->initFromMeshFile(shortPath.c_str(), useNormalMap, useUV2);

				if (!defaultMaterial.empty())
					renderMesh->initMaterialFromFile(defaultMaterial.c_str());

				CIndirectLighting* indirectLighting = gameObject->addComponent<CIndirectLighting>();
				indirectLighting->setAutoSH(true);
				indirectLighting->setIndirectLightingType(CIndirectLighting::SH9);
			}
			else if (fileExt == "particle")
			{
				Particle::CParticleComponent* particle = gameObject->addComponent<Particle::CParticleComponent>();
				particle->setSourcePath(shortPath.c_str());
				particle->load();
			}
		}

		CGameObject* CSceneController::createTemplateObject(const std::string& path, CContainerObject* container, bool saveHistory)
		{
			CGameObject* result = CSceneImporter::importTemplate(container, path.c_str());

			if (result)
			{
				result->getTransform()->setWorldMatrix(core::IdentityMatrix);
				result->getZone()->updateAddRemoveObject();
				result->getZone()->updateIndexSearchObject();

				CHierachyNode* parentNode = m_hierachyNode->getNodeByTag(container);
				if (parentNode != NULL)
				{
					CHierachyNode* node = buildHierarchyData(result, parentNode);

					if (m_spaceHierarchy != NULL)
						m_spaceHierarchy->addToTreeNode(node);
				}

				if (saveHistory)
					m_history->saveCreateHistory(result);
			}

			return result;
		}

		void CSceneController::deselectAllOnHierachy(bool callEvent)
		{
			if (m_spaceHierarchy)
				m_spaceHierarchy->deselectAll(callEvent);
		}

		CHierachyNode* CSceneController::deselectOnHierachy(CGameObject* gameObject, bool callEvent)
		{
			CHierachyNode* node = m_hierachyNode->getNodeByTag(gameObject);
			if (node != NULL)
			{
				GUI::CTreeNode* treeNode = node->getGUINode();
				treeNode->setSelected(false, callEvent);
			}
			return node;
		}

		CHierachyNode* CSceneController::deselectOnHierachy(CEntity* entity, bool callEvent)
		{
			CHierachyNode* node = m_hierachyNode->getNodeByTag(entity);
			if (node != NULL)
			{
				GUI::CTreeNode* treeNode = node->getGUINode();
				if (treeNode)
					treeNode->setSelected(false, callEvent);
			}
			return node;
		}

		CHierachyNode* CSceneController::selectOnHierachy(CGameObject* gameObject, bool callEvent)
		{
			CHierachyNode* node = m_hierachyNode->getNodeByTag(gameObject);
			if (node != NULL)
			{
				GUI::CTreeNode* treeNode = node->getGUINode();
				if (treeNode)
				{
					treeNode->setSelected(true, callEvent);
					m_spaceHierarchy->scrollToNode(treeNode);
				}
			}
			return node;
		}

		CHierachyNode* CSceneController::selectOnHierachy(CEntity* entity, bool callEvent)
		{
			CHierachyNode* node = m_hierachyNode->getNodeByTag(entity);
			if (node != NULL)
			{
				GUI::CTreeNode* treeNode = node->getGUINode();
				if (treeNode)
				{
					treeNode->setSelected(true, callEvent);
					m_spaceHierarchy->scrollToNode(treeNode);
				}
			}
			return node;
		}

		void CSceneController::setNodeEvent(CHierachyNode* node)
		{
			node->OnUpdate = std::bind(&CSceneController::onUpdateNode, this, std::placeholders::_1);
			node->OnSelected = std::bind(&CSceneController::onSelectNode, this, std::placeholders::_1, std::placeholders::_2);
			node->OnDoubleClick = std::bind(&CSceneController::onDoubleClickNode, this, std::placeholders::_1);
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

				// Set property & event
				CPropertyController* propertyController = CPropertyController::getInstance();
				if (selected)
				{
					// add gizmos
					setGizmos(m_transformGizmos);

					CSelectObject* selectedObject = selection->addSelect(obj);
					propertyController->setProperty(selectedObject);

					// add new observer
					selectedObject->addObserver(this);

					// add select history
					m_history->addSelectHistory();
				}
				else
				{
					propertyController->setProperty(NULL);
					selection->unSelect(obj);
				}

				// particle controller
				CParticleController::getInstance()->setGameObject(obj);
			}
			else if (node->isTagEntity())
			{
				CEntity* entity = (CEntity*)node->getTagData();

				// Set property & event
				CPropertyController* propertyController = CPropertyController::getInstance();
				if (selected)
				{
					CSelectObject* selectedObject = selection->addSelect(entity);
					propertyController->setProperty(selectedObject);

					// add gizmos
					setGizmos(m_worldTransformDataGizmos);

					// add new observer
					selectedObject->addObserver(this);

					// add select history
					m_history->addSelectHistory();
				}
				else
				{
					selection->unSelect(entity);
					propertyController->setProperty(NULL);
				}
			}
		}

		void CSceneController::onDoubleClickNode(CHierachyNode* node)
		{
			if (node->isTagGameObject())
			{
				CGameObject* obj = (CGameObject*)node->getTagData();
				focusCameraToObject(obj);
			}
			else if (node->isTagEntity())
			{
				CEntity* entity = (CEntity*)node->getTagData();
				focusCameraToEntity(entity);
			}
		}

		void CSceneController::focusCameraToSelectObject()
		{
			CSelectObject* selectedObject = CSelection::getInstance()->getLastSelected();
			if (selectedObject != NULL)
			{
				if (selectedObject->getType() == CSelectObject::GameObject)
				{
					CGameObject* obj = m_scene->searchObjectInChildByID(selectedObject->getID().c_str());
					if (obj != NULL)
					{
						focusCameraToObject(obj);
					}
				}
				else if (selectedObject->getType() == CSelectObject::Entity)
				{
					CEntity* entity = m_scene->getEntityManager()->getEntityByID(selectedObject->getID().c_str());
					if (entity != NULL)
					{
						focusCameraToEntity(entity);
					}
				}
			}
		}

		void CSceneController::onObjectChange(CGameObject* object)
		{
			CHierachyNode* node = NULL;
			if (m_focusNode->getTagData() == object)
				node = m_focusNode;

			if (node == NULL)
				node = m_hierachyNode->getNodeByTag(object);

			if (node != NULL && m_spaceHierarchy != NULL)
			{
				CHierarchyController::updateObjectToUI(object, node);
			}
		}

		void CSceneController::onHistoryModifyObject(CGameObject* object)
		{
			if (m_spaceHierarchy != NULL)
				m_spaceHierarchy->getController()->updateTreeNode(object);
		}

		void CSceneController::onMoveStructure(CGameObject* object, CContainerObject* toContainer, CGameObject* before)
		{
			bool behind = true;
			if (before == NULL && toContainer->getChilds()->size() > 0)
			{
				// insert at first
				ArrayGameObject* childs = toContainer->getChilds();
				for (CGameObject* obj : *childs)
				{
					if (!obj->isEditorObject())
					{
						before = obj;
						behind = false;
						break;
					}
				}
			}

			if (before)
			{
				CHierachyNode* containerNode = m_hierachyNode->getNodeByTag(toContainer);
				CHierachyNode* beforeNode = m_hierachyNode->getNodeByTag(before);
				CHierachyNode* node = m_hierachyNode->getNodeByTag(object);
				if (containerNode && node)
				{
					node->removeGUI();
					node->nullGUI();

					containerNode->bringToNext(node, beforeNode, behind);

					if (m_spaceHierarchy)
					{
						m_spaceHierarchy->addToTreeNode(node);
						node->getGUINode()->bringNextToControl(beforeNode ? beforeNode->getGUINode() : NULL, behind);
					}
				}
				toContainer->bringToNext(object, before, behind);
			}
			else
			{
				CHierachyNode* container = m_hierachyNode->getNodeByTag(toContainer);
				CHierachyNode* node = m_hierachyNode->getNodeByTag(object);
				if (node)
				{
					node->removeGUI();
					node->nullGUI();

					if (container)
						container->bringToChild(node);

					if (m_spaceHierarchy != NULL)
						m_spaceHierarchy->addToTreeNode(node);

				}
				toContainer->bringToChild(object);
			}

			m_scene->updateAddRemoveObject();
			m_scene->updateIndexSearchObject();
		}

		void CSceneController::onMoveStructure(CZone* object, CZone* before)
		{
			bool behind = true;
			if (before == NULL && m_scene->getAllZone()->size() > 0)
			{
				// insert at first
				ArrayZone* zones = m_scene->getAllZone();
				for (CZone* obj : *zones)
				{
					if (!obj->isEditorObject())
					{
						before = obj;
						behind = false;
						break;
					}
				}
			}

			if (before)
			{
				CHierachyNode* beforeNode = m_hierachyNode->getNodeByTag(before);
				CHierachyNode* node = m_hierachyNode->getNodeByTag(object);
				if (node)
				{
					node->removeGUI();
					node->nullGUI();

					m_hierachyNode->bringToNext(node, beforeNode, behind);

					if (m_spaceHierarchy != NULL)
					{
						m_spaceHierarchy->addToTreeNode(node);
						node->getGUINode()->bringNextToControl(beforeNode ? beforeNode->getGUINode() : NULL, behind);
					}
				}
				m_scene->bringToNext(object, before, behind);
			}

			m_scene->updateAddRemoveObject();
			m_scene->updateIndexSearchObject();
		}

		void CSceneController::onDeleteObject(CGameObject* object)
		{
			if (CSelection::getInstance()->unSelect(object))
			{
				// clear property this object
				CPropertyController::getInstance()->setProperty(NULL);
			}

			// clear particle
			CParticleController::getInstance()->setGameObject(NULL);

			if (m_zone == object)
			{
				CZone* defaultZone = m_scene->getZone(0);
				if (defaultZone == object)
					setZone(NULL);
				else
					setZone(defaultZone);
			}
		}

		void CSceneController::onDelete()
		{
			CSelection* selection = CSelection::getInstance();
			std::vector<CSelectObject*> selected = selection->getAllSelected();

			// need save delete history first
			{
				std::vector<CGameObject*> deleteObjects;
				for (CSelectObject* selectObject : selected)
				{
					CSelectObject::ESelectType type = selectObject->getType();
					if (type == CSelectObject::GameObject)
					{
						CGameObject* gameObject = m_scene->searchObjectInChildByID(selectObject->getID().c_str());
						if (gameObject != NULL)
							deleteObjects.push_back(gameObject);
					}
				}

				if (deleteObjects.size() > 0)
					m_history->saveDeleteHistory(deleteObjects);
			}

			std::vector<CGameObject*> modifyGameObjects;

			// loop and delete all selected
			for (CSelectObject* selectObject : selected)
			{
				CSelectObject::ESelectType type = selectObject->getType();

				if (type == CSelectObject::GameObject)
				{
					CGameObject* gameObject = m_scene->searchObjectInChildByID(selectObject->getID().c_str());
					if (gameObject != NULL)
					{
						// remove gui hierachy
						if (m_hierachyNode != NULL)
						{
							CHierachyNode* node = m_hierachyNode->getNodeByTag(gameObject);
							if (node != NULL)
								node->remove();
						}

						onDeleteObject(gameObject);

						// remove gameobject
						gameObject->remove();
					}
				}
				else if (type == CSelectObject::Entity)
				{
					CEntity* entity = m_scene->getEntityManager()->getEntityByID(selectObject->getID().c_str());
					if (entity != NULL)
					{
						// delete entity
						CEntityHandleData* data = GET_ENTITY_DATA(entity, CEntityHandleData);
						if (data != NULL)
						{
							CEntityHandler* handler = data->Handler;
							handler->removeEntity(entity);

							// remove gui hierachy
							if (m_spaceHierarchy != NULL)
								m_spaceHierarchy->getController()->updateTreeNode(handler->getGameObject());

							modifyGameObjects.push_back(handler->getGameObject());
						}
					}
				}
			}

			// save modify entity
			if (modifyGameObjects.size() > 0)
				m_history->saveModifyHistory(modifyGameObjects);

			selection->clear();
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

			std::vector<CGameObject*> newObjects;

			CSelectObject* lastSelected = selection->getLastSelected();
			if (lastSelected == NULL)
			{
				CZone* zone = getZone();
				if (zone == NULL)
					return;

				newObjects = CCopyPaste::getInstance()->paste(zone);
				m_spaceHierarchy->getController()->updateTreeNode(zone);
			}
			else
			{
				if (lastSelected->getType() == CSelectObject::GameObject)
				{
					CGameObject* gameObject = m_scene->searchObjectInChildByID(lastSelected->getID().c_str());
					if (gameObject != NULL)
					{
						CZone* zone = dynamic_cast<CZone*>(gameObject);
						if (zone != NULL)
							newObjects = CCopyPaste::getInstance()->paste(zone);
						else
						{
							CContainerObject* container = dynamic_cast<CContainerObject*>(gameObject->getParent());
							newObjects = CCopyPaste::getInstance()->paste(container);
						}
					}
				}
				else if (lastSelected->getType() == CSelectObject::Entity)
				{
					CEntity* entity = m_scene->getEntityManager()->getEntityByID(lastSelected->getID().c_str());
					if (entity != NULL)
					{
						CEntityHandleData* data = GET_ENTITY_DATA(entity, CEntityHandleData);

						CGameObject* gameObject = data->Handler->getGameObject();
						if (gameObject != NULL)
						{
							CContainerObject* container = dynamic_cast<CContainerObject*>(gameObject);
							if (container != NULL)
								newObjects = CCopyPaste::getInstance()->paste(container);
							else
							{
								container = dynamic_cast<CContainerObject*>(gameObject->getParent());
								newObjects = CCopyPaste::getInstance()->paste(container);
							}
						}
					}
				}
			}

			selection->clear();
			selection->addSelect(newObjects);

			m_history->saveCreateHistory(newObjects);
		}

		void CSceneController::onDuplicate()
		{
			// Check duplicate entity
			CSelection* selection = CSelection::getInstance();
			std::vector<CSelectObject*>& selected = selection->getAllSelected();
			std::vector<CEntity*> newEntities;
			std::vector<CGameObject*> updateObjects;

			// Duplicate entities
			for (CSelectObject* selectObject : selected)
			{
				CSelectObject::ESelectType type = selectObject->getType();
				if (type == CSelectObject::Entity)
				{
					CEntity* entity = m_scene->getEntityManager()->getEntityByID(selectObject->getID().c_str());
					if (entity != NULL)
					{
						CEntityHandleData* data = GET_ENTITY_DATA(entity, CEntityHandleData);
						CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);

						if (data != NULL)
						{
							CEntityHandler* handler = data->Handler;
							CGameObject* handlerObj = handler->getGameObject();

							// spawn new entity
							CEntity* spawnNewEntity = handler->spawn();
							if (spawnNewEntity != NULL)
							{
								CWorldTransformData* spawnTransform = GET_ENTITY_DATA(spawnNewEntity, CWorldTransformData);
								spawnTransform->Relative = transform->Relative;

								newEntities.push_back(spawnNewEntity);

								// add to list to update history
								bool addObjToList = true;
								for (CGameObject* obj : updateObjects)
								{
									if (obj == handlerObj)
									{
										addObjToList = false;
										break;
									}
								}
								if (addObjToList)
									updateObjects.push_back(handlerObj);

								// remove gui hierachy
								if (m_spaceHierarchy != NULL)
									m_spaceHierarchy->getController()->updateTreeNode(handlerObj);
							}
						}
					}
				}
			}

			if (newEntities.size() > 0)
			{
				// change selection
				selection->clear();

				for (CEntity* e : newEntities)
				{
					selection->addSelect(e);
					selectOnHierachy(e);
				}

				// add history
				m_history->saveModifyHistory(updateObjects);
				return;
			}

			// Duplicate game object
			onCopy();
			onPaste();
		}

		void CSceneController::onCut()
		{
			onCopy();
			onDelete();
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

		void CSceneController::onCreateTemplate(CGameObject* object)
		{
			CAssetCreateController::getInstance()->createTemplate(object);

			if (m_spaceHierarchy)
				m_spaceHierarchy->getController()->updateTreeNode(object);
		}

		void CSceneController::onCreateTemplate(CGameObject* object, const char* folder)
		{
			CAssetCreateController::getInstance()->createTemplate(object, folder);

			if (m_spaceHierarchy)
				m_spaceHierarchy->getController()->updateTreeNode(object);
		}

		void CSceneController::onApplyTemplate(CGameObject* object)
		{
			CAssetCreateController::getInstance()->applyTemplate(object);

			if (object->isTemplateAsset())
				m_spaceScene->getEditor()->initApplyTemplateGUI(object->getTemplateAsset());
		}

		void CSceneController::doFinishApplyTemplate(std::list<CGameObject*>& objects)
		{
			m_scene->updateAddRemoveObject();

			for (CGameObject* obj : objects)
			{
				rebuildHierarchyData(obj);

				if (m_spaceHierarchy)
					m_spaceHierarchy->getController()->updateTreeNode(obj, true);
			}

			CSelectObject* selectObject = CSelection::getInstance()->getLastSelected();
			CPropertyController::getInstance()->setProperty(selectObject);
		}

		void CSceneController::onRevertTemplate(CGameObject* object)
		{
			if (!object->isTemplateAsset())
				return;

			std::string templatePath = object->getTemplateAsset();

			CObjectSerializable* templateData = CSceneImporter::importTemplateToObject(m_zone, templatePath.c_str());
			if (templateData)
			{
				CSceneImporter::reloadTemplate(object, templateData);
				delete templateData;

				m_scene->updateAddRemoveObject();

				rebuildHierarchyData(object);

				if (m_spaceHierarchy)
					m_spaceHierarchy->getController()->updateTreeNode(object, true);

				CSelectObject* selectObject = CSelection::getInstance()->getLastSelected();
				CPropertyController::getInstance()->setProperty(selectObject);
			}
		}

		void CSceneController::onUnpackTemplate(CGameObject* object)
		{
			object->unpackTemplate();

			if (m_spaceHierarchy)
				m_spaceHierarchy->getController()->updateTreeNode(object);
		}

		void CSceneController::focusCameraToEntity(CEntity* entity)
		{
			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			
			core::vector3df objectPosition = transform->World.getTranslation();

			CCamera* camera = m_spaceScene->getEditorCamera();
			core::vector3df look = -camera->getLookVector();
			core::vector3df cameraPosition = camera->getPosition();

			float cos = look.dotProduct(Transform::Oy);

			float yDistance = fabsf(cameraPosition.Y - objectPosition.Y);
			float d = yDistance;
			const float maxDistance = 20.0f;
			if (cos != 0)
			{
				d = yDistance / cos;
				d = core::clamp(d, -maxDistance, maxDistance);
			}
			
			core::vector3df newPosition = objectPosition + look * d;
			camera->lookAt(newPosition, objectPosition, Transform::Oy);
		}
	
		void CSceneController::focusCameraToObject(CGameObject* obj)
		{
			bool haveBBox = false;
			core::aabbox3df bbox;
			
			// billboard object (ex: light...)
			if (obj->getComponent<CSprite>() != NULL)
				return focusCameraToEntity(obj->getEntity());
			
			CSelectObjectSystem *selectObjectSystem = m_scene->getEntityManager()->getSystem<CSelectObjectSystem>();
			haveBBox = selectObjectSystem->getTransformBBox(obj, bbox);
			if (!haveBBox)
				return focusCameraToEntity(obj->getEntity());
			
			CCamera* camera = m_spaceScene->getEditorCamera();
			core::vector3df lookAt = bbox.getCenter();

			float diagonalLength = bbox.getExtent().getLength();
			float fovRadians = camera->getFOV() * core::DEGTORAD;
			float distance = (diagonalLength * 0.5f) / tanf(fovRadians * 0.5f) * 1.5f;
			
			core::vector3df camDirection = -camera->getLookVector();
			camDirection.normalize();

			camera->setPosition(lookAt + camDirection * distance);
			camera->lookAt(lookAt, Transform::Oy);
		}
	
		void CSceneController::applySelected(std::vector<CSelectObject*> ids)
		{
			m_scene->updateAddRemoveObject();
			m_scene->updateIndexSearchObject();

			std::vector<CSelectObject*> listIds;

			for (CSelectObject* id : ids)
			{
				if (id->getType() == CSelectObject::GameObject)
				{
					CGameObject* go = m_scene->searchObjectInChildByID(id->getID().c_str());
					if (go)
					{
						selectOnHierachy(go, false);
						listIds.push_back(id);

						m_history->beginSaveHistory(go);
					}
				}
				else if (id->getType() == CSelectObject::Entity)
				{
					CEntity* entity = m_scene->searchEntityInChildByID(id->getID().c_str());
					if (entity)
					{
						selectOnHierachy(entity, false);
						listIds.push_back(id);

						CEntityHandleData* data = GET_ENTITY_DATA(entity, CEntityHandleData);
						if (data && data->Handler)
							m_history->beginSaveHistory(data->Handler->getGameObject());
					}
				}
			}

			CSelection::getInstance()->applySelected(listIds);
		}
	}
}
