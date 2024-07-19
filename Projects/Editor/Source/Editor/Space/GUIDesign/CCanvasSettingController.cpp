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
#include "CCanvasSettingController.h"
#include "Editor/SpaceController/CSceneController.h"

namespace Skylicht
{
	namespace Editor
	{
		CCanvasSettingController::CCanvasSettingController(CEditor* editor, GUI::CMenu* menu) :
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
			label->setString(L"Canvas setting");
			label->setTextAlignment(GUI::TextLeft);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Width");
			label->setTextAlignment(GUI::TextRight);

			m_inputWidth = new GUI::CNumberInput(layout);
			m_inputWidth->setNumberType(GUI::UInteger);
			m_inputWidth->setValue(1920.0f, false);
			m_inputWidth->OnTextChanged = BIND_LISTENER(&CCanvasSettingController::onChanged, this);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Height");
			label->setTextAlignment(GUI::TextRight);

			m_inputHeight = new GUI::CNumberInput(layout);
			m_inputHeight->setNumberType(GUI::UInteger);
			m_inputHeight->setValue(1080.0f, false);
			m_inputHeight->OnTextChanged = BIND_LISTENER(&CCanvasSettingController::onChanged, this);
			boxLayout->endVertical();
			
			boxLayout->addSpace(20.0f);

			layout = boxLayout->beginVertical();
			GUI::CButton* defaultBtn = new GUI::CButton(layout);
			defaultBtn->setLabel(L"Cancel");
			defaultBtn->setTextAlignment(GUI::TextCenter);
			defaultBtn->OnPress = BIND_LISTENER(&CCanvasSettingController::onCancel, this);

			GUI::CButton* okBtn = new GUI::CButton(layout);
			okBtn->setLabel(L"OK");
			okBtn->setTextAlignment(GUI::TextCenter);
			okBtn->OnPress = BIND_LISTENER(&CCanvasSettingController::onOK, this);
			boxLayout->endVertical();
		}

		CCanvasSettingController::~CCanvasSettingController()
		{

		}

		void CCanvasSettingController::update()
		{

		}

		void CCanvasSettingController::onShow()
		{
			
		}

		void CCanvasSettingController::onCancel(GUI::CBase* base)
		{
			m_menu->close();
		}

		void CCanvasSettingController::onOK(GUI::CBase* base)
		{
			m_menu->close();
		}

		void CCanvasSettingController::onChanged(GUI::CBase* base)
		{
			
		}
	}
}
