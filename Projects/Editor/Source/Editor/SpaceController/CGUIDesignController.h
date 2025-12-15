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

#pragma once

#include "GUI/GUIContext.h"

#include "Utils/CSingleton.h"
#include "AssetManager/CAssetManager.h"
#include "Reactive/IObserver.h"
#include "Editor/Space/GUIHierarchy/CGUIHierachyNode.h"
#include "Editor/Space/GUIHierarchy/CSpaceGUIHierarchy.h"
#include "Editor/Space/GUIDesign/CSpaceGUIDesign.h"
#include "CContextMenuGUIElement.h"
#include "History/CGUIEditorHistory.h"

namespace Skylicht
{
	namespace Editor
	{
		class CGUIDesignController :
			public IFileLoader,
			public IObserver
		{
		public:
			DECLARE_SINGLETON(CGUIDesignController)

		protected:
			CScene* m_scene;
			GUI::CCanvas* m_canvas;
			Skylicht::CCanvas* m_guiCanvas;

			CCamera* m_guiCamera;

			CSpaceGUIHierarchy* m_spaceHierarchy;
			CSpaceGUIDesign* m_spaceDesign;

			CContextMenuGUIElement* m_contextMenu;

			CGUIHierachyNode* m_rootNode;
			std::string m_guiFilePath;

			CGUIEditorHistory* m_history;

			std::string m_localizePath;

			bool m_enableLocalize;

		public:
			CGUIDesignController();

			virtual ~CGUIDesignController();

			void setScene(CScene* scene);

			void deleteScene();

			inline CScene* getScene()
			{
				return m_scene;
			}

			inline CCamera* getCamera()
			{
				return m_guiCamera;
			}

			inline void setSpaceHierarchy(CSpaceGUIHierarchy* space)
			{
				m_spaceHierarchy = space;
			}

			inline CSpaceGUIHierarchy* getSpaceHierarchy()
			{
				return m_spaceHierarchy;
			}

			inline void setSpaceDesign(CSpaceGUIDesign* space)
			{
				m_spaceDesign = space;
			}

			inline CSpaceGUIDesign* getSpaceDesign()
			{
				return m_spaceDesign;
			}

			inline CGUIHierachyNode* getRootNode()
			{
				return m_rootNode;
			}

			void initContextMenu(GUI::CCanvas* canvas);

			void initHistory();

			inline Skylicht::CCanvas* getCanvas()
			{
				return m_guiCanvas;
			}

			inline const std::string& getSaveGUIPath()
			{
				return m_guiFilePath;
			}

			inline CGUIEditorHistory* getHistory()
			{
				return m_history;
			}

			void newGUI();

			bool needSave();

			void save(const char* path);

			virtual void loadFile(const std::string& path);

			void rebuildGUIHierachy();

			void onContextMenu(CGUIHierachyNode* node);

			void createGUINode(CGUIHierachyNode* parent, const std::wstring& command);

			void createGUINode(CGUIHierachyNode* parent, CGUIElement* node);

			void deselectAllOnHierachy(bool callEvent = true);

			void removeGUIElement(CGUIElement* node);

			void onHistoryModifyObject(CGUIElement* node);

			void onDelete();

			void refresh();

			CGUIHierachyNode* deselectOnHierachy(CGUIElement* element);

			CGUIHierachyNode* selectOnHierachy(CGUIElement* element);

			void onCopy();

			void onPaste();

			void onDuplicate();

			void onCut();

			void onUndo();

			void onRedo();

			void onMoveStructure(CGUIElement* element, CGUIElement* parent, CGUIElement* before);

			void syncGUID();

			std::string getSpritePath(const char* framePath);

			void applySelected(std::vector<CSelectObject*> ids);

			CGUIHierachyNode* selectOnHierachy(CGUIElement* gui, bool callEvent);

			void doSpriteChange(const char* resource);

			void doReplaceTexture(ITexture* oldTexture, ITexture* newTexture);

			void initLocalizePath(const char* path);

			void setLanguage(u32 id);

			void enableLocalize(bool b);

			inline bool isEnableLocalize()
			{
				return m_enableLocalize;
			}

			const std::string& getLocalizePath()
			{
				return m_localizePath;
			}

		protected:

			void updateAllTexts();

			void rebuildGUIHierachy(CGUIElement* parent, CGUIHierachyNode* parentNode);

			void setNodeEvent(CGUIHierachyNode* node);

			void onUpdateNode(CGUIHierachyNode* node);

			void onSelectNode(CGUIHierachyNode* node, bool selected);

			virtual void onNotify(ISubject* subject, IObserver* from);
		};
	}
}
