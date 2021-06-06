/*
!@
MIT License

CopyRight (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CSceneController.h"

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
			m_canvas(NULL)
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
			m_hierachyNode->setName(L"Scene");
			m_hierachyNode->setIcon(GUI::ESystemIcon::Folder);
			m_hierachyNode->setTagData(m_scene, CHierachyNode::Scene);

			buildHierarchyNodes();

			if (m_spaceHierarchy != NULL)
				m_spaceHierarchy->setHierarchyNode(m_hierachyNode);
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
				if (node != NULL)
					node->setIcon(GUI::ESystemIcon::Collection);
			}
		}

		CHierachyNode* CSceneController::buildHierarchyNodes(CGameObject* object, CHierachyNode* parentNode)
		{
			if (object->isEditorObject())
				return NULL;

			CContainerObject* container = dynamic_cast<CContainerObject*>(object);

			CHierachyNode* node = NULL;

			if (container != NULL)
			{
				// add container child
				node = parentNode->addChild();
				node->setName(object->getName());
				node->setIcon(GUI::ESystemIcon::Folder);
				node->setTagData(object, CHierachyNode::GameObject);

				// enable color on current zone
				if (m_zone == object)
					node->EnableColor(true);

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

			return node;
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

			if (m_contextMenuScene->onContextMenu(node, m_scene, m_zone))
				m_contextNode = node;
		}

		void CSceneController::createZone()
		{
			CZone* zone = m_scene->createZone();

			CHierachyNode* node = m_hierachyNode->addChild();
			node->setName(zone->getName());
			node->setIcon(GUI::ESystemIcon::Collection);
			node->setTagData(zone, CHierachyNode::GameObject);

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
				node->setTagData(newObject, CHierachyNode::GameObject);

				if (m_spaceHierarchy != NULL)
					m_spaceHierarchy->add(node);
			}
		}
	}
}