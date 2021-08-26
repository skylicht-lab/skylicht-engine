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
			m_contextMenuScene(NULL)
		{
		}

		CSceneController::~CSceneController()
		{
			delete m_contextMenuScene;

			if (m_hierachyNode != NULL)
				delete m_hierachyNode;
		}

		void CSceneController::initContextMenu(GUI::CCanvas* canvas)
		{
			m_canvas = canvas;
			m_contextMenuScene = new CContextMenuScene(canvas);
		}

		void CSceneController::setScene(CScene* scene, CSceneHistory* history)
		{
			if (m_hierachyNode != NULL)
			{
				delete m_hierachyNode;
				m_hierachyNode = NULL;
			}

			m_scene = scene;
			m_history = history;

			if (m_scene == NULL)
				return;

			m_scene->update();

			setZone(m_scene->getZone(0));

			m_hierachyNode = new CHierachyNode(NULL);
			m_hierachyNode->setName(m_scene->getName());
			m_hierachyNode->setIcon(GUI::ESystemIcon::Folder);
			m_hierachyNode->setTagData(m_scene, CHierachyNode::Scene);

			setNodeEvent(m_hierachyNode);

			buildHierarchyNodes();

			if (m_spaceHierarchy != NULL)
				m_spaceHierarchy->setHierarchyNode(m_hierachyNode);

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
					node->setIcon(GUI::ESystemIcon::Collection);
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
		}

		void CSceneController::refresh()
		{
			CSelection* selection = CSelection::getInstance();

			CSelectObject* selectedObject = selection->getLastSelected();
			if (selectedObject != NULL)
				selectedObject->removeAllObserver();
			else
				return;

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
				return;

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
			CHierachyNode::EDataType dataType = node->getTagDataType();
			if (dataType == CHierachyNode::GameObject || dataType == CHierachyNode::Container)
			{
				CGameObject* obj = (CGameObject*)node->getTagData();
				obj->setName(node->getName().c_str());

				// notify object changed
				CSelectObject* selectedObject = CSelection::getInstance()->getSelected(obj);
				if (selectedObject != NULL)
					selectedObject->notify(this);
			}
			else if (dataType == CHierachyNode::Scene)
			{
				CScene* scene = (CScene*)node->getTagData();
				scene->setName(node->getName().c_str());
			}
		}

		void CSceneController::onSelectNode(CHierachyNode* node, bool selected)
		{
			m_focusNode = node;

			CHierachyNode::EDataType dataType = node->getTagDataType();
			if (dataType == CHierachyNode::GameObject || dataType == CHierachyNode::Container)
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