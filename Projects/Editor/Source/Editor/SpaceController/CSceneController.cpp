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
			m_sceneNode(NULL)
		{

		}

		CSceneController::~CSceneController()
		{
			if (m_sceneNode != NULL)
				delete m_sceneNode;
		}

		void CSceneController::setScene(CScene* scene)
		{
			if (m_sceneNode != NULL)
			{
				delete m_sceneNode;
				m_sceneNode = NULL;
			}

			m_scene = scene;
			if (m_scene == NULL)
				return;

			m_scene->update();

			m_sceneNode = new CHierachyNode(NULL);
			m_sceneNode->setName(L"Scene");
			m_sceneNode->setIcon(GUI::ESystemIcon::Folder);
			m_sceneNode->setTagData(m_scene);

			buildHierarchyNodes();

			m_spaceHierarchy->setHierarchyNode(m_sceneNode);
		}

		void CSceneController::buildHierarchyNodes()
		{
			ArrayZone* zones = m_scene->getAllZone();
			for (CZone* zone : *zones)
			{
				buildHierarchyNodes(zone, m_sceneNode)->setIcon(GUI::ESystemIcon::Collection);
			}
		}

		CHierachyNode* CSceneController::buildHierarchyNodes(CGameObject* object, CHierachyNode* parentNode)
		{
			CContainerObject* container = dynamic_cast<CContainerObject*>(object);

			CHierachyNode* node = NULL;

			if (container != NULL)
			{
				// add container child
				node = parentNode->addChild();
				node->setName(object->getName());
				node->setIcon(GUI::ESystemIcon::Folder);
				node->setTagData(object);

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
				node->setTagData(object);
			}

			return node;
		}
	}
}