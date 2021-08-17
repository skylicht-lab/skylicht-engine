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
#include "Editor/Space/Property/CSpaceProperty.h"
#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		CGameObjectEditor::CGameObjectEditor() :
			Name(L""),
			Enable(true),
			Visible(true),
			Static(false)
		{

		}

		CGameObjectEditor::~CGameObjectEditor()
		{

		}

		void CGameObjectEditor::initGUI(CGameObject* object, CSpaceProperty* ui)
		{
			Name.removeAllObserver();
			Enable.removeAllObserver();
			Visible.removeAllObserver();
			Static.removeAllObserver();

			Name = object->getName();
			Enable = object->isEnable();
			Visible = object->isVisible();
			Static = object->isStatic();

			GUI::CCollapsibleGroup* group = ui->addGroup("GameObject", this);
			GUI::CBoxLayout* layout = ui->createBoxLayout(group);

			ui->addTextBox(layout, L"Name", &Name);

			initObjectLayerMenu(ui->addDropBox(layout, L"Culling", L"Default"));

			ui->addCheckBox(layout, L"Enable", &Enable);
			ui->addCheckBox(layout, L"Visible", &Visible);
			ui->addCheckBox(layout, L"Static", &Static);

			Name.addObserver(new CObserver<CGameObject>(object,
				[me = this](ISubject* subject, IObserver* from, CGameObject* target)
				{
					CSubject<std::wstring>* value = (CSubject<std::wstring>*) subject;
					target->setName(value->get().c_str());
					CSelection::getInstance()->notify(target, me);
				}), true);

			CSelectObject* selectObject = CSelection::getInstance()->getSelected(object);
			if (selectObject != NULL)
			{
				selectObject->addObserver(new CObserver<CGameObject>(object,
					[n = &Name, me = this](ISubject* subject, IObserver* from, CGameObject* target) {
						if (from != me)
						{
							n->set(target->getName());
							n->notify(from);
						}
					}), true);
			}

			Enable.addObserver(new CObserver<CGameObject>(object,
				[me = this](ISubject* subject, IObserver* from, CGameObject* target)
				{
					CSubject<bool>* value = (CSubject<bool>*) subject;
					target->setEnable(value->get());
					CSelection::getInstance()->notify(target, me);
				}), true);

			Visible.addObserver(new CObserver<CGameObject>(object,
				[me = this](ISubject* subject, IObserver* from, CGameObject* target)
				{
					CSubject<bool>* value = (CSubject<bool>*) subject;
					target->setVisible(value->get());
					CSelection::getInstance()->notify(target, me);
				}), true);

			Static.addObserver(new CObserver<CGameObject>(object,
				[me = this](ISubject* subject, IObserver* from, CGameObject* target)
				{
					CSubject<bool>* value = (CSubject<bool>*) subject;
					target->setStatic(value->get());
					CSelection::getInstance()->notify(target, me);
				}), true);

			group->setExpand(true);
		}

		void CGameObjectEditor::update()
		{

		}

		void CGameObjectEditor::onNotify(ISubject* subject, IObserver* from)
		{

		}

		void CGameObjectEditor::initObjectLayerMenu(GUI::CDropdownBox* dropDown)
		{
			GUI::CMenu* menu = dropDown->getMenu();

			GUI::CBoxLayout* boxLayout = new GUI::CBoxLayout(menu);
			boxLayout->dock(GUI::EPosition::Top);
			boxLayout->setPadding(GUI::SPadding(0.0, 5.0, 0.0, 5.0));

			for (int i = 0; i < 16; i++)
			{
				GUI::CLayoutColumn* row = boxLayout->beginColumn();
				row->enableRenderHover(true);

				GUI::CCheckBox* check = new GUI::CCheckBox(row);
				check->setToggle(true);
				check->setPadding(GUI::SPadding(4.0f, 1.0f));

				GUI::CLabel* label = new GUI::CLabel(row);
				label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));

				wchar_t labelw[32];
				swprintf(labelw, 32, L"%d", i + 1);
				label->setString(labelw);
				label->setTextAlignment(GUI::TextLeft);
				boxLayout->endColumn();
			}
		}

		EDITOR_REGISTER(CGameObjectEditor, CGameObject)
	}
}