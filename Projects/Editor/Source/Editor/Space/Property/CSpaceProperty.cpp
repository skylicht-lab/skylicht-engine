/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
			for (SGroup* group : m_groups)
			{
				for (IObserver* o : group->Observer)
				{
					delete o;
				}

				delete group;
			}
			m_groups.clear();

			CPropertyController::getInstance()->setSpaceProperty(NULL);
		}

		void CSpaceProperty::clearAllGroup()
		{
			for (SGroup* group : m_groups)
			{
				group->releaseObserver();
				group->GroupUI->remove();
				delete group;
			}

			m_groups.clear();
		}

		void CSpaceProperty::update()
		{
			for (SGroup* group : m_groups)
			{
				group->Owner->update();
			}
		}

		void CSpaceProperty::addComponent(CComponentEditor* editor, CComponentSystem* component)
		{
			editor->initGUI(component, this);
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const wchar_t* label, CComponentEditor* editor)
		{
			GUI::CCollapsibleGroup* colapsible = new GUI::CCollapsibleGroup(m_content);
			colapsible->dock(GUI::EPosition::Top);
			colapsible->getHeader()->setLabel(label);

			SGroup* g = new SGroup();
			g->Owner = editor;
			g->GroupUI = colapsible;

			m_groups.push_back(g);
			return colapsible;
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const char* label, CComponentEditor* editor)
		{
			std::wstring wlabel = CStringImp::convertUTF8ToUnicode(label);
			return addGroup(wlabel.c_str(), editor);
		}

		void CSpaceProperty::removeGroupByOwner(CComponentEditor* editor)
		{
			for (size_t i = 0, n = m_groups.size(); i < n; i++)
			{
				if (m_groups[i]->Owner == editor)
				{
					m_groups[i]->releaseObserver();
					m_groups[i]->GroupUI->remove();
					m_groups.erase(m_groups.begin() + i);
					return;
				}
			}
		}

		CSpaceProperty::SGroup* CSpaceProperty::getGroupByLayout(GUI::CBoxLayout* layout)
		{
			for (SGroup* group : m_groups)
			{
				// inner of parent -> parent
				if (layout->getParent()->getParent() == group->GroupUI)
				{
					return group;
				}
			}
			return NULL;
		}

		GUI::CBoxLayout* CSpaceProperty::createBoxLayout(GUI::CCollapsibleGroup* group)
		{
			GUI::CBoxLayout* boxLayout = new GUI::CBoxLayout(group);
			boxLayout->setPadding(GUI::SPadding(5.0, 5.0, 15.0, 5.0));
			return boxLayout;
		}

		void CSpaceProperty::addNumberInput(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<float>* value, float step)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CNumberInput* input = new GUI::CNumberInput(layout);
			input->setValue(value->get(), false);
			input->setStep(step);

			m_window->getCanvas()->TabableGroup.add(input);

			boxLayout->endVertical();

			CSpaceProperty::SGroup* group = getGroupByLayout(boxLayout);
			if (group != NULL)
			{
				// when value change
				CObserver<GUI::CNumberInput>* onChange = new CObserver<GUI::CNumberInput>(input);
				onChange->Notify = [me = onChange](ISubject* subject, IObserver* from, GUI::CNumberInput* target)
				{
					if (from != me)
					{
						CSubject<float>* floatValue = (CSubject<float>*)subject;
						target->setValue(floatValue->get(), false);
					}
				};
				IObserver* observer = value->addObserver(onChange);

				// when input text change
				input->OnTextChanged = [value, input, observer](GUI::CBase* base) {
					value->set(input->getValue());
					value->notify(observer);
				};

				// hold observer to release later
				group->Observer.push_back(observer);
			}
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