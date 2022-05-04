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
#include "Editor/SpaceController/CSceneController.h"
#include "Selection/CSelection.h"

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
			size_t historySize = m_history.size();
			if (historySize == 0)
				return;

			CSceneController* sceneController = CSceneController::getInstance();
			CScene* scene = sceneController->getScene();

			// last history save
			SHistoryData* historyData = m_history[historySize - 1];

			switch (historyData->History)
			{
			case EHistory::Create:
			{
				size_t numObject = historyData->ObjectID.size();
				for (size_t i = 0; i < numObject; i++)
				{
					// object id
					std::string& id = historyData->ObjectID[i];

					// remove this object
					CGameObject* gameObject = scene->searchObjectInChildByID(id.c_str());
					if (gameObject != NULL)
						sceneController->removeObject(gameObject);
				}
			}
			break;
			case EHistory::Modify:
			{
				size_t numObject = historyData->DataModified.size();
				for (size_t i = 0; i < numObject; i++)
				{
					// object id
					std::string& id = historyData->ObjectID[i];

					// old data
					CObjectSerializable* data = historyData->Data[i];

					// get object and undo data
					CGameObject* gameObject = scene->searchObjectInChildByID(id.c_str());
					if (gameObject != NULL)
						gameObject->loadSerializable(data);

					// set current data for next action
					SGameObjectHistory* objHistory = getObjectHistory(id);
					if (objHistory != NULL)
						objHistory->changeData(data);
				}
			}
			break;
			case EHistory::Delete:
			{

			}
			break;
			}

			// move history to redo action
			m_redo.push_back(historyData);
			m_history.erase(m_history.begin() + (historySize - 1));

			// refresh ui on editor
			CEditor::getInstance()->refresh();
		}

		void CSceneHistory::redo()
		{
			size_t historySize = m_redo.size();
			if (historySize == 0)
				return;

			CSceneController* sceneController = CSceneController::getInstance();
			CScene* scene = sceneController->getScene();

			// last history save
			SHistoryData* historyData = m_redo[historySize - 1];

			switch (historyData->History)
			{
			case EHistory::Create:
			{
				size_t numObject = historyData->ObjectID.size();
				for (size_t i = 0; i < numObject; i++)
				{
					std::string& id = historyData->Container[i];
					CContainerObject* containerObject = (CContainerObject*)scene->searchObjectInChildByID(id.c_str());
					if (containerObject != NULL)
						sceneController->createGameObject(containerObject, historyData->Data[i], false);
				}
			}
			break;
			case EHistory::Modify:
			{
				size_t numObject = historyData->DataModified.size();
				for (size_t i = 0; i < numObject; i++)
				{
					// object id
					std::string& id = historyData->ObjectID[i];

					// old data
					CObjectSerializable* data = historyData->DataModified[i];

					// get object and redo data
					CGameObject* gameObject = scene->searchObjectInChildByID(id.c_str());
					if (gameObject != NULL)
						gameObject->loadSerializable(data);

					// set current data for next action
					SGameObjectHistory* objHistory = getObjectHistory(id);
					if (objHistory != NULL)
						objHistory->changeData(data);
				}
			}
			break;
			case EHistory::Delete:
			{
			}
			break;
			}

			// move history to redo action
			m_history.push_back(historyData);
			m_redo.erase(m_redo.begin() + (historySize - 1));

			// refresh ui on editor
			CEditor::getInstance()->refresh();
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

		void CSceneHistory::beginSaveHistory(CGameObject* gameObject)
		{
			std::string objectID = gameObject->getID();

			for (SGameObjectHistory* history : m_objects)
			{
				// no need
				if (history->ObjectID == objectID)
					return;
			}

			SGameObjectHistory* objectData = new SGameObjectHistory();
			objectData->ObjectID = objectID;
			objectData->ObjectData = gameObject->createSerializable();
			m_objects.push_back(objectData);
		}

		void CSceneHistory::removeSaveHistory(CGameObject* gameObject)
		{
			std::string objectID = gameObject->getID();

			std::vector<SGameObjectHistory*>::iterator i = m_objects.begin(), end = m_objects.end();
			while (i != end)
			{
				SGameObjectHistory* history = (*i);
				if (history->ObjectID == objectID)
				{
					delete history->ObjectData;
					delete history;
					m_objects.erase(i);
					return;
				}
				++i;
			}
		}

		void CSceneHistory::saveCreateHistory(CGameObject* gameObject)
		{
			CObjectSerializable* currentData = gameObject->createSerializable();

			std::vector<std::string> container;
			std::vector<std::string> id;
			std::vector<CObjectSerializable*> modifyData;
			std::vector<CObjectSerializable*> objectData;

			// parent container id
			if (gameObject->getParent() != NULL)
			{
				container.push_back(gameObject->getParent()->getID());
			}
			else
			{
				// this is zone (no parent)
				container.push_back("_");
			}

			// game object id
			id.push_back(gameObject->getID());

			// last data object
			objectData.push_back(currentData);

			// current data object
			modifyData.push_back(NULL);

			// save history
			addHistory(EHistory::Create, container, id, modifyData, objectData);
		}

		void CSceneHistory::saveDeleteHistory(std::vector<CGameObject*> gameObject)
		{

		}

		bool CSceneHistory::saveModifyHistory(std::vector<CGameObject*> gameObjects)
		{
			bool success = true;

			std::vector<std::string> container;
			std::vector<std::string> id;
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
				if (gameObject->getParent() != NULL)
				{
					container.push_back(gameObject->getParent()->getID());
				}
				else
				{
					// this is zone (no parent)
					container.push_back("_");
				}

				// game object id
				id.push_back(gameObject->getID());

				// last data object
				objectData.push_back(historyData->ObjectData->clone());

				// current data object
				modifyData.push_back(currentData);

				// change save point
				historyData->changeData(currentData);
			}

			if (success)
			{
				addHistory(EHistory::Modify, container, id, modifyData, objectData);
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