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
#include "CGUIEditorHistory.h"
#include "Graphics2D/CGUIImporter.h"
#include "Editor/SpaceController/CGUIDesignController.h"
#include "Selection/CSelection.h"

namespace Skylicht
{
	namespace Editor
	{
		CGUIEditorHistory::CGUIEditorHistory(CCanvas* canvas) :
			m_canvas(canvas)
		{

		}

		CGUIEditorHistory::~CGUIEditorHistory()
		{
			freeCurrentObjectData();
		}

		void CGUIEditorHistory::doDelete(SHistoryData* historyData)
		{
			CGUIDesignController* controller = CGUIDesignController::getInstance();
			CGUIHierarchyController* hierarchyController = NULL;
			CCanvas* canvas = controller->getCanvas();

			size_t numObject = historyData->ObjectID.size();
			for (size_t i = 0; i < numObject; i++)
			{
				// object id
				std::string& id = historyData->ObjectID[i];

				// remove this object
				CGUIElement* element = canvas->getGUIByID(id.c_str());
				if (element)
					controller->removeGUIElement(element);
			}
		}

		void CGUIEditorHistory::doCreate(SHistoryData* historyData)
		{
			CGUIDesignController* controller = CGUIDesignController::getInstance();
			CGUIHierarchyController* hierarchyController = NULL;
			CCanvas* canvas = controller->getCanvas();

			if (controller->getSpaceHierarchy() != NULL)
				hierarchyController = controller->getSpaceHierarchy()->getController();

			size_t numObject = historyData->ObjectID.size();
			for (size_t i = 0; i < numObject; i++)
			{
				std::string& parentId = historyData->Container[i];
				if (parentId != "_")
				{
					CGUIElement* parent = canvas->getGUIByID(parentId.c_str());
					if (parent)
					{
						CGUIElement* ui = CGUIImporter::importGUI(canvas, parent, historyData->Data[i]);

						// update on GUI
						if (ui && hierarchyController)
						{
							CGUIHierachyNode* parentNode = hierarchyController->getNodeByObject(parent);
							if (parentNode)
								controller->createGUINode(parentNode, ui);
						}
					}
				}
			}
		}

		void CGUIEditorHistory::doModify(SHistoryData* historyData, bool undo)
		{
			CGUIDesignController* controller = CGUIDesignController::getInstance();
			CGUIHierarchyController* hierarchyController = NULL;
			CCanvas* canvas = controller->getCanvas();

			if (controller->getSpaceHierarchy() != NULL)
				hierarchyController = controller->getSpaceHierarchy()->getController();

			size_t numObject = historyData->DataModified.size();
			for (size_t i = 0; i < numObject; i++)
			{
				// object id
				std::string& id = historyData->ObjectID[i];

				// revert to old data
				CObjectSerializable* data = undo ? historyData->Data[i] : historyData->DataModified[i];

				// get object and undo data
				CGUIElement* element = canvas->getGUIByID(id.c_str());
				if (element != NULL)
					element->loadSerializable(data);

				controller->onHistoryModifyObject(element);

				// set current data for next action
				SGUIObjectHistory* objHistory = getObjectHistory(id);
				if (objHistory != NULL)
					objHistory->changeData(data);
			}
		}

		void CGUIEditorHistory::undo()
		{
			size_t historySize = m_history.size();
			if (historySize == 0)
				return;

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

			// move history to redo action
			m_redo.push_back(historyData);
			m_history.erase(m_history.begin() + (historySize - 1));

			// refresh ui on editor
			CEditor::getInstance()->refresh();
		}

		void CGUIEditorHistory::redo()
		{
			size_t historySize = m_redo.size();
			if (historySize == 0)
				return;

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

			// move history to redo action
			m_history.push_back(historyData);
			m_redo.erase(m_redo.begin() + (historySize - 1));

			// refresh ui on editor
			CEditor::getInstance()->refresh();
		}

		void CGUIEditorHistory::freeCurrentObjectData()
		{
			for (SGUIObjectHistory* history : m_objects)
			{
				delete history->ObjectData;
				delete history;
			}
			m_objects.clear();
		}

		SGUIObjectHistory* CGUIEditorHistory::getObjectHistory(const std::string& id)
		{
			for (SGUIObjectHistory* history : m_objects)
			{
				if (history->ObjectID == id)
					return history;
			}
			return NULL;
		}

		void CGUIEditorHistory::beginSaveHistory(CGUIElement* guiObject)
		{
			std::string objectID = guiObject->getID();

			for (SGUIObjectHistory* history : m_objects)
			{
				// no need
				if (history->ObjectID == objectID)
					return;
			}

			SGUIObjectHistory* objectData = new SGUIObjectHistory();
			objectData->ObjectID = objectID;
			objectData->ObjectData = guiObject->createSerializable();
			m_objects.push_back(objectData);
		}

		void CGUIEditorHistory::removeSaveHistory(CGUIElement* guiObject)
		{
			std::string objectID = guiObject->getID();

			std::vector<SGUIObjectHistory*>::iterator i = m_objects.begin(), end = m_objects.end();
			while (i != end)
			{
				SGUIObjectHistory* history = (*i);
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

		void CGUIEditorHistory::saveCreateHistory(CGUIElement* guiObject)
		{
			std::vector<CGUIElement*> guiObjects;
			guiObjects.push_back(guiObject);
			saveCreateHistory(guiObjects);
		}

		void CGUIEditorHistory::saveCreateHistory(std::vector<CGUIElement*> guiObjects)
		{
			if (guiObjects.size() == 0)
				return;

			std::vector<std::string> container;
			std::vector<std::string> id;
			std::vector<CObjectSerializable*> modifyData;
			std::vector<CObjectSerializable*> objectData;

			for (CGUIElement* guiObject : guiObjects)
			{
				CObjectSerializable* currentData = guiObject->createSerializable();

				// parent container id
				if (guiObject->getParent() != NULL)
				{
					container.push_back(guiObject->getParent()->getID());
				}
				else
				{
					// this is zone (no parent)
					container.push_back("_");
				}

				// game object id
				id.push_back(guiObject->getID());

				// last data object
				objectData.push_back(currentData);

				// current data object
				modifyData.push_back(NULL);
			}

			// save history
			addHistory(EHistory::Create, container, id, getSelected(), modifyData, objectData);
		}

		void CGUIEditorHistory::saveDeleteHistory(CGUIElement* guiObject)
		{
			std::vector<CGUIElement*> guiObjects;

			guiObjects.push_back(guiObject);
			guiObject->getAllChilds(guiObjects);

			saveDeleteHistory(guiObjects);
		}

		void CGUIEditorHistory::saveDeleteHistory(std::vector<CGUIElement*> guiObjects)
		{
			if (guiObjects.size() == 0)
				return;

			std::vector<std::string> container;
			std::vector<std::string> id;
			std::vector<CObjectSerializable*> modifyData;
			std::vector<CObjectSerializable*> objectData;

			for (CGUIElement* guiObject : guiObjects)
			{
				CObjectSerializable* currentData = guiObject->createSerializable();

				// parent container id
				if (guiObject->getParent() != NULL)
				{
					container.push_back(guiObject->getParent()->getID());
				}
				else
				{
					// this is zone (no parent)
					container.push_back("_");
				}

				// game object id
				id.push_back(guiObject->getID());

				// last data object
				objectData.push_back(currentData);

				// current data object
				modifyData.push_back(NULL);
			}

			// save history
			addHistory(EHistory::Delete, container, id, getSelected(), modifyData, objectData);
		}

		bool CGUIEditorHistory::saveModifyHistory(CGUIElement* guiObject)
		{
			std::vector<CGUIElement*> guiObjects;
			guiObjects.push_back(guiObject);
			return saveModifyHistory(guiObjects);
		}

		bool CGUIEditorHistory::saveModifyHistory(std::vector<CGUIElement*> guiObjects)
		{
			if (guiObjects.size() == 0)
				return false;

			bool success = true;

			std::vector<std::string> container;
			std::vector<std::string> id;
			std::vector<CObjectSerializable*> modifyData;
			std::vector<CObjectSerializable*> objectData;

			for (CGUIElement* guiObject : guiObjects)
			{
				SGUIObjectHistory* historyData = getObjectHistory(guiObject->getID());
				if (historyData == NULL)
				{
					os::Printer::log("[CGUIEditorHistory::saveHistory] failed, call CGUIEditorHistory::beginSaveHistory first!");
					success = false;
					break;
				}

				CObjectSerializable* currentData = guiObject->createSerializable();

				// parent container id
				if (guiObject->getParent() != NULL)
				{
					container.push_back(guiObject->getParent()->getID());
				}
				else
				{
					// this is zone (no parent)
					container.push_back("_");
				}

				// game object id
				id.push_back(guiObject->getID());

				// last data object
				objectData.push_back(historyData->ObjectData->clone());

				// current data object
				modifyData.push_back(currentData);

				// change save point
				historyData->changeData(currentData);
			}

			if (success)
			{
				addHistory(EHistory::Modify, container, id, getSelected(), modifyData, objectData);
			}
			else
			{
				for (CObjectSerializable* objData : modifyData)
					delete objData;
			}

			return success;
		}

		void CGUIEditorHistory::endSaveHistory()
		{
			freeCurrentObjectData();
		}
	}
}