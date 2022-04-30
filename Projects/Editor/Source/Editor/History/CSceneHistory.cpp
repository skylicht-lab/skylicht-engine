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
#include "CSceneHistory.h"
#include "Scene/CSceneExporter.h"

namespace Skylicht
{
	namespace Editor
	{
		CSceneHistory::CSceneHistory(CScene* scene) :
			m_scene(scene)
		{

		}

		CSceneHistory::~CSceneHistory()
		{
			freeCurrentObjectData();
		}

		void CSceneHistory::undo()
		{

		}

		void CSceneHistory::redo()
		{

		}

		void CSceneHistory::freeCurrentObjectData()
		{
			for (SGameObjectHistory* history : m_objects)
			{
				delete history->ObjectData;
				delete history;
			}
			m_objects.clear();
		}

		SGameObjectHistory* CSceneHistory::getObjectHistory(const std::string& id)
		{
			for (SGameObjectHistory* history : m_objects)
			{
				if (history->ObjectID == id)
					return history;
			}
			return NULL;
		}

		void CSceneHistory::beginSaveHistory(std::vector<CGameObject*> gameObjects)
		{
			freeCurrentObjectData();

			for (CGameObject* gameObject : gameObjects)
			{
				SGameObjectHistory* objectData = new SGameObjectHistory();
				objectData->ObjectID = gameObject->getID();
				objectData->ObjectData = gameObject->createSerializable();
				m_objects.push_back(objectData);
			}
		}

		bool CSceneHistory::saveHistory(std::vector<CGameObject*> gameObjects)
		{
			bool success = true;

			std::vector<std::string> container;
			std::vector<CObjectSerializable*> modifyData;
			std::vector<CObjectSerializable*> objectData;

			for (CGameObject* gameObject : gameObjects)
			{
				SGameObjectHistory* historyData = getObjectHistory(gameObject->getID());
				if (historyData == NULL)
				{
					os::Printer::log("[CSceneHistory::saveHistory] failed, call CSceneHistory::beginSaveHistory first!");
					success = false;
					break;
				}

				CObjectSerializable* currentData = gameObject->createSerializable();

				// parent container id
				container.push_back(gameObject->getParent()->getID());

				// last data object
				objectData.push_back(historyData->ObjectData);

				// current data object
				modifyData.push_back(currentData);

				// change save point
				historyData->changeData(currentData);
			}

			if (success)
			{
				addHistory(EHistory::Modify, container, modifyData, modifyData);
			}
			else
			{
				for (CObjectSerializable* objData : modifyData)
					delete objData;
			}

			return success;
		}

		void CSceneHistory::endSaveHistory()
		{
			freeCurrentObjectData();
		}
	}
}