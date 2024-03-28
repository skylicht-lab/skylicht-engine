/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CCopyPaste.h"
#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CCopyPaste)

		CCopyPaste::CCopyPaste()
		{

		}

		CCopyPaste::~CCopyPaste()
		{
			clear();
		}

		void CCopyPaste::clear()
		{
			for (CObjectSerializable* gameObject : m_gameObjects)
				delete gameObject;
			m_gameObjects.clear();
		}

		void CCopyPaste::copy(std::vector<CGameObject*>& objects)
		{
			clear();

			// find container
			std::vector<CContainerObject*> listContainer;
			for (CGameObject* gameObject : objects)
			{
				CContainerObject* container = dynamic_cast<CContainerObject*>(gameObject);
				if (container != NULL)
					listContainer.push_back(container);
			}

			// loop all and add copy
			for (CGameObject* gameObject : objects)
			{
				if (!checkInsideParent(gameObject, listContainer))
				{
					m_gameObjects.push_back(CSceneExporter::exportGameObject(gameObject));
				}
			}
		}

		bool CCopyPaste::checkInsideParent(CGameObject* gameObject, std::vector<CContainerObject*> list)
		{
			for (CContainerObject* container : list)
			{
				if (container->haveChild(gameObject))
				{
					return true;
				}
			}

			return false;
		}

		std::vector<CGameObject*> CCopyPaste::paste(CContainerObject* target)
		{
			std::vector<CGameObject*> results;

			CSceneController* sceneController = CSceneController::getInstance();
			CHierachyNode* parentNode = NULL;
			CHierarchyController* hierarchyController = NULL;

			if (sceneController->getSpaceHierarchy() != NULL)
			{
				hierarchyController = sceneController->getSpaceHierarchy()->getController();
				parentNode = hierarchyController->getNodeByObject(target);
			}

			for (CObjectSerializable* objectData : m_gameObjects)
			{
				// paste object data
				CGameObject* gameObject = target->createObject(objectData, true);

				if (gameObject)
					results.push_back(gameObject);

				// update on GUI editor
				if (hierarchyController != NULL && gameObject != NULL && parentNode != NULL)
				{
					GUI::CTreeNode* node = hierarchyController->addToTreeNode(sceneController->buildHierarchyData(gameObject, parentNode));
					node->collapse(false);
					node->setSelected(true);
				}
			}

			return results;
		}
	}
}