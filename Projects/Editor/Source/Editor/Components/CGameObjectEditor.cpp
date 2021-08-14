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
			ui->addCheckBox(layout, L"Enable", &Enable);
			ui->addCheckBox(layout, L"Visible", &Visible);
			ui->addCheckBox(layout, L"Static", &Static);

			Name.addObserver(new CObserver<CGameObject>(object,
				[](ISubject* subject, IObserver* from, CGameObject* target)
				{
					CSubject<std::wstring>* value = (CSubject<std::wstring>*) subject;
					target->setName(value->get().c_str());
				}), true);

			Enable.addObserver(new CObserver<CGameObject>(object,
				[](ISubject* subject, IObserver* from, CGameObject* target)
				{
					CSubject<bool>* value = (CSubject<bool>*) subject;
					target->setEnable(value->get());
				}), true);

			Visible.addObserver(new CObserver<CGameObject>(object,
				[](ISubject* subject, IObserver* from, CGameObject* target)
				{
					CSubject<bool>* value = (CSubject<bool>*) subject;
					target->setVisible(value->get());
				}), true);

			Static.addObserver(new CObserver<CGameObject>(object,
				[](ISubject* subject, IObserver* from, CGameObject* target)
				{
					CSubject<bool>* value = (CSubject<bool>*) subject;
					target->setStatic(value->get());
				}), true);

			group->setExpand(true);
		}

		void CGameObjectEditor::update()
		{

		}

		EDITOR_REGISTER(CGameObjectEditor, CGameObject)
	}
}