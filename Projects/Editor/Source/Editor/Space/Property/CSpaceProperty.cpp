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
#include "Utils/CStringImp.h"

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

			addCheckBox(boxLayout, L"Enable", true);
			boxLayout->addSpace(5.0f);
			addNumberInput(boxLayout, L"Position X", 0.0f, 0.1f);
			addNumberInput(boxLayout, L"Y", 0.0f, 0.1f);
			addNumberInput(boxLayout, L"Z", 0.0f, 0.1f);
			boxLayout->addSpace(5.0f);
			addNumberInput(boxLayout, L"Rotation(Deg) X", 0.0f, 0.1f);
			addNumberInput(boxLayout, L"Y", 0.0f, 0.1f);
			addNumberInput(boxLayout, L"Z", 0.0f, 0.1f);
			boxLayout->addSpace(5.0f);
			addNumberInput(boxLayout, L"Scale X", 1.0f, 0.1f);
			addNumberInput(boxLayout, L"Y", 1.0f, 0.1f);
			addNumberInput(boxLayout, L"Z", 1.0f, 0.1f);

			transformColapsible->setExpand(true);


			GUI::CCollapsibleGroup* rendererColapsible = new GUI::CCollapsibleGroup(window);
			rendererColapsible->dock(GUI::EPosition::Top);
			rendererColapsible->getHeader()->setLabel(L"Render Mesh");

			boxLayout = new GUI::CBoxLayout(rendererColapsible);
			boxLayout->setPadding(GUI::SPadding(5.0, 5.0, 5.0, 5.0));

			addCheckBox(boxLayout, L"Enable", true);

			rendererColapsible->setExpand(true);


			GUI::CCollapsibleGroup* indirectLighting = new GUI::CCollapsibleGroup(window);
			indirectLighting->dock(GUI::EPosition::Top);
			indirectLighting->getHeader()->setLabel(L"Indirect Lighting");

			boxLayout = new GUI::CBoxLayout(indirectLighting);
			boxLayout->setPadding(GUI::SPadding(5.0, 5.0, 5.0, 5.0));

			addCheckBox(boxLayout, L"Enable", true);
			boxLayout->addSpace(5.0f);

			std::vector<std::string> list;
			list.push_back("LightmapArray");
			list.push_back("VertexColor");
			list.push_back("SH4");
			list.push_back("SH9");
			addComboBox(boxLayout, L"Type", list[0], list);
			addSlider(boxLayout, L"Indirect multipler", 1.0f, 0.0f, 1.0f);
			addSlider(boxLayout, L"Direct multipler", 1.0f, 0.0f, 1.0f);

			indirectLighting->setExpand(true);
		}

		CSpaceProperty::~CSpaceProperty()
		{

		}

		void CSpaceProperty::addNumberInput(GUI::CBoxLayout* boxLayout, wchar_t* name, float value, float step)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CNumberInput* input = new GUI::CNumberInput(layout);
			input->setValue(value);
			input->setStep(step);

			boxLayout->endVertical();
		}

		void CSpaceProperty::addCheckBox(GUI::CBoxLayout* boxLayout, wchar_t* name, bool value)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CBase* subLayout = new GUI::CBase(layout);

			GUI::CCheckBox* check = new GUI::CCheckBox(subLayout);
			check->setToggle(value);

			boxLayout->endVertical();
		}

		void CSpaceProperty::addComboBox(GUI::CBoxLayout* boxLayout, wchar_t* name, const std::string& value, const std::vector<std::string>& listValue)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			wchar_t wtext[512];

			std::vector<std::wstring> list;
			for (const std::string& s : listValue)
			{
				CStringImp::convertUTF8ToUnicode(s.c_str(), wtext);
				list.push_back(wtext);
			}

			CStringImp::convertUTF8ToUnicode(value.c_str(), wtext);

			GUI::CComboBox* comboBox = new GUI::CComboBox(layout);
			comboBox->setListValue(list);

			boxLayout->endVertical();
		}

		void CSpaceProperty::addSlider(GUI::CBoxLayout* boxLayout, wchar_t* name, float value, float min, float max)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CSlider* slider = new GUI::CSlider(layout);
			slider->setValue(value, min, max);

			boxLayout->endVertical();
		}
	}
}