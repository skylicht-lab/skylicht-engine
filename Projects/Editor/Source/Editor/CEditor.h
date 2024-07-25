/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "GUI/GUI.h"
#include "Space/CSpace.h"
#include "AssetManager/CAssetWatcher.h"
#include "Graphics2D/SpriteFrame/CSpriteAtlas.h"
#include "Utils/CSingleton.h"

namespace Skylicht
{
	namespace Editor
	{
		class CEditor
		{
			friend class CSpace;
		public:
			DECLARE_SINGLETON(CEditor)

		private:
			GUI::CCanvas* m_canvas;
			GUI::CMenuBar* m_menuBar;
			GUI::CDockPanel* m_dockPanel;

			GUI::CTableRow* m_statusInfo;
			GUI::CIconTextItem* m_status;

			GUI::CDialogWindow* m_importDialog;
			GUI::CDialogWindow* m_loadSceneDialog;
			GUI::CDialogWindow* m_exportGMapDialog;
			GUI::CDialogWindow* m_exportSpriteDialog;

			std::list<CSpace*> m_workspaces;

			std::list<GUI::CMenuItem*> m_menuWindowItems;

			CAssetWatcher* m_assetWatcher;

			CSpriteAtlas* m_spriteIcon;

			bool m_uiInitiate;

			bool m_confirmQuit;
		public:
			CEditor();

			virtual ~CEditor();

			void update();

			void initImportGUI(bool fromWatcher = false);

			void initLoadSceneGUI(const char* path);

			void initEditorGUI();

			bool isUIInitiate()
			{
				return m_uiInitiate;
			}

			bool isImportFinish();

			void closeImportDialog();

			void closeLoadSceneDialog();

			void closeExportGMapDialog();

			void closeExportSpriteDialog();

			void closeProperty();

			void saveLayout(const std::string& data);

			void pause();

			void resume();

			void refresh();

			bool onClose();

			bool needReImport();

		public:

			CSpriteAtlas* getSpriteIcon()
			{
				return m_spriteIcon;
			}

			GUI::CCanvas* getRootCanvas()
			{
				return m_canvas;
			}

		public:

			CSpace* getWorkspace(GUI::CWindow* window);

			CSpace* getWorkspaceByName(const std::wstring& name);

			CSpace* openWorkspace(const std::wstring& name);

		protected:

			void initMenuBar();

			void initDefaultLayout();

			void initSessionLayout(const std::string& data);

			CSpace* initWorkspace(GUI::CWindow* window, const std::wstring& workspace);

			void removeWorkspace(CSpace* space);

		protected:

			void readDockLayout(io::IXMLReader* xml, GUI::CDockPanel* panel);

			void readSpliterLayout(io::IXMLReader* xml, GUI::CDockPanel* panel, GUI::CSplitter* spliter, bool isHorizontal);

			void readDockTab(io::IXMLReader* xml, GUI::CDockTabControl* tabcontrol);

			void readBound(io::IXMLReader* xml, GUI::CBase* base);

		protected:

			void OnMenuQuit(GUI::CBase* item);

			void OnMenuSave(GUI::CBase* item);

			void OnCommandLogo(GUI::CBase* item);

			void OnOpenMenuFile(GUI::CBase* item);

			void OnCommandFile(GUI::CBase* item);

			void OnCommandEdit(GUI::CBase* item);

			void OnCommandAssetCreate(GUI::CBase* item);

			void OnCommandWindowOpen(GUI::CBase* item);

			void OnCommandWindow(GUI::CBase* item);

			void OnCommandGameObject(GUI::CBase* item);

			void setCheckIcon(GUI::CMenuItem* item);

			void closeOpenWorkspace(GUI::CMenuItem* item);

		public:

			void onSaveScene();
			
			void onSaveGUICanvas();
			
			void showProjectSetting();

			void showGoogleMap();

			void exportGMap(const char* path, long x1, long y1, long x2, long y2, int zoom, int type, int gridSize);

			void exportSprite(const char* id, const char* path, const std::vector<std::string>& pngs, int width, int height, bool alpha);
		};
	}
}
