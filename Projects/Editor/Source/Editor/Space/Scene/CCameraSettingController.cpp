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
#include "CCameraSettingController.h"
#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		CCameraSettingController::CCameraSettingController(CEditor* editor, GUI::CMenu* menu) :
			m_menu(menu),
			m_editor(editor)
		{
			GUI::CBoxLayout* boxLayout = new GUI::CBoxLayout(menu);
			boxLayout->setPadding(GUI::SPadding(5.0, 5.0, -5.0, 5.0));
			boxLayout->dock(GUI::EPosition::Top);

			GUI::CLayout* layout;
			GUI::CLabel* label;

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setString(L"Editor Camera");
			label->setTextAlignment(GUI::TextLeft);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Speed");
			label->setTextAlignment(GUI::TextRight);

			m_moveSpeed = new GUI::CSlider(layout);
			m_moveSpeed->setValue(2.0f, 0.1f, 5.0f, false);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Speed multiply");
			label->setTextAlignment(GUI::TextRight);

			m_speedMultiply = new GUI::CSlider(layout);
			m_speedMultiply->setValue(1.0f, 0.1f, 2.0f, false);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("FOV");
			label->setTextAlignment(GUI::TextRight);

			m_fov = new GUI::CSlider(layout);
			m_fov->setValue(60.0f, 1.0f, 90.0f, false);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Near");
			label->setTextAlignment(GUI::TextRight);

			m_near = new GUI::CSlider(layout);
			m_near->setValue(0.05f, 0.01f, 10.0f, false);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Far");
			label->setTextAlignment(GUI::TextRight);

			m_far = new GUI::CSlider(layout);
			m_far->setValue(1500.0f, 20.0f, 5000.0f, false);
			boxLayout->endVertical();

			boxLayout->addSpace(20.0f);

			layout = boxLayout->beginVertical();
			GUI::CButton* defaultBtn = new GUI::CButton(layout);
			defaultBtn->setLabel(L"Default");
			defaultBtn->setTextAlignment(GUI::TextCenter);
			defaultBtn->OnPress = BIND_LISTENER(&CCameraSettingController::onDefault, this);

			GUI::CButton* okBtn = new GUI::CButton(layout);
			okBtn->setLabel(L"OK");
			okBtn->setTextAlignment(GUI::TextCenter);
			okBtn->OnPress = BIND_LISTENER(&CCameraSettingController::onOK, this);
			boxLayout->endVertical();
		}

		CCameraSettingController::~CCameraSettingController()
		{

		}

		void CCameraSettingController::update()
		{

		}

		void CCameraSettingController::onShow()
		{
			CSpaceScene* spaceScene = CSceneController::getInstance()->getSpaceScene();
			CCamera* editorCamera = spaceScene->getEditorCamera();
		}

		void CCameraSettingController::onDefault(GUI::CBase* base)
		{
			m_menu->close();
		}

		void CCameraSettingController::onOK(GUI::CBase* base)
		{
			m_menu->close();
		}
	}
}