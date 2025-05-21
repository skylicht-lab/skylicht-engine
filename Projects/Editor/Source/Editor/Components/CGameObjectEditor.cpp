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
#include "CGameObjectEditor.h"
#include "Selection/CSelection.h"
#include "ProjectSettings/CProjectSettings.h"
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/CEditor.h"
#include "Editor/SpaceController/CSceneController.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		EDITOR_REGISTER(CGameObjectEditor, CGameObject);
		EDITOR_REGISTER(CGameObjectEditor, CContainerObject);

		CGameObjectEditor::CGameObjectEditor() :
			Name(L""),
			Enable(true),
			Visible(true),
			Static(false),
			Lock(false),
			SelectObject(NULL),
			Observer(NULL)
		{

		}

		CGameObjectEditor::~CGameObjectEditor()
		{

		}

		void CGameObjectEditor::closeGUI()
		{
			if (CSelection::getInstance()->isObjectAvaiable(SelectObject))
			{
				if (SelectObject && Observer)
					SelectObject->removeObserver(Observer);
			}

			SelectObject = NULL;
			Observer = NULL;
		}

		void CGameObjectEditor::initGUI(CGameObject* object, CSpaceProperty* ui)
		{
			m_gameObject = object;

			Name.removeAllObserver();
			Enable.removeAllObserver();
			Visible.removeAllObserver();
			Static.removeAllObserver();
			Lock.removeAllObserver();

			Name = object->getName();
			Enable = object->isEnable();
			Visible = object->isVisible();
			Static = object->isStatic();
			Lock = object->isSelfLock();

			GUI::CCollapsibleGroup* group = ui->addGroup("GameObject", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);

			ui->addTextBox(layout, L"Name", &Name);

			initObjectLayerMenu(object, ui->addDropBox(layout, L"Culling", L""), ui);

			ui->addCheckBox(layout, L"Enable", &Enable);
			ui->addCheckBox(layout, L"Visible", &Visible);
			ui->addCheckBox(layout, L"Static", &Static);
			ui->addCheckBox(layout, L"Lock", &Lock);

			Name.addObserver(new CObserver([&, target = object](ISubject* subject, IObserver* from)
				{
					CSubject<std::wstring>* value = (CSubject<std::wstring>*) subject;
					target->setName(value->get().c_str());
					CSelection::getInstance()->notify(target, this);
				}), true);


			SelectObject = CSelection::getInstance()->getSelected(object);
			if (SelectObject != NULL)
			{
				Observer = new CObserver([&, n = &Name, target = object](ISubject* subject, IObserver* from)
					{
						if (from != this)
						{
							n->set(target->getName());
							n->notify(from);
						}
					});
				SelectObject->addObserver(Observer, true);
			}

			Enable.addObserver(new CObserver([&, target = object](ISubject* subject, IObserver* from)
				{
					CSubject<bool>* value = (CSubject<bool>*) subject;
					target->setEnable(value->get());
					CSelection::getInstance()->notify(target, this);

					// apply for all selected
					std::vector<CGameObject*> selected = getSelected();
					for (CGameObject* obj : selected)
					{
						if (obj != object)
						{
							obj->setEnable(value->get());
							CSelection::getInstance()->notify(obj, this);
						}
					}
				}), true);

			Visible.addObserver(new CObserver([&, target = object](ISubject* subject, IObserver* from)
				{
					CSubject<bool>* value = (CSubject<bool>*) subject;
					target->setVisible(value->get());
					CSelection::getInstance()->notify(target, this);

					// apply for all selected
					std::vector<CGameObject*> selected = getSelected();
					for (CGameObject* obj : selected)
					{
						if (obj != object)
						{
							obj->setVisible(value->get());
							CSelection::getInstance()->notify(obj, this);
						}
					}
				}), true);

			Static.addObserver(new CObserver([&, target = object](ISubject* subject, IObserver* from)
				{
					CSubject<bool>* value = (CSubject<bool>*) subject;
					target->setStatic(value->get());
					CSelection::getInstance()->notify(target, this);

					// apply for all selected
					std::vector<CGameObject*> selected = getSelected();
					for (CGameObject* obj : selected)
					{
						if (obj != object)
						{
							obj->setStatic(value->get());
							CSelection::getInstance()->notify(obj, this);
						}
					}
				}), true);

			Lock.addObserver(new CObserver([&, target = object](ISubject* subject, IObserver* from)
				{
					CSubject<bool>* value = (CSubject<bool>*) subject;
					target->setLock(value->get());
					CSelection::getInstance()->notify(target, this);

					// apply for all selected
					std::vector<CGameObject*> selected = getSelected();
					for (CGameObject* obj : selected)
					{
						if (obj != object)
						{
							obj->setLock(value->get());
							CSelection::getInstance()->notify(obj, this);
						}
					}
				}), true);

			group->setExpand(true);
		}

		void CGameObjectEditor::update()
		{

		}

		void CGameObjectEditor::onNotify(ISubject* subject, IObserver* from)
		{

		}

		void CGameObjectEditor::initObjectLayerMenu(CGameObject* object, GUI::CDropdownBox* dropDown, CSpaceProperty* ui)
		{
			GUI::CMenu* menu = dropDown->getMenu();

			wchar_t labelw[256];
			CObjectLayer* objectLayer = CProjectSettings::getInstance()->getObjectLayer();

			for (int i = 0; i < 16; i++)
			{
				std::wstring name = CStringImp::convertUTF8ToUnicode(objectLayer->getName(i).c_str());
				CStringImp::copy(labelw, name.c_str());
				CStringImp::trim(labelw);

				GUI::CMenuItem* item = NULL;

				if (CStringImp::length(labelw) > 0)
					item = menu->addItem(labelw);
				else
				{
					swprintf(labelw, 256, L"Layer: %d", i);
					item = menu->addItem(labelw);
				}

				u32 value = (1 << i);
				if (object->getCullingLayer() == value)
				{
					item->setIcon(GUI::ESystemIcon::Check);
					dropDown->setLabel(labelw);
				}
				else
				{
					item->setIcon(GUI::ESystemIcon::None);
				}

				item->OnPress = [&, object, item, value, dropDown, ui](GUI::CBase* base)
					{
						// uncheck all menu item
						GUI::CMenu* menu = dropDown->getMenu();
						for (GUI::CBase* childMenu : menu->getChildren())
						{
							GUI::CMenuItem* item = dynamic_cast<GUI::CMenuItem*>(childMenu);
							if (item != NULL)
								item->setIcon(GUI::ESystemIcon::None);
						}

						// check this item
						item->setIcon(GUI::ESystemIcon::Check);

						// apply culling
						object->setCullingLayer(value);

						// apply for all selected
						std::vector<CGameObject*> selected = getSelected();
						for (CGameObject* obj : selected)
						{
							if (obj != object)
								obj->setCullingLayer(value);
						}

						// apply value
						dropDown->setLabel(item->getLabel());

						// close menu
						ui->getWindow()->getCanvas()->closeMenu();
					};
			}

			menu->addSeparator();
			menu->addItem(L"Edit layer")->OnPress = [ui](GUI::CBase* base)
				{
					ui->getWindow()->getCanvas()->closeMenu();
					ui->getEditor()->showProjectSetting();
				};
		}

		std::vector<CGameObject*> CGameObjectEditor::getSelected()
		{
			std::vector<CGameObject*> result;

			CSceneController* sceneController = CSceneController::getInstance();
			CScene* scene = sceneController->getScene();

			CSelection* selection = CSelection::getInstance();
			std::vector<CSelectObject*>& selected = selection->getAllSelected();
			for (CSelectObject* sel : selected)
			{
				if (sel->getType() == CSelectObject::GameObject)
				{
					CGameObject* obj = scene->searchObjectInChildByID(sel->getID().c_str());
					if (obj)
						result.push_back(obj);
				}
			}

			return result;
		}
	}
}