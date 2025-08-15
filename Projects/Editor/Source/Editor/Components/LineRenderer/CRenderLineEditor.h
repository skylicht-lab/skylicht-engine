#pragma once

#include "RenderLine/CRenderLine.h"
#include "Editor/Components/Default/CDefaultEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		class CRenderLineEditor : public CDefaultEditor
		{
		public:
			CRenderLineEditor();

			virtual ~CRenderLineEditor();

			virtual void initCustomGUI(GUI::CBoxLayout* layout, CSpaceProperty* ui);

			virtual void update();
		};
	}
}