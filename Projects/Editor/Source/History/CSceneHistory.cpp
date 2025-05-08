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

		void CSceneHistory::doDelete(SHistoryData* historyData)
		{
			CSceneController* sceneController = CSceneController::getInstance();
			CScene* scene = sceneController->getScene();

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

		void CSceneHistory::doCreate(SHistoryData* historyData)
		{
			CSceneController* sceneController = CSceneController::getInstance();
			CScene* scene = sceneController->getScene();

			size_t numObject = historyData->ObjectID.size();
			for (size_t i = 0; i < numObject; i++)
			{
				std::string& id = historyData->Container[i];
				std::string& before = historyData->BeforeID[i];

				CGameObject* beforeObject = NULL;

				if (!before.empty())
					beforeObject = scene->searchObjectInChildByID(before.c_str());

				if (id != "_")
				{
					CContainerObject* containerObject = (CContainerObject*)scene->searchObjectInChildByID(id.c_str());
					if (containerObject != NULL)
					{
						sceneController->createGameObject(containerObject, beforeObject, historyData->Data[i], false);
					}
				}
				else
				{
					sceneController->createZoneObject(historyData->Data[i], beforeObject, false);
				}
			}
		}

		void CSceneHistory::doModify(SHistoryData* historyData, bool undo)
		{
			CSceneController* sceneController = CSceneController::getInstance();
			CScene* scene = sceneController->getScene();

			size_t numObject = historyData->DataModified.size();
			for (size_t i = 0; i < numObject; i++)
			{
				// object id
				std::string& id = historyData->ObjectID[i];

				// revert data
				CObjectSerializable* data = undo ? historyData->Data[i] : historyData->DataModified[i];

				// get object and undo data
				CGameObject* gameObject = scene->searchObjectInChildByID(id.c_str());
				if (gameObject != NULL)
					gameObject->loadSerializable(data);

				sceneController->onHistoryModifyObject(gameObject);

				// set current data for next action
				SGameObjectHistory* objHistory = getObjectHistory(id);
				if (objHistory != NULL)
					objHistory->changeData(data);
			}
		}

		void CSceneHistory::undo()
		{
			int historySize = (int)m_history.size();
			if (historySize == 0)
				return;

			m_enable = false;

			// last history save
			SHistoryData* historyData = m_history[historySize - 1];

			switch (historyData->History)
			{
			case EHistory::Create:
			{
				doDelete(historyData);
			}
			break;
			case EHistory::Modify:
			{
				doModify(historyData, true);
			}
			break;
			case EHistory::Delete:
			{
				doCreate(historyData);
			}
			break;
			}

			CSceneController* sceneController = CSceneController::getInstance();
			sceneController->deselectAllOnHierachy(false);
			if (historySize - 2 >= 0)
			{
				// apply history selection
				sceneController->applySelected(m_history[historySize - 2]->Selected);
			}

			// move history to redo action
			m_redo.push_back(historyData);
			m_history.erase(m_history.begin() + (historySize - 1));

			// refresh ui on editor
			CEditor::getInstance()->refresh();

			m_enable = true;
		}

		void CSceneHistory::redo()
		{
			int historySize = (int)m_redo.size();
			if (historySize == 0)
				return;

			m_enable = false;

			CSceneController* sceneController = CSceneController::getInstance();
			CScene* scene = sceneController->getScene();

			// last history save
			SHistoryData* historyData = m_redo[historySize - 1];

			switch (historyData->History)
			{
			case EHistory::Create:
			{
				doCreate(historyData);
			}
			break;
			case EHistory::Modify:
			{
				doModify(historyData, false);
			}
			break;
			case EHistory::Delete:
			{
				doDelete(historyData);
			}
			break;
			}

			// apply history selection
			sceneController->deselectAllOnHierachy(false);
			sceneController->applySelected(historyData->Selected);

			// move history to redo action
			m_history.push_back(historyData);
			m_redo.erase(m_redo.begin() + (historySize - 1));

			// refresh ui on editor
			CEditor::getInstance()->refresh();

			m_enable = true;
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

		void CSceneHistory::saveToHistory(SGameObjectHistory* historyData, CGameObject* gameObject)
		{
			historyData->ObjectID = gameObject->getID();
			historyData->ObjectData = gameObject->createSerializable();

			CContainerObject* container = (CContainerObject*)gameObject->getParent();
			if (container != NULL)
			{
				historyData->ContainerID = container->getID();
				CGameObject* obj = container->getChildObjectBefore(gameObject);
				if (obj)
					historyData->BeforeID = obj->getID();
			}
			else
			{
				historyData->ContainerID = "_";
				CZone* zone = gameObject->getScene()->getZoneBefore((CZone*)gameObject);
				if (zone)
					historyData->BeforeID = zone->getID();
			}
		}

		void CSceneHistory::beginSaveHistory(CGameObject* gameObject)
		{
			if (!m_enable || !m_enableSelectHistory)
				return;

			const std::string& objectID = gameObject->getID();
			for (SGameObjectHistory* history : m_objects)
			{
				if (history->ObjectID == objectID)
				{
					delete history->ObjectData;
					saveToHistory(history, gameObject);
					return;
				}
			}

			SGameObjectHistory* objectData = new SGameObjectHistory();
			saveToHistory(objectData, gameObject);
			m_objects.push_back(objectData);
		}

		void CSceneHistory::removeSaveHistory(CGameObject* gameObject)
		{
			if (!m_enable || !m_enableSelectHistory)
				return;

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
			if (!m_enable)
				return;

			std::vector<CGameObject*> gameObjects;
			gameObjects.push_back(gameObject);
			saveCreateHistory(gameObjects);
		}

		void CSceneHistory::addData(CGameObject* gameObject,
			std::vector<std::string>& container,
			std::vector<std::string>& id,
			std::vector<std::string>& before,
			std::vector<CObjectSerializable*>& modifyData,
			std::vector<CObjectSerializable*>& objectData)
		{
			CObjectSerializable* currentData = gameObject->createSerializable();

			CContainerObject* parent = (CContainerObject*)gameObject->getParent();
			if (parent != NULL)
			{
				container.push_back(parent->getID());
				CGameObject* obj = parent->getChildObjectBefore(gameObject);
				if (obj)
					before.push_back(obj->getID());
				else
					before.push_back("");
			}
			else
			{
				container.push_back("_");
				CZone* zone = gameObject->getScene()->getZoneBefore((CZone*)gameObject);
				if (zone)
					before.push_back(zone->getID());
				else
					before.push_back("");
			}

			id.push_back(gameObject->getID());
			objectData.push_back(currentData);
			modifyData.push_back(NULL);

			beginSaveHistory(gameObject);

			// all childs
			CContainerObject* containerObject = dynamic_cast<CContainerObject*>(gameObject);
			if (containerObject)
			{
				ArrayGameObject* childs = containerObject->getChilds();
				for (CGameObject* go : *childs)
				{
					addData(go, container, id, before, modifyData, objectData);
				}
			}
		}

		void CSceneHistory::saveCreateHistory(std::vector<CGameObject*> gameObjects)
		{
			if (!m_enable)
				return;

			if (gameObjects.size() == 0)
				return;

			std::vector<std::string> container;
			std::vector<std::string> id;
			std::vector<std::string> before;
			std::vector<CObjectSerializable*> modifyData;
			std::vector<CObjectSerializable*> objectData;

			for (CGameObject* gameObject : gameObjects)
			{
				addData(gameObject, container, id, before, modifyData, objectData);
			}

			// save history
			addHistory(EHistory::Create, container, id, before, getSelected(), modifyData, objectData);
		}

		void CSceneHistory::saveDeleteHistory(CGameObject* gameObject)
		{
			if (!m_enable)
				return;

			std::vector<CGameObject*> gameObjects;
			gameObjects.push_back(gameObject);
			saveDeleteHistory(gameObjects);
		}

		void CSceneHistory::saveDeleteHistory(std::vector<CGameObject*> gameObjects)
		{
			if (!m_enable)
				return;

			if (gameObjects.size() == 0)
				return;

			std::vector<std::string> container;
			std::vector<std::string> id;
			std::vector<std::string> before;
			std::vector<CObjectSerializable*> modifyData;
			std::vector<CObjectSerializable*> objectData;

			for (CGameObject* gameObject : gameObjects)
			{
				addData(gameObject, container, id, before, modifyData, objectData);
			}

			// save history
			addHistory(EHistory::Delete, container, id, before, getSelected(), modifyData, objectData);
		}

		bool CSceneHistory::saveModifyHistory(std::vector<CGameObject*> gameObjects)
		{
			if (!m_enable)
				return false;

			if (gameObjects.size() == 0)
				return false;

			bool success = true;

			std::vector<std::string> container;
			std::vector<std::string> id;
			std::vector<std::string> before;
			std::vector<CObjectSerializable*> modifyData;
			std::vector<CObjectSerializable*> objectData;

			for (CGameObject* gameObject : gameObjects)
			{
				SGameObjectHistory* historyData = getObjectHistory(gameObject->getID());
				if (historyData == NULL)
				{
					os::Printer::log("[CSceneHistory::saveModifyHistory] failed, call CSceneHistory::beginSaveHistory first!");
					success = false;
					break;
				}

				CObjectSerializable* currentData = gameObject->createSerializable();

				id.push_back(gameObject->getID());
				container.push_back(historyData->ContainerID);
				before.push_back(historyData->BeforeID);
				objectData.push_back(historyData->ObjectData->clone());
				modifyData.push_back(currentData);
				historyData->changeData(currentData);
			}

			if (success)
			{
				addHistory(EHistory::Modify, container, id, before, getSelected(), modifyData, objectData);
			}
			else
			{
				for (CObjectSerializable* objData : modifyData)
					delete objData;
				clearRedo();
			}

			return success;
		}

		void CSceneHistory::saveStructureHistory(std::vector<CGameObject*> gameObjects)
		{
			if (!m_enable)
				return;

			if (gameObjects.size() == 0)
				return;

			std::vector<std::string> container;
			std::vector<std::string> id;
			std::vector<std::string> before;
			std::vector<SMoveCommand> moveCmd;

			for (CGameObject* gameObject : gameObjects)
			{
				SGameObjectHistory* historyData = getObjectHistory(gameObject->getID());
				if (historyData == NULL)
				{
					os::Printer::log("[CSceneHistory::saveStructureHistory] failed, call CSceneHistory::beginSaveHistory first!");
					break;
				}

				CObjectSerializable* currentData = gameObject->createSerializable();

				// old location
				id.push_back(gameObject->getID());
				container.push_back(historyData->ContainerID);
				before.push_back(historyData->BeforeID);

				// update new location
				saveToHistory(historyData, gameObject);

				SMoveCommand moveCmdData;
				moveCmdData.TargetContainer = historyData->ContainerID;
				moveCmdData.To = historyData->BeforeID;
				moveCmd.push_back(moveCmdData);
			}

			// addStrucureHistory(container, id, before, getSelected(), moveCmd);
		}

		void CSceneHistory::endSaveHistory()
		{
			if (!m_enable || !m_enableSelectHistory)
				return;
			freeCurrentObjectData();
		}
	}
}