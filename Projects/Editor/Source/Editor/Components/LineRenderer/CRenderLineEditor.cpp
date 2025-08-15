#include "pch.h"
#include "CRenderLineEditor.h"

#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/Space/Hierarchy/CHierarchyController.h"
#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		EDITOR_REGISTER(CRenderLineEditor, CRenderLine);

		CRenderLineEditor::CRenderLineEditor()
		{

		}

		CRenderLineEditor::~CRenderLineEditor()
		{

		}

		void CRenderLineEditor::initCustomGUI(GUI::CBoxLayout* layout, CSpaceProperty* ui)
		{
			layout->addSpace(5.0f);

			ui->addButton(layout, L"Add Point")->OnPress = [&](GUI::CBase* button)
				{
					CRenderLine* probes = (CRenderLine*)m_component;
					CEntity* newPoint = probes->spawn();

					CSceneController* sceneController = CSceneController::getInstance();
					sceneController->updateTreeNode(m_gameObject);
					sceneController->deselectAllOnHierachy();
					sceneController->selectOnHierachy(newPoint);
				};
		}

		void CRenderLineEditor::update()
		{

		}
	}
}