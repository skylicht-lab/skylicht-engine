/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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

#pragma once

#include "CHistory.h"
#include "Graphics2D/CCanvas.h"

namespace Skylicht
{
	namespace Editor
	{
		struct SGUIObjectHistory
		{
			std::string ObjectID;
			CObjectSerializable* ObjectData;

			std::string ContainerID;
			std::string BeforeID;

			void changeData(CObjectSerializable* data)
			{
				delete ObjectData;
				ObjectData = data->clone();
			}
		};

		class CGUIEditorHistory : public CHistory
		{
		protected:
			CCanvas* m_canvas;

			std::vector<SGUIObjectHistory*> m_objects;

		public:
			CGUIEditorHistory(CCanvas* canvas);

			virtual ~CGUIEditorHistory();

			virtual void undo();

			virtual void redo();

			void beginSaveHistory(CGUIElement* guiObject);

			void removeSaveHistory(CGUIElement* guiObject);

			void saveCreateHistory(CGUIElement* guiObject);

			void saveCreateHistory(std::vector<CGUIElement*> guiObjects);

			void saveDeleteHistory(CGUIElement* guiObject);

			void saveDeleteHistory(std::vector<CGUIElement*> guiObjects);

			bool saveModifyHistory(CGUIElement* guiObject);

			bool saveModifyHistory(std::vector<CGUIElement*> guiObjects);

			void saveStructureHistory(std::vector<CGUIElement*> guiObjects);

			void endSaveHistory();

		protected:

			void addData(CGUIElement* guiObject,
				std::vector<std::string>& container,
				std::vector<std::string>& id,
				std::vector<std::string>& before,
				std::vector<CObjectSerializable*>& modifyData,
				std::vector<CObjectSerializable*>& objectData);

			void saveHistory(SGUIObjectHistory* historyData, CGUIElement* guiObject);

			void doDelete(SHistoryData* historyData);

			void doCreate(SHistoryData* historyData);

			void doModify(SHistoryData* historyData, bool undo);

			void doStructure(SHistoryData* historyData, bool undo);

			void freeCurrentObjectData();

			SGUIObjectHistory* getObjectHistory(const std::string& id);
		};
	}
}