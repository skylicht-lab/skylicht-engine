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
#include "CSceneController.h"

#include "Reactive/CObserver.h"
#include "AssetManager/CAssetManager.h"
#include "AssetManager/CAssetImporter.h"
#include "Editor/Space/Assets/CSpaceAssets.h"
#include "Utils/CPath.h"
#include "Editor/CEditor.h"

#include "Scene/CSceneExporter.h"
#include "ResourceSettings/SpriteExportSettings.h"
#include "Graphics2D/CGUIExporter.h"
#include "Graphics2D/SpriteFrame/CFontSource.h"

namespace Skylicht
{
	namespace Editor
	{
		IMPLEMENT_SINGLETON(CAssetCreateController);

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

			std::string fullPath = assetMgr->generateAssetPath("/Material%02d.mat", currentFolder.c_str());

			ArrayMaterial materials;

			CMaterial* newMaterial = new CMaterial("NoName", "BuiltIn/Shader/Basic/TextureColor.xml");
			materials.push_back(newMaterial);
			CMaterialManager::getInstance()->saveMaterial(materials, fullPath.c_str());

			importAndSelect(fullPath.c_str());
		}

		void CAssetCreateController::createEmptyScene()
		{
			// create a scene
			CScene* scene = new CScene();

			// create a zone in scene
			CZone* zone = scene->createZone();

			// lighting
			CGameObject* lightObj = zone->createEmptyObject();
			lightObj->setName(L"DirectionLight");

			CTransformEuler* lightTransform = lightObj->getTransformEuler();
			lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

			core::vector3df direction = core::vector3df(0.0f, -1.5f, -2.0f);
			lightTransform->setOrientation(direction, Transform::Oy);

			CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();
			SColor c(255, 255, 244, 214);
			directionalLight->setColor(SColorf(c));

			// update search index
			scene->updateAddRemoveObject();
			scene->updateIndexSearchObject();
			scene->update();

			CSpaceAssets* spaceAssets = (CSpaceAssets*)CEditor::getInstance()->getWorkspaceByName(L"Assets");

			CAssetManager* assetMgr = CAssetManager::getInstance();

			std::string currentFolder = assetMgr->getAssetFolder();
			if (spaceAssets != NULL)
				currentFolder = spaceAssets->getListController()->getCurrentFolder();

			std::string fullPath = assetMgr->generateAssetPath("/Scene%02d.scene", currentFolder.c_str());

			CSceneExporter::exportScene(scene, fullPath.c_str());

			delete scene;

			importAndSelect(fullPath.c_str());
		}

		void CAssetCreateController::createEmptySprite()
		{
			CSpaceAssets* spaceAssets = (CSpaceAssets*)CEditor::getInstance()->getWorkspaceByName(L"Assets");

			CAssetManager* assetMgr = CAssetManager::getInstance();

			std::string currentFolder = assetMgr->getAssetFolder();
			if (spaceAssets != NULL)
				currentFolder = spaceAssets->getListController()->getCurrentFolder();

			std::string fullPath = assetMgr->generateAssetPath("/Sprite%02d.sprite", currentFolder.c_str());

			SpriteExportSettings nullSprite;
			nullSprite.save(fullPath.c_str());

			importAndSelect(fullPath.c_str());
		}

		void CAssetCreateController::createEmptyFont()
		{
			CSpaceAssets* spaceAssets = (CSpaceAssets*)CEditor::getInstance()->getWorkspaceByName(L"Assets");

			CAssetManager* assetMgr = CAssetManager::getInstance();

			std::string currentFolder = assetMgr->getAssetFolder();
			if (spaceAssets != NULL)
				currentFolder = spaceAssets->getListController()->getCurrentFolder();

			std::string fullPath = assetMgr->generateAssetPath("/Font%02d.font", currentFolder.c_str());

			CFontSource nullFont;
			nullFont.save(fullPath.c_str());

			importAndSelect(fullPath.c_str());
		}

		void CAssetCreateController::createEmptyGUI()
		{
			CSpaceAssets* spaceAssets = (CSpaceAssets*)CEditor::getInstance()->getWorkspaceByName(L"Assets");

			CAssetManager* assetMgr = CAssetManager::getInstance();

			std::string currentFolder = assetMgr->getAssetFolder();
			if (spaceAssets != NULL)
				currentFolder = spaceAssets->getListController()->getCurrentFolder();

			std::string fullPath = assetMgr->generateAssetPath("/GUI%02d.gui", currentFolder.c_str());

			CZone* zone = CSceneController::getInstance()->getScene()->getZone(0);
			CGameObject* canvas = zone->createEmptyObject();
			CCanvas* nullCanvas = canvas->addComponent<CCanvas>();

			CGUIExporter::save(fullPath.c_str(), nullCanvas);

			importAndSelect(fullPath.c_str());

			canvas->remove();
		}

		void CAssetCreateController::importAndSelect(const char* path)
		{
			CAssetImporter importer;
			importer.add(path);
			importer.importAll();

			CEditor::getInstance()->refresh();

			std::string shortPath = CAssetManager::getInstance()->getShortPath(path);
			CAssetPropertyController::getInstance()->browseAsset(shortPath.c_str());
		}
	}
}