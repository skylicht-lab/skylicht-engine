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
#include "GUI/GUI.h"
#include "CMeshEditor.h"
#include "Activator/CEditorActivator.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/SpaceController/CSceneController.h"

#include "MeshManager/CMeshManager.h"
#include "Material/CMaterialManager.h"
#include "Editor/CEditor.h"

#include "AssetManager/CAssetImporter.h"

namespace Skylicht
{
	namespace Editor
	{
		CMeshEditor::CMeshEditor() :
			m_settings(NULL)
		{

		}

		CMeshEditor::~CMeshEditor()
		{
			closeGUI();
		}

		void CMeshEditor::closeGUI()
		{
			CAssetEditor::closeGUI();
			if (m_settings)
			{
				delete m_settings;
				m_settings = NULL;
			}
		}

		void CMeshEditor::initGUI(const char* path, CSpaceProperty* ui)
		{
			std::string meta = path;
			meta += ".meta";
			m_settings = createGetMeshExportSetting(meta.c_str());

			GUI::CCollapsibleGroup* group = ui->addGroup("Mesh Exporter", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);
			serializableToControl(m_settings, ui, layout);

			layout->addSpace(5.0f);

			std::string reloadPath = path;
			ui->addButton(layout, L"Reload mesh in scene")->OnPress = [&, p = reloadPath](GUI::CBase* button)
				{
					CMeshManager* meshMgr = CMeshManager::getInstance();
					if (meshMgr->isMeshLoaded(p.c_str()))
					{
						// unload mesh & reload
						meshMgr->releaseResource(p.c_str());
						CSceneController::getInstance()->doMeshChange(p.c_str());
					}
				};

			group->setExpand(true);

			group = ui->addGroup("Exporter", this);
			layout = ui->createBoxLayout(group);
			std::string daePath = path;
			ui->addButton(layout, L"Export Mesh")->OnPress = [&, p = daePath](GUI::CBase* button)
				{
					std::string outout = CPath::replaceFileExt(p, ".smesh");

					CEntityPrefab* prefab = CMeshManager::getInstance()->loadModel(
						p.c_str(),
						m_settings->TextureFolder.getString(),
						true,
						m_settings->FlipNormal.get(),
						m_settings->UseUV2.get());

					if (prefab != NULL)
					{
						CMeshManager::getInstance()->exportModel(prefab->getEntities(), prefab->getNumEntities(), outout.c_str());

						CAssetImporter importer;
						importer.add(outout.c_str());
						importer.importAll();

						CEditor::getInstance()->refresh();
					}
				};
			layout->addSpace(5.0f);
			ui->addButton(layout, L"Export Material")->OnPress = [&, p = daePath](GUI::CBase* button)
				{
					std::string outout = CPath::replaceFileExt(p, ".mat");
					std::string textureFolder = m_settings->TextureFolder.getString();

					CEntityPrefab* prefab = CMeshManager::getInstance()->loadModel(
						p.c_str(),
						m_settings->TextureFolder.getString(),
						true,
						m_settings->FlipNormal.get(),
						false);

					if (prefab != NULL)
					{
						CMaterialManager::getInstance()->exportMaterial(prefab, outout.c_str());
						CMeshManager::getInstance()->releasePrefab(prefab);

						CAssetImporter importer;
						importer.add(outout.c_str());
						importer.importAll();

						CEditor::getInstance()->refresh();
					}
				};

			group->setExpand(true);
		}

		CMeshExportSettings* CMeshEditor::createGetMeshExportSetting(const char* path)
		{
			CMeshExportSettings* setting = new CMeshExportSettings();
			if (!setting->load(path))
				setting->save(path);
			return setting;
		}

		void CMeshEditor::onUpdateValue(CObjectSerializable* object)
		{
			m_settings->saveToFile();
		}

		ASSET_EDITOR_REGISTER(CDaeMeshEditor, dae);

		CDaeMeshEditor::CDaeMeshEditor()
		{

		}

		ASSET_EDITOR_REGISTER(CObjMeshEditor, obj);

		CObjMeshEditor::CObjMeshEditor()
		{

		}

		ASSET_EDITOR_REGISTER(CFbxMeshEditor, fbx);

		CFbxMeshEditor::CFbxMeshEditor()
		{

		}

	}
}