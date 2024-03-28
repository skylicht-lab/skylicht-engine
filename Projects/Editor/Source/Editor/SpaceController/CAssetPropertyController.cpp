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
#include "CAssetPropertyController.h"
#include "Activator/CEditorActivator.h"
#include "Reactive/CObserver.h"
#include "AssetManager/CAssetManager.h"
#include "Utils/CPath.h"
#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CAssetPropertyController);

		CAssetPropertyController::CAssetPropertyController()
		{

		}

		CAssetPropertyController::~CAssetPropertyController()
		{

		}

		void CAssetPropertyController::onSelectAsset(const char* path, bool isFolder)
		{
			CSpaceProperty* spaceProperty = (CSpaceProperty*)CEditor::getInstance()->getWorkspaceByName(L"Property");

			if (spaceProperty == NULL || isFolder)
				return;

			// Name and icon			
			std::string fileName = CPath::getFileName(path);
			std::string ext = CPath::getFileNameExt(path);

			GUI::CButton* btn = spaceProperty->setButtonLabel(CStringImp::convertUTF8ToUnicode(fileName.c_str()).c_str());
			btn->setIcon(GUI::ESystemIcon::File);

			std::string assetPath = CAssetManager::getInstance()->getShortPath(path);
			btn->OnPress = [&, p = assetPath](GUI::CBase* base) {
				browseAsset(p.c_str());
				};

			// Clear old ui
			spaceProperty->clearAllGroup();

			// Tabable
			spaceProperty->getWindow()->getCanvas()->TabableGroup.clear();

			// Get file ext editor
			if (!isFolder)
			{
				CAssetEditor* assetEditor = CEditorActivator::getInstance()->getAssetEditorInstance(ext.c_str());
				if (assetEditor != NULL)
					spaceProperty->addAsset(assetEditor, assetPath.c_str());
			}

			spaceProperty->getWindow()->forceUpdateLayout();
		}

		void CAssetPropertyController::browseAsset(const char* path)
		{
			CSpaceAssets* spaceAssets = (CSpaceAssets*)CEditor::getInstance()->getWorkspaceByName(L"Assets");

			if (spaceAssets == NULL)
				return;

			std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();
			std::string fullPath = assetFolder + "/" + path;

			std::string folder = CPath::getFolderPath(fullPath);

			GUI::CTreeNode* node = spaceAssets->getTreeController()->expand(folder);
			if (node != NULL)
				spaceAssets->getTreeController()->selectNode(node);

			spaceAssets->getListController()->scrollAndSelectPath(fullPath.c_str());
			spaceAssets->getWindow()->forceUpdateLayout();
		}
	}
}