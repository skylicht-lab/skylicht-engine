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
#include "Editor/SpaceController/CPropertyController.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceProperty::CSpaceProperty(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor)
		{
			GUI::CScrollControl* scrollWindow = new GUI::CScrollControl(window);
			scrollWindow->dock(GUI::EPosition::Fill);
			scrollWindow->enableScroll(false, true);
			scrollWindow->showScrollBar(false, true);

			m_content = new GUI::CContentSizeControl(scrollWindow);
			m_content->setFitType(GUI::CContentSizeControl::SizeToParent, GUI::CContentSizeControl::WrapChildren);
			m_content->setTargetParent(window);

			GUI::CBase* titleBar = new GUI::CBase(m_content);
			titleBar->setHeight(25.0f);
			titleBar->dock(GUI::EPosition::Top);
			titleBar->setPadding(GUI::SPadding(5.0f, 3.0f, 5.0f, -3.0f));
			titleBar->enableRenderFillRect(true);
			titleBar->setFillRectColor(GUI::CThemeConfig::WindowBackgroundColor);

			m_icon = new GUI::CIcon(titleBar, GUI::ESystemIcon::None);
			m_icon->dock(GUI::EPosition::Left);

			m_label = new GUI::CLabel(titleBar);
			m_label->setMargin(GUI::SMargin(5.0f, 2.0f));
			m_label->dock(GUI::EPosition::Fill);

			/*
			GUI::CBoxLayout* boxLayout;

			GUI::CCollapsibleGroup* transformColapsible = addGroup(L"Transform");
			boxLayout = createBoxLayout(transformColapsible);
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

			GUI::CCollapsibleGroup* rendererColapsible = addGroup(L"Render Mesh");
			boxLayout = createBoxLayout(rendererColapsible);
			addCheckBox(boxLayout, L"Enable", true);
			rendererColapsible->setExpand(true);


			GUI::CCollapsibleGroup* indirectLighting = addGroup(L"Indirect Lighting");
			boxLayout = createBoxLayout(indirectLighting);

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
			*/

			CPropertyController::getInstance()->setSpaceProperty(this);
		}

		CSpaceProperty::~CSpaceProperty()
		{
			CPropertyController::getInstance()->setSpaceProperty(NULL);
		}

		void CSpaceProperty::clearAllGroup()
		{
			for (GUI::CCollapsibleGroup* group : m_groups)
			{
				group->remove();
			}
			m_groups.clear();
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const wchar_t* label)
		{
			GUI::CCollapsibleGroup* transformColapsible = new GUI::CCollapsibleGroup(m_content);
			transformColapsible->dock(GUI::EPosition::Top);
			transformColapsible->getHeader()->setLabel(label);
			m_groups.push_back(transformColapsible);
			return transformColapsible;
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const char* label)
		{
			std::wstring wlabel = CStringImp::convertUTF8ToUnicode(label);

			GUI::CCollapsibleGroup* transformColapsible = new GUI::CCollapsibleGroup(m_content);
			transformColapsible->dock(GUI::EPosition::Top);
			transformColapsible->getHeader()->setLabel(wlabel.c_str());
			m_groups.push_back(transformColapsible);
			return transformColapsible;
		}

		GUI::CBoxLayout* CSpaceProperty::createBoxLayout(GUI::CCollapsibleGroup* group)
		{
			GUI::CBoxLayout* boxLayout = new GUI::CBoxLayout(group);
			boxLayout->setPadding(GUI::SPadding(5.0, 5.0, 15.0, 5.0));
			return boxLayout;
		}

		void CSpaceProperty::addNumberInput(GUI::CBoxLayout* boxLayout, const wchar_t* name, float value, float step)
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

		void CSpaceProperty::addCheckBox(GUI::CBoxLayout* boxLayout, const wchar_t* name, bool value)
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

		void CSpaceProperty::addComboBox(GUI::CBoxLayout* boxLayout, const wchar_t* name, const std::string& value, const std::vector<std::string>& listValue)
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

		void CSpaceProperty::addSlider(GUI::CBoxLayout* boxLayout, const wchar_t* name, float value, float min, float max)
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