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
					CRenderLine* line = (CRenderLine*)m_component;

					CEntity* newPoint = NULL;

					u32 numPoint = line->getNumPoint();
					if (numPoint == 0)
					{
						newPoint = line->spawn();
						newPoint = line->spawn();
						CWorldTransformData* t = GET_ENTITY_DATA(newPoint, CWorldTransformData);
						t->Relative.setTranslation(core::vector3df(0.0f, 0.0f, 1.0f));
					}
					else
					{
						CWorldTransformData* lastPoint = line->getPoint(numPoint - 1);

						CEntity* newPoint = line->spawn();
						if (lastPoint)
						{
							CWorldTransformData* t = GET_ENTITY_DATA(newPoint, CWorldTransformData);
							t->Relative = lastPoint->Relative;
						}
					}

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