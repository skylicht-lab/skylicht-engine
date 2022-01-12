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

#include "CAssetCreateController.h"
#include "CAssetPropertyController.h"

#include "Reactive/CObserver.h"
#include "AssetManager/CAssetManager.h"
#include "AssetManager/CAssetImporter.h"
#include "Editor/Space/Assets/CSpaceAssets.h"
#include "Utils/CPath.h"
#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		CAssetCreateController::CAssetCreateController()
		{

		}

		CAssetCreateController::~CAssetCreateController()
		{

		}

		void CAssetCreateController::createEmptyMaterial()
		{
			CSpaceAssets* spaceAssets = (CSpaceAssets*)CEditor::getInstance()->getWorkspaceByName(L"Assets");

			CAssetManager* assetMgr = CAssetManager::getInstance();

			std::string currentFolder = assetMgr->getAssetFolder();
			if (spaceAssets != NULL)
				currentFolder = spaceAssets->getListController()->getCurrentFolder();

			int i = 1;
			bool exists = false;
			char name[256];
			std::string fullPath;

			do
			{
				sprintf(name, "/Material%02d.mat", i);
				fullPath = currentFolder + name;
				exists = assetMgr->isExist(fullPath.c_str());
				i++;
			} while (exists);

			ArrayMaterial materials;
			CMaterial* newMaterial = new CMaterial("NoName", "BuiltIn/Shader/Basic/TextureColor.xml");
			materials.push_back(newMaterial);

			CMaterialManager::getInstance()->saveMaterial(materials, fullPath.c_str());

			delete newMaterial;

			CAssetImporter importer;
			importer.add(fullPath.c_str());
			importer.importAll();

			CEditor::getInstance()->refresh();

			std::string shortPath = assetMgr->getShortPath(fullPath.c_str());
			CAssetPropertyController::getInstance()->browseAsset(shortPath.c_str());
		}
	}
}