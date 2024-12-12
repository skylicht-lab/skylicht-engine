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
#include "CGraphEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/Space/Hierarchy/CHierarchyController.h"
#include "Editor/SpaceController/CSceneController.h"
#include "Selection/CSelection.h"

#ifdef BUILD_SKYLICHT_GRAPH

using namespace Graph;

namespace Skylicht
{
	namespace Editor
	{
		EDITOR_REGISTER(CGraphEditor, CGraphComponent);

		CGraphEditor::CGraphEditor()
		{

		}

		CGraphEditor::~CGraphEditor()
		{

		}

		void CGraphEditor::initCustomDataGUI(CObjectSerializable* obj, CValueProperty* data, GUI::CBoxLayout* layout, CSpaceProperty* ui)
		{
			if (data->Name == "agentMaxSlope")
			{
				layout->addSpace(5.0f);
				ui->addButton(layout, L"Build RecastMesh")->OnPress = [&](GUI::CBase* button)
					{
						CGraphComponent* graph = (CGraphComponent*)m_component;
						graph->buildRecastMesh();
					};
				ui->addButton(layout, L"Save RecastMesh")->OnPress = [&](GUI::CBase* button)
					{
						CSceneController* controller = CSceneController::getInstance();
						std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();

						GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(
							GUI::Context::getRoot(),
							GUI::COpenSaveDialog::Save,
							assetFolder.c_str(),
							assetFolder.c_str(),
							"obj;*"
						);

						dialog->OnSave = [&, controller](std::string path)
							{
								CGraphComponent* graph = (CGraphComponent*)m_component;
								graph->buildRecastMesh();

								CEntityPrefab prefab;
								CEntity* entity = prefab.createEntity();

								prefab.addTransformData(entity, NULL, core::IdentityMatrix, "NavMesh");
								CRenderMeshData* renderMesh = entity->addData<CRenderMeshData>();
								renderMesh->setMesh(graph->getNavMesh());

								CMeshManager::getInstance()->exportModel(prefab.getEntities(), prefab.getNumEntities(), path.c_str());

								CEditor::getInstance()->refresh();
							};
					};
				layout->addSpace(5.0f);
			}
			else if (data->Name == "recastMesh")
			{
				layout->addSpace(5.0f);
				ui->addButton(layout, L"Load RecastMesh")->OnPress = [&](GUI::CBase* button)
					{
						CGraphComponent* graph = (CGraphComponent*)m_component;
						graph->loadRecastMesh();
					};
				ui->addButton(layout, L"Build WalkingTilemap")->OnPress = [&](GUI::CBase* button)
					{
						CGraphComponent* graph = (CGraphComponent*)m_component;
						CEditor::getInstance()->initBuildWalkMap(graph);
					};
				layout->addSpace(5.0f);
				ui->addButton(layout, L"Save WalkingTilemap")->OnPress = [&](GUI::CBase* button)
					{
						CSceneController* controller = CSceneController::getInstance();
						std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();

						GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(
							GUI::Context::getRoot(),
							GUI::COpenSaveDialog::Save,
							assetFolder.c_str(),
							assetFolder.c_str(),
							"wtm;*"
						);

						dialog->OnSave = [&, controller](std::string path)
							{
								CGraphComponent* graph = (CGraphComponent*)m_component;
								graph->saveWalkMap(path.c_str());
								CEditor::getInstance()->refresh();
							};
					};
				layout->addSpace(5.0f);
			}
			else if (data->Name == "walkingTileMap")
			{
				ui->addButton(layout, L"Load WalkingTilemap")->OnPress = [&](GUI::CBase* button)
					{
						CGraphComponent* graph = (CGraphComponent*)m_component;
						graph->loadWalkMap();
					};
				layout->addSpace(10.0f);
				ui->addButton(layout, L"Release all")->OnPress = [&](GUI::CBase* button)
					{
						CGraphComponent* graph = (CGraphComponent*)m_component;
						graph->release();
					};
				layout->addSpace(5.0f);
			}
		}

		void CGraphEditor::update()
		{

		}
	}
}

#endif