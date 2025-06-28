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
#include "Editor/SpaceController/CGUIDesignController.h"
#include "GUI/Theme/ThemeConfig.h"
#include "Editor/Components/Transform/CTransformEditor.h"
#include "Editor/EntityData/CEntityDataEditor.h"
#include "Entity/CEntityHandleData.h"

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

			m_content->setKeyboardInputEnabled(true);
			m_content->addAccelerator("Ctrl + Z", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Z"); });
			m_content->addAccelerator("Ctrl + Y", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Y"); });

			GUI::CBase* titleBar = new GUI::CBase(m_content);
			titleBar->setHeight(25.0f);
			titleBar->dock(GUI::EPosition::Top);
			titleBar->setPadding(GUI::SPadding(5.0f, 3.0f, -5.0f, -3.0f));
			titleBar->enableRenderFillRect(true);
			titleBar->setFillRectColor(GUI::ThemeConfig::WindowBackgroundColor);

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
			m_addComponentController = new CAddComponentController(editor, this, m_addComponentMenu);
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

		void CSpaceProperty::OnHotkey(GUI::CBase* base, const std::string& hotkey)
		{
			if (hotkey == "Ctrl + Z")
			{
				onUndo();
			}
			else if (hotkey == "Ctrl + Y")
			{
				onRedo();
			}
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

			onEndEditValue(base);
			focus();

			CSceneController::getInstance()->updateTreeNode(gameObject);

			getEditor()->refresh();
		}

		void CSpaceProperty::clearAllGroup()
		{
			for (SGroup* group : m_groups)
			{
				group->GroupUI->remove();

				if (group->Owner)
					group->Owner->closeGUI();

				delete group;
			}

			// delete editor components
			for (CComponentEditor* editor : m_releaseComponents)
				delete editor;
			m_releaseComponents.clear();
			m_groups.clear();
		}

		void CSpaceProperty::focus()
		{
			m_content->focus();
		}

		void CSpaceProperty::onEndEditValue(GUI::CBase* base)
		{
			int editorId = 0;
			bool changed = false;
			bool notifyTemplateChange = false;

			std::vector<CGameObject*> objs;
			std::vector<CGUIElement*> guis;

			for (SGroup* group : m_groups)
			{
				if (group->Owner)
				{
					CComponentEditor* componentEditor = dynamic_cast<CComponentEditor*>(group->Owner);
					CGUIEditor* guiEditor = dynamic_cast<CGUIEditor*>(group->Owner);
					CEntityDataEditor* entityDataEditor = dynamic_cast<CEntityDataEditor*>(group->Owner);

					if (componentEditor)
					{
						// need save history
						CGameObject* obj = componentEditor->getGameObject();
						if (std::find(objs.begin(), objs.end(), obj) == objs.end())
							objs.push_back(obj);

						changed = true;
						editorId = 1;

						// check component (that is not transform) is changed
						if (componentEditor->isChanged())
						{
							CTransformEditor* transformEditor = dynamic_cast<CTransformEditor*>(group->Owner);
							if (transformEditor == NULL)
								notifyTemplateChange = true;
						}
					}
					else if (guiEditor)
					{
						CGUIElement* element = guiEditor->getGUIElement();
						if (std::find(guis.begin(), guis.end(), element) == guis.end())
							guis.push_back(element);

						changed = true;
						editorId = 2;
					}
					else if (entityDataEditor)
					{
						IEntityData* data = entityDataEditor->getEntityData();
						CEntityHandleData* handler = data->Entity->getData<CEntityHandleData>();
						if (handler && handler->Handler)
						{
							CGameObject* obj = handler->Handler->getGameObject();
							if (std::find(objs.begin(), objs.end(), obj) == objs.end())
								objs.push_back(obj);

							changed = true;
							editorId = 1;
						}
					}
				}
			}

			if (editorId == 1)
			{
				if (changed && objs.size() > 0)
				{
					CSceneController::getInstance()->getHistory()->saveModifyHistory(objs);

					// notify the current template is changed
					for (CGameObject* gameObject : objs)
					{
						CGameObject* parent = gameObject->getParentTemplate();
						if (parent == gameObject)
						{
							// if template just change transform, we do not set modify
							if (notifyTemplateChange)
								parent->setTemplateChanged(true);
						}
						else
						{
							// if child is changed, we set modify
							parent->setTemplateChanged(true);
						}

						// update on hierachy
						CSpaceHierarchy* hierachy = CSceneController::getInstance()->getSpaceHierarchy();
						if (hierachy)
							hierachy->getController()->updateObjectToUI(parent);
					}
				}
			}
			else if (editorId == 2)
			{
				if (changed && guis.size() > 0)
				{
					CGUIDesignController::getInstance()->getHistory()->saveModifyHistory(guis);
				}
			}
		}

		void CSpaceProperty::onUndo()
		{
			for (SGroup* group : m_groups)
			{
				if (group->Owner)
				{
					CComponentEditor* componentEditor = dynamic_cast<CComponentEditor*>(group->Owner);
					CGUIEditor* guiEditor = dynamic_cast<CGUIEditor*>(group->Owner);
					CEntityDataEditor* entityDataEditor = dynamic_cast<CEntityDataEditor*>(group->Owner);

					if (componentEditor || entityDataEditor)
					{
						CSceneController::getInstance()->onUndo();
						break;
					}
					else if (guiEditor)
					{
						CGUIDesignController::getInstance()->onUndo();
						break;
					}
				}
			}
			focus();
		}

		void CSpaceProperty::onRedo()
		{
			for (SGroup* group : m_groups)
			{
				if (group->Owner)
				{
					CComponentEditor* componentEditor = dynamic_cast<CComponentEditor*>(group->Owner);
					CGUIEditor* guiEditor = dynamic_cast<CGUIEditor*>(group->Owner);
					CEntityDataEditor* entityDataEditor = dynamic_cast<CEntityDataEditor*>(group->Owner);

					if (componentEditor || entityDataEditor)
					{
						CSceneController::getInstance()->onRedo();
						break;
					}
					else if (guiEditor)
					{
						CGUIDesignController::getInstance()->onRedo();
						break;
					}
				}
			}
			focus();
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

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const wchar_t* label, IPropertyEditor* editor)
		{
			GUI::CCollapsibleGroup* colapsible = new GUI::CCollapsibleGroup(m_content);
			colapsible->dock(GUI::EPosition::Top);
			colapsible->getHeader()->setLabel(label);

			SGroup* g = new SGroup();
			g->Owner = editor;
			g->GroupUI = colapsible;

			CComponentEditor* componentEditor = dynamic_cast<CComponentEditor*>(editor);
			if (componentEditor != NULL && componentEditor->getComponent() != NULL)
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
				btn->OnPress = [&, componentEditor](GUI::CBase* button)
					{
						CGameObject* object = componentEditor->getGameObject();
						CComponentSystem* component = componentEditor->getComponent();

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

						m_menuContextEditor = componentEditor;
						m_componentContextMenu->open(button);
					};
			}

			m_groups.push_back(g);
			return colapsible;
		}

		GUI::CCollapsibleGroup* CSpaceProperty::addGroup(const char* label, IPropertyEditor* editor)
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

		GUI::CButton* CSpaceProperty::addButton(GUI::CLayout* layout, const wchar_t* label)
		{
			GUI::CButton* button = new GUI::CButton(layout);
			button->setLabel(label);
			button->setTextAlignment(GUI::TextCenter);
			return button;
		}

		GUI::CButton* CSpaceProperty::addButton(const wchar_t* label)
		{
			GUI::CBase* content = new GUI::CBase(m_content);
			content->setHeight(45);
			content->setMargin(GUI::SMargin(0.0f, 10.0f, 0.0f, 0.0f));
			content->dock(GUI::EPosition::Top);
			content->setFillRectColor(GUI::ThemeConfig::WindowBackgroundColor);
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
							if (GUI::Context::KeyboardFocus != target)
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

				input->OnEndTextEdit = BIND_LISTENER(&CSpaceProperty::onEndEditValue, this);
				input->addAccelerator("Ctrl + Z", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Z"); });
				input->addAccelerator("Ctrl + Y", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Y"); });
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
							if (GUI::Context::KeyboardFocus != target)
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

				input->OnEndTextEdit = BIND_LISTENER(&CSpaceProperty::onEndEditValue, this);
				input->addAccelerator("Ctrl + Z", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Z"); });
				input->addAccelerator("Ctrl + Y", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Y"); });
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
							if (GUI::Context::KeyboardFocus != target)
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

				input->OnEndTextEdit = BIND_LISTENER(&CSpaceProperty::onEndEditValue, this);
				input->addAccelerator("Ctrl + Z", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Z"); });
				input->addAccelerator("Ctrl + Y", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Y"); });
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

				input->OnEndTextEdit = BIND_LISTENER(&CSpaceProperty::onEndEditValue, this);
				input->addAccelerator("Ctrl + Z", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Z"); });
				input->addAccelerator("Ctrl + Y", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Y"); });
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
							if (GUI::Context::KeyboardFocus != target)
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

				input->OnEndTextEdit = BIND_LISTENER(&CSpaceProperty::onEndEditValue, this);
				input->addAccelerator("Ctrl + Z", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Z"); });
				input->addAccelerator("Ctrl + Y", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Y"); });
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
				check->OnChanged = [&, value, check, observer = onChange](GUI::CBase* base) {
					value->set(check->getToggle());
					value->notify(observer);

					onEndEditValue(check);
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
			comboBox->OnChanged = [&, value, comboBox, observer = onChange](GUI::CBase* base)
				{
					value->set(comboBox->getLabel());
					value->notify(observer);

					onEndEditValue(comboBox);
					focus();
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

			slider->OnEndTextEdit = BIND_LISTENER(&CSpaceProperty::onEndEditValue, this);
			slider->addAccelerator("Ctrl + Z", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Z"); });
			slider->addAccelerator("Ctrl + Y", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Y"); });

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
			colorPicker->OnChanged = [&, value, colorPicker, observer = onChange](GUI::CBase* base) {
				const GUI::SGUIColor& guiColor = colorPicker->getColor();
				value->set(SColor(guiColor.A, guiColor.R, guiColor.G, guiColor.B));
				value->notify(observer);
				};

			colorPicker->OnEndValue = [&](GUI::CBase* base) {
				onEndEditValue(base);
				focus();
				};

			boxLayout->endVertical();
		}

		void CSpaceProperty::addInterpolateCurves(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<CInterpolator>* value)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			CInterpolator interpolator = value->get();
			GUI::CInterpolateCurvesButton* curvesBtn = new GUI::CInterpolateCurvesButton(layout, interpolator);

			// when check value change => update ui
			CObserver* onChange = new CObserver();
			onChange->Notify = [me = onChange, target = curvesBtn, ui = layout](ISubject* subject, IObserver* from)
				{
					if (from != me)
					{
						CSubject<CInterpolator>* value = (CSubject<CInterpolator>*)subject;
						target->setInterpolator(value->get());
						ui->setHidden(!value->isEnable());
					}
				};

			// when update interpolate change => update value
			value->addObserver(onChange, true);
			curvesBtn->OnChanged = [&, value, curvesBtn, observer = onChange](GUI::CBase* base) {
				value->set(curvesBtn->getInterpolator());
				value->notify(observer);
				};

			curvesBtn->OnEndValue = BIND_LISTENER(&CSpaceProperty::onEndEditValue, this);
			curvesBtn->addAccelerator("Ctrl + Z", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Z"); });
			curvesBtn->addAccelerator("Ctrl + Y", [&](GUI::CBase* base) {this->OnHotkey(base, "Ctrl + Y"); });

			boxLayout->endVertical();
		}

		GUI::CBoxLayout* CSpaceProperty::addChildGroup(GUI::CBoxLayout* boxLayout, const wchar_t* name, bool defaultExpand)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CCollapsibleGroup* colapsible = new GUI::CCollapsibleGroup(layout);
			colapsible->dock(GUI::EPosition::Top);
			colapsible->getHeader()->setLabel(name);
			colapsible->getHeader()->enableDrawBackground(false);
			colapsible->setExpand(defaultExpand);

			GUI::CBoxLayout* childBoxLayout = new GUI::CBoxLayout(colapsible);

			boxLayout->endVertical();

			return childBoxLayout;
		}

		GUI::CInputResourceBox* CSpaceProperty::addInputFile(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<std::string>* value, const std::vector<std::string>& exts)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CInputResourceBox* input = new GUI::CInputResourceBox(layout);
			input->showIcon(GUI::ESystemIcon::File);

			std::string fileName = CPath::getFileName(value->get());
			if (fileName.size() > 0)
				input->setString(CStringImp::convertUTF8ToUnicode(fileName.c_str()));
			else
			{
				std::string extString = "None (";
				for (int i = 0, n = (int)exts.size(); i < n; i++)
				{
					extString += ".";
					extString += exts[i];
					if (i < n - 1)
						extString += ", ";
				}
				extString += ")";
				input->setString(CStringImp::convertUTF8ToUnicode(extString.c_str()));
			}

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
			input->OnDrop = [&, target = input, s = value, observer = onChange](GUI::SDragDropPackage* data, float mouseX, float mouseY)
				{
					if (data->Name == "ListFSItem")
					{
						GUI::CListRowItem* rowItem = (GUI::CListRowItem*)data->UserData;
						std::string path = rowItem->getTagString();
						path = CAssetManager::getInstance()->getShortPath(path.c_str());
						s->set(path);
						s->notify(observer);

						onEndEditValue(target);
					}
				};

			input->OnEndTextEdit = BIND_LISTENER(&CSpaceProperty::onEndEditValue, this);

			boxLayout->endVertical();

			return input;
		}

		GUI::CInputResourceBox* CSpaceProperty::addInputFolder(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<std::string>* value)
		{
			GUI::CLayout* layout = boxLayout->beginVertical();

			GUI::CLabel* label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString(name);
			label->setTextAlignment(GUI::TextRight);

			GUI::CInputResourceBox* input = new GUI::CInputResourceBox(layout);
			input->showIcon(GUI::ESystemIcon::Folder);

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
			input->OnDrop = [&, target = input, s = value, observer = onChange](GUI::SDragDropPackage* data, float mouseX, float mouseY)
				{
					if (data->Name == "ListFSItem")
					{
						GUI::CListRowItem* rowItem = (GUI::CListRowItem*)data->UserData;
						std::string path = rowItem->getTagString();
						path = CAssetManager::getInstance()->getShortPath(path.c_str());
						s->set(path);
						s->notify(observer);

						onEndEditValue(target);
					}
					else if (data->Name == "TreeFSItem")
					{
						GUI::CTreeNode* node = (GUI::CTreeNode*)data->UserData;
						std::string path = node->getTagString();
						path = CAssetManager::getInstance()->getShortPath(path.c_str());
						s->set(path);
						s->notify(observer);

						onEndEditValue(target);
					}
				};

			input->OnEndTextEdit = BIND_LISTENER(&CSpaceProperty::onEndEditValue, this);
			boxLayout->endVertical();

			return input;
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