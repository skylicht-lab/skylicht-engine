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
#include "AssetManager/CAssetManager.h"
#include "Editor/SpaceController/CPropertyController.h"
#include "Editor/SpaceController/CAssetPropertyController.h"
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

			m_labelButton = new GUI::CButton(titleBar);
			m_labelButton->setPos(2.0f, 2.0f);
			m_labelButton->setHeight(20.0f);
			m_labelButton->setHidden(true);
			m_labelButton->showIcon(true);
			m_labelButton->enableDrawBackground(false);
			m_labelButton->getTextContainer()->setMargin(GUI::SMargin(5.0f, 3.0));

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
		}

		CSpaceProperty::~CSpaceProperty()
		{
			delete m_addComponentController;

			for (SGroup* group : m_groups)
			{
				delete group;
			}
			m_groups.clear();

			for (CComponentEditor* editor : m_releaseComponents)
			{
				delete editor;
			}
			m_releaseComponents.clear();
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

			CSceneController::getInstance()->updateTreeNode(gameObject);

			getEditor()->refresh();
		}

		void CSpaceProperty::clearAllGroup()
		{
			bool changed = false;
			std::vector<CGameObject*> objs;

			for (SGroup* group : m_groups)
			{
				group->GroupUI->remove();
				if (group->AssetOwner)
					group->AssetOwner->closeGUI();
				if (group->Owner)
				{
					if (group->Owner->isChanged())
					{
						// need save history
						CGameObject* obj = group->Owner->getGameObject();
						if (std::find(objs.begin(), objs.end(), obj) == objs.end())
							objs.push_back(obj);
						changed = true;
					}

					group->Owner->closeGUI();
				}

				if (group->EntityDataOwner)
					group->EntityDataOwner->closeGUI();

				if (group->GUIEditorOwner)
					group->GUIEditorOwner->closeGUI();

				delete group;
			}

			// save modify history
			if (changed && objs.size() > 0)
				CSceneController::getInstance()->getHistory()->saveModifyHistory(objs);

			// delete editor components
			for (CComponentEditor* editor : m_releaseComponents)
				delete editor;
			m_releaseComponents.clear();
			m_groups.clear();
		}

		void CSpaceProperty::update()
		{
			if (!m_addComponentMenu->isHidden())
				m_addComponentController->update();

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

		void CSpaceProperty::addAsset(CAssetEditor* editor, const char* path)
		{
			editor->initGUI(path, this);
		}

		void CSpaceProperty::addEntityData(CEntityDataEditor* editor, IEntityData* entityData)
		{
			editor->initGUI(entityData, this);
		}

		void CSpaceProperty::addGUIEditor(CGUIEditor* editor, CGUIElement* gui)
		{
			editor->initGUI(gui, this);
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const wchar_t* label, CAssetEditor* editor)
		{
			GUI::CCollapsibleGroup* colapsible = new GUI::CCollapsibleGroup(m_content);
			colapsible->dock(GUI::EPosition::Top);
			colapsible->getHeader()->setLabel(label);

			SGroup* g = new SGroup();
			g->AssetOwner = editor;
			g->GroupUI = colapsible;

			m_groups.push_back(g);
			return colapsible;
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const wchar_t* label, CComponentEditor* editor)
		{
			GUI::CCollapsibleGroup* colapsible = new GUI::CCollapsibleGroup(m_content);
			colapsible->dock(GUI::EPosition::Top);
			colapsible->getHeader()->setLabel(label);

			SGroup* g = new SGroup();
			g->Owner = editor;
			g->GroupUI = colapsible;

			if (editor != NULL && editor->getComponent() != NULL)
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

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const wchar_t* label, CEntityDataEditor* editor)
		{
			GUI::CCollapsibleGroup* colapsible = new GUI::CCollapsibleGroup(m_content);
			colapsible->dock(GUI::EPosition::Top);
			colapsible->getHeader()->setLabel(label);

			SGroup* g = new SGroup();
			g->EntityDataOwner = editor;
			g->GroupUI = colapsible;

			m_groups.push_back(g);
			return colapsible;
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const wchar_t* label, CGUIEditor* editor)
		{
			GUI::CCollapsibleGroup* colapsible = new GUI::CCollapsibleGroup(m_content);
			colapsible->dock(GUI::EPosition::Top);
			colapsible->getHeader()->setLabel(label);

			SGroup* g = new SGroup();
			g->GUIEditorOwner = editor;
			g->GroupUI = colapsible;

			m_groups.push_back(g);
			return colapsible;
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const char* label, CComponentEditor* editor)
		{
			std::wstring wlabel = CStringImp::convertUTF8ToUnicode(label);
			return addGroup(wlabel.c_str(), editor);
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const char* label, CAssetEditor* editor)
		{
			std::wstring wlabel = CStringImp::convertUTF8ToUnicode(label);
			return addGroup(wlabel.c_str(), editor);
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const char* label, CEntityDataEditor* editor)
		{
			std::wstring wlabel = CStringImp::convertUTF8ToUnicode(label);
			return addGroup(wlabel.c_str(), editor);
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const char* label, CGUIEditor* editor)
		{
			std::wstring wlabel = CStringImp::convertUTF8ToUnicode(label);
			return addGroup(wlabel.c_str(), editor);
		}

		GUI::CLabel* CSpaceProperty::addLabel(GUI::CBoxLayout* boxLayout, const wchar_t* label, GUI::ETextAlign align)
		{
			GUI::CLayoutVertical* layout = boxLayout->beginVertical();

			GUI::CLabel* labelControl = new GUI::CLabel(layout);
			labelControl->setString(label);
			labelControl->setTextAlignment(align);
			boxLayout->endVertical();

			return labelControl;
		}

		GUI::CButton* CSpaceProperty::addButton(GUI::CBoxLayout* boxLayout, const wchar_t* label)
		{
			GUI::CLayoutVertical* layout = boxLayout->beginVertical();

			GUI::CButton* button = new GUI::CButton(layout);
			button->setLabel(label);
			button->setTextAlignment(GUI::TextCenter);

			boxLayout->endVertical();
			return button;
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

			m_addComponentController->focusSearch();
		}

		CSpaceProperty::SGroup* CSpaceProperty::getGroupByLayout(GUI::CBoxLayout* layout)
		{
			for (SGroup* group : m_groups)
			{
				if (group->GroupUI->isChild(layout, true))
				{
					return group;
				}
			}
			return NULL;
		}

		GUI::CBoxLayout* CSpaceProperty::createBoxLayout(GUI::CCollapsibleGroup* group)
		{
			GUI::CBoxLayout* boxLayout = new GUI::CBoxLayout(group);
			boxLayout->setPadding(GUI::SPadding(15.0, 5.0, -15.0, 5.0));
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
				// when check value change => update ui
				CObserver* onChange = new CObserver();
				onChange->Notify = [me = onChange, target = input, ui = layout](ISubject* subject, IObserver* from)
				{
					if (from != me)
					{
						CSubject<float>* value = (CSubject<float>*)subject;
						target->setValue(value->get(), false);
						ui->setHidden(!value->isEnable());
					}
				};

				value->addObserver(onChange, true);

				// when input text change
				input->OnTextChanged = [value, input, observer = onChange](GUI::CBase* base) {
					value->set(input->getValue());
					value->notify(observer);
				};
			}
		}

		void CSpaceProperty::addNumberInput(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<int>* value, int step)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CNumberInput* input = new GUI::CNumberInput(layout);
			input->setNumberType(GUI::Integer);
			input->setValue((float)value->get(), false);
			input->setStep((float)step);

			m_window->getCanvas()->TabableGroup.add(input);

			boxLayout->endVertical();

			CSpaceProperty::SGroup* group = getGroupByLayout(boxLayout);
			if (group != NULL)
			{
				// when check value change => update ui
				CObserver* onChange = new CObserver();
				onChange->Notify = [me = onChange, target = input, ui = layout](ISubject* subject, IObserver* from)
				{
					if (from != me)
					{
						CSubject<int>* value = (CSubject<int>*)subject;
						target->setValue((float)value->get(), false);
						ui->setHidden(!value->isEnable());
					}
				};

				value->addObserver(onChange, true);

				// when input text change
				input->OnTextChanged = [value, input, observer = onChange](GUI::CBase* base) {
					value->set((int)input->getValue());
					value->notify(observer);
				};
			}
		}

		void CSpaceProperty::addNumberInput(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<u32>* value, int step)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CNumberInput* input = new GUI::CNumberInput(layout);
			input->setNumberType(GUI::UInteger);
			input->setValue((float)value->get(), false);
			input->setStep((float)step);

			m_window->getCanvas()->TabableGroup.add(input);

			boxLayout->endVertical();

			CSpaceProperty::SGroup* group = getGroupByLayout(boxLayout);
			if (group != NULL)
			{
				// when check value change => update ui
				CObserver* onChange = new CObserver();
				onChange->Notify = [me = onChange, target = input, ui = layout](ISubject* subject, IObserver* from)
				{
					if (from != me)
					{
						CSubject<u32>* value = (CSubject<u32>*)subject;
						target->setValue((float)value->get(), false);
						ui->setHidden(!value->isEnable());
					}
				};

				value->addObserver(onChange, true);

				// when input text change
				input->OnTextChanged = [value, input, observer = onChange](GUI::CBase* base) {
					value->set((u32)input->getValue());
					value->notify(observer);
				};
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
				// when check value change => update ui
				CObserver* onChange = new CObserver();
				onChange->Notify = [me = onChange, target = input, ui = layout](ISubject* subject, IObserver* from)
				{
					if (from != me)
					{
						CSubject<std::wstring>* value = (CSubject<std::wstring>*)subject;
						target->setString(value->get());
						ui->setHidden(!value->isEnable());
					}
				};

				// when input text change
				value->addObserver(onChange, true);
				input->OnTextChanged = [value, input, observer = onChange](GUI::CBase* base) {
					value->set(input->getString());
					value->notify(observer);
				};
			}

			boxLayout->endVertical();
		}

		void CSpaceProperty::addNumberTextBox(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<int>* value)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CNumberInput* input = new GUI::CNumberInput(layout);
			input->setNumberType(GUI::ENumberInputType::Integer);
			input->alwayFocusTextbox(true);

			wchar_t text[32];
			swprintf(text, 32, L"%d", value->get());
			input->setString(text);

			CSpaceProperty::SGroup* group = getGroupByLayout(boxLayout);
			if (group != NULL)
			{
				// when check value change => update ui
				CObserver* onChange = new CObserver();
				onChange->Notify = [me = onChange, target = input, ui = layout](ISubject* subject, IObserver* from)
				{
					if (from != me)
					{
						CSubject<std::wstring>* value = (CSubject<std::wstring>*)subject;
						target->setString(value->get());
						ui->setHidden(!value->isEnable());
					}
				};

				// when input text change
				value->addObserver(onChange, true);
				input->OnLostKeyboardFocus = [value, input, observer = onChange](GUI::CBase* base) {
					value->set(input->getValueInt());
					value->notify(observer);
				};
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
				// when check value change => update ui
				CObserver* onChange = new CObserver();
				onChange->Notify = [me = onChange, target = check, ui = layout](ISubject* subject, IObserver* from)
				{
					if (from != me)
					{
						CSubject<bool>* value = (CSubject<bool>*)subject;
						target->setToggle(value->get());
						ui->setHidden(!value->isEnable());
					}
				};

				// when check control change => update value
				value->addObserver(onChange, true);
				check->OnChanged = [value, check, observer = onChange](GUI::CBase* base) {
					value->set(check->getToggle());
					value->notify(observer);
				};
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

			CObserver* onChange = new CObserver();

			// when check value change => update ui
			onChange->Notify = [me = onChange, target = comboBox, ui = layout](ISubject* subject, IObserver* from)
			{
				if (from != me)
				{
					CSubject<std::wstring>* value = (CSubject<std::wstring>*)subject;
					target->setLabel(value->get());
					ui->setHidden(!value->isEnable());
				}
			};

			// when combobox change => update value
			value->addObserver(onChange, true);
			comboBox->OnChanged = [value, comboBox, observer = onChange](GUI::CBase* base)
			{
				value->set(comboBox->getLabel());
				value->notify(observer);
			};

			comboBox->setLabel(value->get());

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

			CObserver* onChange = new CObserver();

			// when check value change => update ui
			onChange->Notify = [me = onChange, target = slider, ui = layout](ISubject* subject, IObserver* from)
			{
				if (from != me)
				{
					CSubject<float>* value = (CSubject<float>*)subject;
					target->setValue(value->get(), false);
					ui->setHidden(!value->isEnable());
				}
			};

			// when slider change => update value
			value->addObserver(onChange, true);
			slider->OnTextChanged = [value, slider, observer = onChange](GUI::CBase* base) {
				value->set(slider->getValue());
				value->notify(observer);
			};

			boxLayout->endVertical();
		}

		void CSpaceProperty::addColorPicker(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<SColor>* value)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			const SColor& c = value->get();
			GUI::CColorPicker* colorPicker = new GUI::CColorPicker(layout);
			colorPicker->setColor(GUI::SGUIColor(c.getAlpha(), c.getRed(), c.getGreen(), c.getBlue()));

			// when check value change => update ui
			CObserver* onChange = new CObserver();
			onChange->Notify = [me = onChange, target = colorPicker, ui = layout](ISubject* subject, IObserver* from)
			{
				if (from != me)
				{
					CSubject<SColor>* value = (CSubject<SColor>*)subject;
					const SColor& c = value->get();
					target->setColor(GUI::SGUIColor(c.getAlpha(), c.getRed(), c.getGreen(), c.getBlue()));
					ui->setHidden(!value->isEnable());
				}
			};

			// when color pick change => update value
			value->addObserver(onChange, true);
			colorPicker->OnChanged = [value, colorPicker, observer = onChange](GUI::CBase* base) {
				const GUI::SGUIColor& guiColor = colorPicker->getColor();
				value->set(SColor(guiColor.A, guiColor.R, guiColor.G, guiColor.B));
				value->notify(observer);
			};

			boxLayout->endVertical();
		}

		void CSpaceProperty::addInputFile(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<std::string>* value, const std::vector<std::string>& exts)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CTextBox* input = new GUI::CTextBox(layout);
			input->showIcon(GUI::ESystemIcon::File);
			input->setEditable(false);

			std::string fileName = CPath::getFileName(value->get());
			if (fileName.size() > 0)
				input->setString(CStringImp::convertUTF8ToUnicode(fileName.c_str()));
			else
				input->setString(L"None");

			input->OnPressed = [value](GUI::CBase* base)
			{
				std::string path = value->get();
				if (!path.empty())
					CAssetPropertyController::getInstance()->browseAsset(path.c_str());
			};

			input->OnAcceptDragDrop = [&, x = exts](GUI::SDragDropPackage* data)
			{
				if (data->Name == "ListFSItem")
				{
					GUI::CListRowItem* rowItem = (GUI::CListRowItem*)data->UserData;
					bool isFolder = rowItem->getTagBool();
					if (isFolder)
						return false;

					if (x.size() == 0)
					{
						// Enable all ext
						return true;
					}
					else
					{
						// Check ext
						std::string path = rowItem->getTagString();
						std::string fileExt = CPath::getFileNameExt(path);
						for (const std::string& s : x)
						{
							if (s == fileExt)
								return true;
						}
						return false;
					}
				}
				return false;
			};

			// when check value change => update ui
			CObserver* onChange = new CObserver();
			onChange->Notify = [me = onChange, target = input, ui = layout](ISubject* subject, IObserver* from)
			{
				CSubject<std::string>* value = (CSubject<std::string>*)subject;
				std::string fileName = CPath::getFileName(value->get());
				if (fileName.size() > 0)
					target->setString(CStringImp::convertUTF8ToUnicode(fileName.c_str()));
				else
					target->setString(L"None");

				ui->setHidden(!value->isEnable());
			};

			// when ui update => change value
			value->addObserver(onChange, true);
			input->OnDrop = [s = value, observer = onChange](GUI::SDragDropPackage* data, float mouseX, float mouseY)
			{
				if (data->Name == "ListFSItem")
				{
					GUI::CListRowItem* rowItem = (GUI::CListRowItem*)data->UserData;
					std::string path = rowItem->getTagString();
					path = CAssetManager::getInstance()->getShortPath(path.c_str());
					s->set(path);
					s->notify(observer);
				}
			};
			boxLayout->endVertical();
		}

		void CSpaceProperty::addInputFolder(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<std::string>* value)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CTextBox* input = new GUI::CTextBox(layout);
			input->showIcon(GUI::ESystemIcon::Folder);
			input->setEditable(false);

			std::string fileName = CPath::getFileName(value->get());
			if (fileName.size() > 0)
				input->setString(CStringImp::convertUTF8ToUnicode(fileName.c_str()));
			else
				input->setString(L"None");

			input->OnPressed = [value](GUI::CBase* base)
			{
				std::string path = value->get();
				if (!path.empty())
					CAssetPropertyController::getInstance()->browseAsset(path.c_str());
			};

			input->OnAcceptDragDrop = [&](GUI::SDragDropPackage* data)
			{
				if (data->Name == "ListFSItem")
				{
					GUI::CListRowItem* rowItem = (GUI::CListRowItem*)data->UserData;
					bool isFolder = rowItem->getTagBool();
					if (isFolder)
						return true;
					return false;
				}
				else if (data->Name == "TreeFSItem")
				{
					return true;
				}
				return false;
			};

			// when check value change => update ui
			CObserver* onChange = new CObserver();
			onChange->Notify = [me = onChange, target = input, ui = layout](ISubject* subject, IObserver* from)
			{
				CSubject<std::string>* value = (CSubject<std::string>*)subject;
				std::string fileName = CPath::getFileName(value->get());
				if (fileName.size() > 0)
					target->setString(CStringImp::convertUTF8ToUnicode(fileName.c_str()));
				else
					target->setString(L"None");

				ui->setHidden(!value->isEnable());
			};

			value->addObserver(onChange, true);
			input->OnDrop = [s = value, observer = onChange](GUI::SDragDropPackage* data, float mouseX, float mouseY)
			{
				if (data->Name == "ListFSItem")
				{
					GUI::CListRowItem* rowItem = (GUI::CListRowItem*)data->UserData;
					std::string path = rowItem->getTagString();
					path = CAssetManager::getInstance()->getShortPath(path.c_str());
					s->set(path);
					s->notify(observer);
				}
				else if (data->Name == "TreeFSItem")
				{
					GUI::CTreeNode* node = (GUI::CTreeNode*)data->UserData;
					std::string path = node->getTagString();
					path = CAssetManager::getInstance()->getShortPath(path.c_str());
					s->set(path);
					s->notify(observer);
				}
			};
			boxLayout->endVertical();
		}

		GUI::CImageButton* CSpaceProperty::addInputTextureFile(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<std::string>* value)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CBase* container = new GUI::CBase(layout);
			container->setSize(64.0f, 64.0f);

			GUI::CImageButton* image = new GUI::CImageButton(container);
			image->setSize(64, 64);
			image->dock(GUI::EPosition::Left);
			image->getImage()->setColor(GUI::SGUIColor(255, 50, 50, 50));

			boxLayout->endVertical();
			return image;
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

		GUI::CCollapsibleGroup* CSpaceProperty::addSubGroup(GUI::CBoxLayout* boxLayout)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();
			GUI::CCollapsibleGroup* groupControl = new GUI::CCollapsibleGroup(layout);
			boxLayout->endVertical();

			return groupControl;
		}

		const wchar_t* CSpaceProperty::getPrettyName(const std::string& name)
		{
			char prettyName[512] = { 0 };
			int j = 0;
			bool lastCharIsUpper = false;

			for (size_t i = 0, j = 0, n = name.length(); i < n; i++, j++)
			{
				char c = name[i];

				if (i == 0)
				{
					prettyName[j] = toupper(c);
				}
				else if (isupper(c) && !lastCharIsUpper)
				{
					prettyName[j++] = ' ';
					prettyName[j] = c;
				}
				else
				{
					prettyName[j] = c;
				}

				lastCharIsUpper = isupper(c);
			}

			m_tempName = CStringImp::convertUTF8ToUnicode(prettyName);
			return m_tempName.c_str();
		}
	}
}