#pragma once

#include "IndirectLighting/CIndirectLighting.h"
#include "Editor/Components/Default/CDefaultEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		class CIndirectLightingEditor : public CDefaultEditor
		{
		public:
			CIndirectLightingEditor();

			virtual ~CIndirectLightingEditor();

			virtual void initCustomValueGUI(CObjectSerializable* obj, CValueProperty* data, GUI::CBoxLayout* layout, CSpaceProperty* ui);

			virtual void update();
		};
	}
}