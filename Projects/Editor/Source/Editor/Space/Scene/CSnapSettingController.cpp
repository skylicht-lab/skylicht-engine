/*
!@
MIT License

Copyright (c) 2022 Skylicht Technology CO., LTD

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
#include "CSnapSettingController.h"
#include "Editor/SpaceController/CSceneController.h"
#include "Handles/CHandles.h"

namespace Skylicht
{
	namespace Editor
	{
		CSnapSettingController::CSnapSettingController(CEditor* editor, GUI::CMenu* menu) :
			m_menu(menu),
			m_editor(editor)
		{
			m_menu->setWidth(270.0f);

			GUI::CBoxLayout* boxLayout = new GUI::CBoxLayout(menu);
			boxLayout->setPadding(GUI::SPadding(5.0, 5.0, -5.0, 5.0));
			boxLayout->dock(GUI::EPosition::Top);

			GUI::CLayout* layout;
			GUI::CLabel* label;

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Enable XZ");
			label->setTextAlignment(GUI::TextRight);
			m_enableXZ = new GUI::CCheckBox(new GUI::CBase(layout));
			m_enableXZ->OnChanged = BIND_LISTENER(&CSnapSettingController::onChanged, this);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Distance XZ (m)");
			label->setTextAlignment(GUI::TextRight);

			m_snapDistanceXZ = new GUI::CSlider(layout);
			m_snapDistanceXZ->setValue(1.0f, 0.0f, 10.0f, false);
			m_snapDistanceXZ->OnTextChanged = BIND_LISTENER(&CSnapSettingController::onChanged, this);
			boxLayout->endVertical();

			boxLayout->addSpace(5.0f);

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Enable Y");
			label->setTextAlignment(GUI::TextRight);
			m_enableY = new GUI::CCheckBox(new GUI::CBase(layout));
			m_enableY->OnChanged = BIND_LISTENER(&CSnapSettingController::onChanged, this);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Distance Y (m)");
			label->setTextAlignment(GUI::TextRight);

			m_snapDistanceY = new GUI::CSlider(layout);
			m_snapDistanceY->setValue(1.0f, 0.0f, 10.0f, false);
			m_snapDistanceY->OnTextChanged = BIND_LISTENER(&CSnapSettingController::onChanged, this);
			boxLayout->endVertical();

			boxLayout->addSpace(5.0f);

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Enable Rotate");
			label->setTextAlignment(GUI::TextRight);
			m_enableRotate = new GUI::CCheckBox(new GUI::CBase(layout));
			m_enableRotate->OnChanged = BIND_LISTENER(&CSnapSettingController::onChanged, this);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Snap Rotate (deg)");
			label->setTextAlignment(GUI::TextRight);

			m_snapRotateDeg = new GUI::CSlider(layout);
			m_snapRotateDeg->setValue(10.0f, 0.0f, 180.0f, false);
			m_snapRotateDeg->OnTextChanged = BIND_LISTENER(&CSnapSettingController::onChanged, this);
			boxLayout->endVertical();
		}

		CSnapSettingController::~CSnapSettingController()
		{

		}

		void CSnapSettingController::update()
		{

		}

		void CSnapSettingController::onShow()
		{
			CSpaceScene* spaceScene = CSceneController::getInstance()->getSpaceScene();
			CHandles* handles = CHandles::getInstance();

			m_enableXZ->setIsToggle(handles->isSnapXZ());
			m_enableY->setIsToggle(handles->isSnapY());
			m_enableRotate->setIsToggle(handles->isSnapRotate());

			m_snapDistanceXZ->setValue(handles->getSnapDistanceXZ(), true);
			m_snapDistanceY->setValue(handles->getSnapDistanceXZ(), true);
			m_snapRotateDeg->setValue(handles->getSnapRotateDeg(), true);
		}

		void CSnapSettingController::onChanged(GUI::CBase* base)
		{
			CSpaceScene* spaceScene = CSceneController::getInstance()->getSpaceScene();
			CHandles* handles = CHandles::getInstance();

			handles->snapXZ(m_enableXZ->getToggle());
			handles->snapY(m_enableY->getToggle());
			handles->snapRotate(m_enableRotate->getToggle());

			handles->setSnapDistanceXZ(m_snapDistanceXZ->getValue());
			handles->setSnapDistanceY(m_snapDistanceY->getValue());
			handles->setSnapRotateDeg(m_snapRotateDeg->getValue());

			spaceScene->updateSnapButton();
		}
	}
}