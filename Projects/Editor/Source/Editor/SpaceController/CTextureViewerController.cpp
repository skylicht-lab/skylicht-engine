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

#include "pch.h"
#include "Editor/CEditor.h"
#include "Editor/Space/TextureViewer/CSpaceTextureViewer.h"
#include "GUI/Utils/CDragAndDrop.h"
#include "CTextureViewerController.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CTextureViewerController);

		CTextureViewerController::CTextureViewerController()
		{
			CAssetManager* assetMgr = CAssetManager::getInstance();
			assetMgr->registerFileLoader("png", this);
			assetMgr->registerFileLoader("tga", this);
			assetMgr->registerFileLoader("jpg", this);
			assetMgr->registerFileLoader("jpeg", this);
			assetMgr->registerFileLoader("dds", this);
		}

		CTextureViewerController::~CTextureViewerController()
		{
			CAssetManager* assetMgr = CAssetManager::getInstance();
			assetMgr->unRegisterFileLoader("png", this);
			assetMgr->unRegisterFileLoader("tga", this);
			assetMgr->unRegisterFileLoader("jpg", this);
			assetMgr->unRegisterFileLoader("jpeg", this);
			assetMgr->unRegisterFileLoader("dds", this);
		}

		void CTextureViewerController::loadFile(const std::string& path)
		{
			CEditor* editor = CEditor::getInstance();

			CSpace* space = editor->getWorkspaceByName(std::wstring(L"Texture Viewer"));
			if (!space)
				space = editor->openWorkspace(std::wstring(L"Texture Viewer"));
			else
			{
				GUI::CDockableWindow* dockWindow = dynamic_cast<GUI::CDockableWindow*>(space->getWindow());
				if (dockWindow != NULL)
				{
					GUI::CDockTabControl* dockTab = dockWindow->getCurrentDockTab();
					if (dockTab != NULL)
					{
						// activate
						dockTab->setCurrentWindow(dockWindow);
						GUI::CDragAndDrop::cancel();
					}
				}
			}

			if (space)
			{
				CSpaceTextureViewer* spaceTV = dynamic_cast<CSpaceTextureViewer*>(space);
				spaceTV->openTexture(path.c_str());
			}
		}
	}
}