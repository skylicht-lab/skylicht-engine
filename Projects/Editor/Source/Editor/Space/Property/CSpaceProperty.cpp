/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CSpaceProperty.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceProperty::CSpaceProperty(GUI::CDockableWindow* window, CEditor* editor) :
			CSpace(window, editor)
		{
			GUI::CBase* titleBar = new GUI::CBase(window);
			titleBar->setHeight(25.0f);
			titleBar->dock(GUI::EPosition::Top);
			titleBar->setPadding(GUI::SPadding(5.0f, 3.0f, 5.0f, -3.0f));
			titleBar->enableRenderFillRect(true);
			titleBar->setFillRectColor(GUI::CThemeConfig::WindowBackgroundColor);

			GUI::CIcon* icon = new GUI::CIcon(titleBar, GUI::ESystemIcon::Res3D);
			icon->dock(GUI::EPosition::Left);

			GUI::CLabel* id = new GUI::CLabel(titleBar);
			id->setSize(100.0f, 20.0f);
			id->setMargin(GUI::SMargin(0.0f, 2.0f, 5.0f));
			id->dock(GUI::EPosition::Right);
			id->setString(L"ID: 298312903");

			GUI::CLabel* name = new GUI::CLabel(titleBar);
			name->setMargin(GUI::SMargin(5.0f, 2.0f));
			name->dock(GUI::EPosition::Fill);
			name->setString(L"Object Name");

			GUI::CBoxLayout* boxLayout;

			GUI::CCollapsibleGroup* transformColapsible = new GUI::CCollapsibleGroup(window);
			transformColapsible->dock(GUI::EPosition::Top);
			transformColapsible->getHeader()->setLabel(L"Transform");

			boxLayout = new GUI::CBoxLayout(transformColapsible);
			boxLayout->setPadding(GUI::SPadding(5.0, 5.0, 5.0, 5.0));
			addText(boxLayout, L"Position X", L"0.0");
			addText(boxLayout, L"Y", L"0.0");
			addText(boxLayout, L"Z", L"0.0");

			transformColapsible->setExpand(true);

			GUI::CCollapsibleGroup* rendererColapsible = new GUI::CCollapsibleGroup(window);
			rendererColapsible->dock(GUI::EPosition::Top);
			rendererColapsible->getHeader()->setLabel(L"Render Mesh");

			GUI::CCollapsibleGroup* indirectLighting = new GUI::CCollapsibleGroup(window);
			indirectLighting->dock(GUI::EPosition::Top);
			indirectLighting->getHeader()->setLabel(L"Indirect Lighting");
		}

		CSpaceProperty::~CSpaceProperty()
		{

		}

		void CSpaceProperty::addText(GUI::CBoxLayout* boxLayout, wchar_t* name, wchar_t* value)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CTextBox* text = new GUI::CTextBox(layout);
			text->setString(value);

			boxLayout->endVertical();
		}
	}
}