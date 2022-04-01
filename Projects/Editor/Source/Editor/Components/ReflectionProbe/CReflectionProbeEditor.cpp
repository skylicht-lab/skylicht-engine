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
#include "CReflectionProbeEditor.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/Space/Hierarchy/CHierarchyController.h"
#include "Editor/SpaceController/CSceneController.h"
#include "Selection/CSelection.h"
#include "Lightmapper/CLightmapper.h"

namespace Skylicht
{
	namespace Editor
	{
		EDITOR_REGISTER(CReflectionProbeEditor, CReflectionProbe);

		CReflectionProbeEditor::CReflectionProbeEditor()
		{

		}

		CReflectionProbeEditor::~CReflectionProbeEditor()
		{

		}

		void CReflectionProbeEditor::initCustomGUI(GUI::CBoxLayout* layout, CSpaceProperty* ui)
		{
			layout->addSpace(5.0f);

			ui->addLabel(layout, L"Bake Function", GUI::TextLeft);

			ui->addButton(layout, L"Bake Reflection")->OnPress = [&](GUI::CBase* button)
			{
				CReflectionProbe* reflectionProbe = (CReflectionProbe*)m_component;
				CSceneController* controller = CSceneController::getInstance();
				CScene* scene = controller->getScene();
				CZone* zone = controller->getZone();

				IRenderPipeline* renderPipeline = controller->getRenderPipeLine();
				if (renderPipeline == NULL)
					return;

				CGameObject* bakeCameraObj = zone->createEmptyObject();
				CCamera* bakeCamera = bakeCameraObj->addComponent<CCamera>();
				scene->updateAddRemoveObject();

				CEntityManager* entityMgr = scene->getEntityManager();
				entityMgr->update();

				reflectionProbe->bakeProbe(
					bakeCamera,
					renderPipeline,
					entityMgr);

				bakeCameraObj->remove();
			};

			ui->addButton(layout, L"Export to file")->OnPress = [&](GUI::CBase* button)
			{
				CReflectionProbe* reflectionProbe = (CReflectionProbe*)m_component;
				CSceneController* controller = CSceneController::getInstance();
				std::string assetFolder = CAssetManager::getInstance()->getAssetFolder();

				GUI::COpenSaveDialog* dialog = new GUI::COpenSaveDialog(
					GUI::CGUIContext::getRoot(),
					GUI::COpenSaveDialog::Save,
					assetFolder.c_str(),
					assetFolder.c_str(),
					"png;*"
				);

				dialog->OnSave = [reflectionProbe, controller](std::string path)
				{
					CScene* scene = controller->getScene();
					CZone* zone = controller->getZone();

					IRenderPipeline* renderPipeline = controller->getRenderPipeLine();
					if (renderPipeline == NULL)
						return;

					CGameObject* bakeCameraObj = zone->createEmptyObject();
					CCamera* bakeCamera = bakeCameraObj->addComponent<CCamera>();
					scene->updateAddRemoveObject();

					CEntityManager* entityMgr = scene->getEntityManager();
					entityMgr->update();

					std::string folder = CPath::getFolderPath(path);
					std::string name = CPath::getFileNameNoExt(path);

					reflectionProbe->bakeProbeToFile(
						bakeCamera,
						renderPipeline,
						entityMgr,
						folder.c_str(),
						name.c_str()
					);

					bakeCameraObj->remove();
				};
			};
		}

		void CReflectionProbeEditor::update()
		{

		}
	}
}