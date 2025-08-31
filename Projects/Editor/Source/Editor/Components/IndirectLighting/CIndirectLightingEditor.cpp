#include "pch.h"
#include "CIndirectLightingEditor.h"

#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/Space/Hierarchy/CHierarchyController.h"
#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		EDITOR_REGISTER(CIndirectLightingEditor, CIndirectLighting);

		CIndirectLightingEditor::CIndirectLightingEditor()
		{

		}

		CIndirectLightingEditor::~CIndirectLightingEditor()
		{

		}

		void CIndirectLightingEditor::update()
		{

		}

		void CIndirectLightingEditor::initCustomValueGUI(CObjectSerializable* obj, CValueProperty* data, GUI::CBoxLayout* boxLayout, CSpaceProperty* ui)
		{
			if (data->Name == "lightLayers")
			{
				CUIntProperty* value = dynamic_cast<CUIntProperty*>(data);
				if (value == NULL)
					return;

				initLightLayerMenu(obj, value, boxLayout, ui);
			}
		}
	}
}