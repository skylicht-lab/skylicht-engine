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
#include "Selection/CSelection.h"
#include "Editor/SpaceController/CPropertyController.h"
#include "Editor/SpaceController/CSceneController.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceProperty::CSpaceProperty(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_menuContextEditor(NULL)
		{
			GUI::CScrollControl* scrollWindow = new GUI::CScrollControl(window);
			scrollWindow->dock(GUI::EPosition::Fill);
			scrollWindow->enableScroll(false, true);
			scrollWindow->showScrollBar(false, true);
			scrollWindow->enableModifyChildWidth(true);

			m_content = scrollWindow;

			GUI::CBase* titleBar = new GUI::CBase(m_content);
			titleBar->setHeight(25.0f);
			titleBar->dock(GUI::EPosition::Top);
			titleBar->setPadding(GUI::SPadding(5.0f, 3.0f, -5.0f, -3.0f));
			titleBar->enableRenderFillRect(true);
			titleBar->setFillRectColor(GUI::CThemeConfig::WindowBackgroundColor);

			m_icon = new GUI::CIcon(titleBar, GUI::ESystemIcon::None);
			m_icon->dock(GUI::EPosition::Left);

			m_label = new GUI::CLabel(titleBar);
			m_label->setMargin(GUI::SMargin(5.0f, 2.0f));
			m_label->dock(GUI::EPosition::Fill);

			m_componentContextMenu = new GUI::CMenu(window->getCanvas());
			m_componentContextMenu->addItem(L"Reset");
			m_componentContextMenu->addSeparator();
			m_componentContextMenu->addItem(L"Copy");
			m_componentContextMenu->addItem(L"Paste");
			m_componentContextMenu->addSeparator();
			m_menuUp = m_componentContextMenu->addItem(L"Move up");
			m_menuDown = m_componentContextMenu->addItem(L"Move down");
			m_componentContextMenu->addSeparator();
			m_menuRemove = m_componentContextMenu->addItem(L"Remove Component");
			m_componentContextMenu->OnCommand = BIND_LISTENER(&CSpaceProperty::OnComponentCommand, this);

			m_addComponentMenu = new GUI::CMenu(window->getCanvas());
			m_addComponentController = new CAddComponentController(editor, m_addComponentMenu);

			CPropertyController::getInstance()->setSpaceProperty(this);
		}

		CSpaceProperty::~CSpaceProperty()
		{
			delete m_addComponentController;

			for (SGroup* group : m_groups)
			{
				for (IObserver* o : group->Observer)
				{
					delete o;
				}

				delete group;
			}
			m_groups.clear();

			for (CComponentEditor* editor : m_releaseComponents)
			{
				delete editor;
			}
			m_releaseComponents.clear();

			CPropertyController::getInstance()->setSpaceProperty(NULL);
		}

		void CSpaceProperty::OnComponentCommand(GUI::CBase* base)
		{
			if (m_menuContextEditor == NULL)
				return;

			CGameObject* gameObject = m_menuContextEditor->getGameObject();
			CComponentSystem* component = m_menuContextEditor->getComponent();

			if (gameObject == NULL || component == NULL)
				return;

			GUI::CMenuItem* menuItem = dynamic_cast<GUI::CMenuItem*>(base);
			const std::wstring& label = menuItem->getLabel();

			if (label == L"Reset")
			{
				component->reset();
			}
			else if (label == L"Move up")
			{
				gameObject->moveSerializableComponentUp(component);
			}
			else if (label == L"Move down")
			{
				gameObject->moveSerializableComponentDown(component);
			}
			else if (label == L"Remove Component")
			{
				gameObject->removeComponent(component);
			}

			getEditor()->refresh();
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

			for (CComponentEditor* editor : m_releaseComponents)
			{
				delete editor;
			}
			m_releaseComponents.clear();
		}

		void CSpaceProperty::update()
		{
			for (SGroup* group : m_groups)
			{
				if (group->Owner != NULL)
					group->Owner->update();
			}
		}

		void CSpaceProperty::refresh()
		{

		}

		void CSpaceProperty::addComponent(CComponentEditor* editor, CComponentSystem* component, bool autoRelease)
		{
			editor->initGUI(component, this);

			if (autoRelease)
				m_releaseComponents.push_back(editor);
		}

		void CSpaceProperty::addComponent(CComponentEditor* editor, CGameObject* gameobject)
		{
			editor->initGUI(gameobject, this);
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const wchar_t* label, CComponentEditor* editor)
		{
			GUI::CCollapsibleGroup* colapsible = new GUI::CCollapsibleGroup(m_content);
			colapsible->dock(GUI::EPosition::Top);
			colapsible->getHeader()->setLabel(label);

			SGroup* g = new SGroup();
			g->Owner = editor;
			g->GroupUI = colapsible;

			if (editor->getComponent() != NULL)
			{
				// add setting button on component group
				GUI::CButton* btn = new GUI::CButton(colapsible->getHeader());
				btn->setWidth(20.0f);
				btn->setPadding(GUI::SPadding(0.0f, 0.0f, 0.0f, 0.0f));
				btn->dock(GUI::EPosition::Right);
				btn->setIcon(GUI::ESystemIcon::Setting);
				btn->showIcon(true);
				btn->showLabel(false);
				btn->enableDrawBackground(false);
				btn->tagData(editor);
				btn->OnPress = [&, editor](GUI::CBase* button)
				{
					CGameObject* object = editor->getGameObject();
					CComponentSystem* component = editor->getComponent();

					if (dynamic_cast<CTransformEuler*>(component) != NULL)
					{
						// disable move & delete on transform editor
						m_menuUp->setDisabled(true);
						m_menuDown->setDisabled(true);
						m_menuRemove->setDisabled(true);
					}
					else
					{
						int pos = object->getComponentPosition(component);
						if (pos > 1)
							m_menuUp->setDisabled(false);
						else
							m_menuUp->setDisabled(true);

						if (pos < object->getSerializableComponentCount() - 1)
							m_menuDown->setDisabled(false);
						else
							m_menuDown->setDisabled(true);

						m_menuRemove->setDisabled(false);
					}

					m_menuContextEditor = editor;
					m_componentContextMenu->open(button);
				};
			}

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

		GUI::CButton* CSpaceProperty::addButton(const wchar_t* label)
		{
			GUI::CBase* content = new GUI::CBase(m_content);
			content->setHeight(45);
			content->setMargin(GUI::SMargin(0.0f, 10.0f, 0.0f, 0.0f));
			content->dock(GUI::EPosition::Top);
			content->setFillRectColor(GUI::CThemeConfig::WindowBackgroundColor);
			content->enableRenderFillRect(true);

			GUI::CBase* line = new GUI::CBase(content);
			line->setHeight(1.0f);
			line->setFillRectColor(GUI::SGUIColor(255, 0, 0, 0));
			line->enableRenderFillRect(true);
			line->dock(GUI::EPosition::Top);

			GUI::CButton* button = new GUI::CButton(content);
			button->setMargin(GUI::SMargin(10.0f, 10.0f, 10.0f, 0.0f));
			button->setLabel(label);
			button->setTextAlignment(GUI::ETextAlign::TextCenter);
			button->dock(GUI::EPosition::Top);

			SGroup* g = new SGroup();
			g->Owner = NULL;
			g->GroupUI = content;
			m_groups.push_back(g);

			return button;
		}

		void CSpaceProperty::popupComponentMenu(CGameObject* gameObject, GUI::CBase* position)
		{
			m_addComponentController->SetGameObject(gameObject);

			m_addComponentMenu->setWidth(position->width());
			m_addComponentMenu->open(position);
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
			boxLayout->setPadding(GUI::SPadding(5.0, 5.0, -15.0, 5.0));
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

		void CSpaceProperty::addTextBox(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<std::wstring>* value)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CTextBox* input = new GUI::CTextBox(layout);
			input->setString(value->get());

			CSpaceProperty::SGroup* group = getGroupByLayout(boxLayout);
			if (group != NULL)
			{
				// when value change
				CObserver<GUI::CTextBox>* onChange = new CObserver<GUI::CTextBox>(input);
				onChange->Notify = [me = onChange](ISubject* subject, IObserver* from, GUI::CTextBox* target)
				{
					if (from != me)
					{
						CSubject<std::wstring>* value = (CSubject<std::wstring>*)subject;
						target->setString(value->get());
					}
				};

				// when input text change
				IObserver* observer = value->addObserver(onChange);
				input->OnTextChanged = [value, input, observer](GUI::CBase* base) {
					value->set(input->getString());
					value->notify(observer);
				};

				// hold observer to release later			
				group->Observer.push_back(observer);
			}

			boxLayout->endVertical();
		}

		void CSpaceProperty::addCheckBox(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<bool>* value)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CBase* subLayout = new GUI::CBase(layout);

			GUI::CCheckBox* check = new GUI::CCheckBox(subLayout);
			check->setToggle(value->get());

			CSpaceProperty::SGroup* group = getGroupByLayout(boxLayout);
			if (group != NULL)
			{
				// when check value c hange
				CObserver<GUI::CCheckBox>* onChange = new CObserver<GUI::CCheckBox>(check);
				onChange->Notify = [me = onChange](ISubject* subject, IObserver* from, GUI::CCheckBox* target)
				{
					if (from != me)
					{
						CSubject<bool>* value = (CSubject<bool>*)subject;
						target->setToggle(value->get());
					}
				};

				// when check control change
				IObserver* observer = value->addObserver(onChange);
				check->OnChanged = [value, check, observer](GUI::CBase* base) {
					value->set(check->getToggle());
					value->notify(observer);
				};

				// hold observer to release later			
				group->Observer.push_back(observer);
			}

			boxLayout->endVertical();
		}

		void CSpaceProperty::addComboBox(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<std::wstring>* value, const std::vector<std::wstring>& listValue)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CComboBox* comboBox = new GUI::CComboBox(layout);
			comboBox->setListValue(listValue);

			// when value change
			CObserver<GUI::CComboBox>* onChange = new CObserver<GUI::CComboBox>(comboBox);
			onChange->Notify = [me = onChange](ISubject* subject, IObserver* from, GUI::CComboBox* target)
			{
				if (from != me)
				{
					CSubject<std::wstring>* value = (CSubject<std::wstring>*)subject;
					target->setLabel(value->get());
				}
			};

			// when combobox change
			IObserver* observer = value->addObserver(onChange);
			comboBox->OnChanged = [value, comboBox, observer](GUI::CBase* base)
			{
				value->set(comboBox->getLabel());
				value->notify(observer);
			};

			boxLayout->endVertical();
		}

		void CSpaceProperty::addSlider(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<float>* value, float min, float max)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CSlider* slider = new GUI::CSlider(layout);
			slider->setValue(value->get(), min, max, false);

			// when value change
			CObserver<GUI::CSlider>* onChange = new CObserver<GUI::CSlider>(slider);
			onChange->Notify = [me = onChange](ISubject* subject, IObserver* from, GUI::CSlider* target)
			{
				if (from != me)
				{
					CSubject<float>* value = (CSubject<float>*)subject;
					target->setValue(value->get(), false);
				}
			};

			// when slider change
			IObserver* observer = value->addObserver(onChange);
			slider->OnTextChanged = [value, slider, observer](GUI::CBase* base) {
				value->set(slider->getValue());
				value->notify(observer);
			};

			boxLayout->endVertical();
		}

		GUI::CDropdownBox* CSpaceProperty::addDropBox(GUI::CBoxLayout* boxLayout, const wchar_t* name, const std::wstring& value)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CDropdownBox* comboBox = new GUI::CDropdownBox(layout);
			comboBox->setLabel(value);

			boxLayout->endVertical();

			return comboBox;
		}
	}
}