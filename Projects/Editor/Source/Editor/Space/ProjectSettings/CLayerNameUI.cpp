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
#include "CLayerNameUI.h"

namespace Skylicht
{
	namespace Editor
	{
		CLayerNameUI::CLayerNameUI(CSpaceProjectSettings* settings, GUI::CBase* base, const std::wstring& name) :
			CProjectSettingUI(settings, base, name)
		{
			GUI::CCollapsibleGroup* colapsible = new GUI::CCollapsibleGroup(base);
			colapsible->dock(GUI::EPosition::Top);
			colapsible->getHeader()->setFillRectColor(GUI::SGUIColor(255, 65, 65, 65));
			colapsible->getHeader()->setLabel(name);

			GUI::CBoxLayout* boxLayout = new GUI::CBoxLayout(colapsible);
			boxLayout->setPadding(GUI::SPadding(5.0, 5.0, -15.0, 5.0));

			wchar_t text[256];

			for (int i = 0; i < 16; i++)
			{
				GUI::CLayout* layout = boxLayout->beginColumn();

				GUI::CLabel* label = new GUI::CLabel(layout);
				label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
				label->setWidth(200.0f);

				swprintf(text, L"Layer: %d", i);
				label->setString(text);

				label->setTextAlignment(GUI::TextRight);

				GUI::CTextBox* input = new GUI::CTextBox(layout);

				if (i == 0)
				{
					input->setString(L"Default");
					input->setEditable(false);
				}

				boxLayout->endColumn();
			}

			colapsible->setExpand(true);
		}

		CLayerNameUI::~CLayerNameUI()
		{

		}
	}
}