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
#include "Editor/CEditorSetting.h"
#include "Editor/SpaceController/CSceneController.h"
#include "Camera/CEditorMoveCamera.h"

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
			label->setString("3D navigation");
			label->setTextAlignment(GUI::TextRight);

			m_navigation = new GUI::CComboBox(layout);
			m_navigation->addItem(std::wstring(L"Default"));
			m_navigation->addItem(std::wstring(L"Maya"));
			m_navigation->addItem(std::wstring(L"Blender"));
			m_navigation->OnChanged = BIND_LISTENER(&CCameraSettingController::onChanged, this);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Move speed");
			label->setTextAlignment(GUI::TextRight);

			m_moveSpeed = new GUI::CSlider(layout);
			m_moveSpeed->setValue(1.0f, 0.1f, 5.0f, false);
			m_moveSpeed->OnTextChanged = BIND_LISTENER(&CCameraSettingController::onChanged, this);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Zoom speed");
			label->setTextAlignment(GUI::TextRight);

			m_zoomSpeed = new GUI::CSlider(layout);
			m_zoomSpeed->setValue(1.0f, 0.1f, 5.0f, false);
			m_zoomSpeed->OnTextChanged = BIND_LISTENER(&CCameraSettingController::onChanged, this);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Rotate speed");
			label->setTextAlignment(GUI::TextRight);

			m_rotSpeed = new GUI::CSlider(layout);
			m_rotSpeed->setValue(16.0f, 1.0f, 30.0f, false);
			m_rotSpeed->OnTextChanged = BIND_LISTENER(&CCameraSettingController::onChanged, this);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("FOV");
			label->setTextAlignment(GUI::TextRight);

			m_fov = new GUI::CSlider(layout);
			m_fov->setValue(60.0f, 1.0f, 90.0f, false);
			m_fov->OnTextChanged = BIND_LISTENER(&CCameraSettingController::onChanged, this);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Near");
			label->setTextAlignment(GUI::TextRight);

			m_near = new GUI::CSlider(layout);
			m_near->setValue(0.05f, 0.01f, 10.0f, false);
			m_near->OnTextChanged = BIND_LISTENER(&CCameraSettingController::onChanged, this);
			boxLayout->endVertical();

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Far");
			label->setTextAlignment(GUI::TextRight);

			m_far = new GUI::CSlider(layout);
			m_far->setValue(1500.0f, 20.0f, 5000.0f, false);
			m_far->OnTextChanged = BIND_LISTENER(&CCameraSettingController::onChanged, this);
			boxLayout->endVertical();

			boxLayout->addSpace(20.0f);

			layout = boxLayout->beginVertical();
			label = new GUI::CLabel(layout);
			label->setPadding(GUI::SMargin(0.0f, 2.0, 0.0f, 0.0f));
			label->setString("Draw grid");
			label->setTextAlignment(GUI::TextRight);

			GUI::CBase* parentCB = new GUI::CBase(layout);
			m_grid = new GUI::CCheckBox(parentCB);
			m_grid->OnChanged = BIND_LISTENER(&CCameraSettingController::onChanged, this);
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

			m_menu->OnClose = BIND_LISTENER(&CCameraSettingController::onClose, this);
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
			CCamera* camera = spaceScene->getEditorCamera();
			CEditorCamera* editorCamera = camera->getGameObject()->getComponent<CEditorCamera>();

			CEditorCamera::EControlStyle style = editorCamera->getControlStyle();
			if (style == CEditorCamera::Maya)
				m_navigation->setSelectIndex(1, false);
			else if (style == CEditorCamera::Blender)
				m_navigation->setSelectIndex(2, false);
			else
				m_navigation->setSelectIndex(0, false);

			// setup value
			m_moveSpeed->setValue(editorCamera->getMoveSpeed(), false);
			m_rotSpeed->setValue(editorCamera->getRotateSpeed(), false);
			m_fov->setValue(camera->getFOV(), false);
			m_near->setValue(camera->getNearValue(), false);
			m_far->setValue(camera->getFarValue(), false);

			m_grid->setToggle(spaceScene->isEnableRenderGrid());
		}

		void CCameraSettingController::onClose(GUI::CBase* base)
		{
			onChanged(base);
			saveSetting();
		}

		void CCameraSettingController::onDefault(GUI::CBase* base)
		{
			CSpaceScene* spaceScene = CSceneController::getInstance()->getSpaceScene();
			CCamera* camera = spaceScene->getEditorCamera();
			CEditorCamera* editorCamera = camera->getGameObject()->getComponent<CEditorCamera>();
			CEditorMoveCamera* editorMove = camera->getGameObject()->getComponent<CEditorMoveCamera>();

			// default value
			editorCamera->setControlStyle(CEditorCamera::Default);
			editorCamera->setMoveSpeed(1.0f);
			editorCamera->setZoomSpeed(1.0f);
			editorCamera->setRotateSpeed(16.0f);
			editorMove->setMoveSpeed(1.0f * 5.0f);
			camera->setFOV(60.0f);
			camera->setNearValue(0.05f);
			camera->setFarValue(1500.0f);

			// ui value
			m_navigation->setSelectIndex(0, false);
			m_moveSpeed->setValue(editorCamera->getMoveSpeed(), false);
			m_zoomSpeed->setValue(editorCamera->getZoomSpeed(), false);
			m_rotSpeed->setValue(editorCamera->getRotateSpeed(), false);
			m_fov->setValue(camera->getFOV(), false);
			m_near->setValue(camera->getNearValue(), false);
			m_far->setValue(camera->getFarValue(), false);

			m_menu->close();
		}

		void CCameraSettingController::onOK(GUI::CBase* base)
		{
			m_menu->close();
		}

		void CCameraSettingController::onChanged(GUI::CBase* base)
		{
			CSpaceScene* spaceScene = CSceneController::getInstance()->getSpaceScene();
			CCamera* camera = spaceScene->getEditorCamera();
			CEditorCamera* editorCamera = camera->getGameObject()->getComponent<CEditorCamera>();
			CEditorMoveCamera* editorMove = camera->getGameObject()->getComponent<CEditorMoveCamera>();

			u32 style = m_navigation->getSelectIndex();
			if (style == 1)
				editorCamera->setControlStyle(CEditorCamera::Maya);
			else if (style == 2)
				editorCamera->setControlStyle(CEditorCamera::Blender);
			else
				editorCamera->setControlStyle(CEditorCamera::Default);

			editorMove->setMoveSpeed(m_moveSpeed->getValue() * 5.0f);
			editorCamera->setMoveSpeed(m_moveSpeed->getValue());
			editorCamera->setZoomSpeed(m_zoomSpeed->getValue());
			editorCamera->setRotateSpeed(m_rotSpeed->getValue());
			camera->setFOV(m_fov->getValue());
			camera->setNearValue(m_near->getValue());
			camera->setFarValue(m_far->getValue());

			spaceScene->enableRenderGrid(m_grid->getToggle());
		}

		void CCameraSettingController::saveSetting()
		{
			CEditorSetting* editorSetting = CEditorSetting::getInstance();
			editorSetting->CameraNavigation.set(m_navigation->getSelectIndex());
			editorSetting->CameraMoveSpeed.set(m_moveSpeed->getValue());
			editorSetting->CameraZoomSpeed.set(m_zoomSpeed->getValue());
			editorSetting->CameraRotateSpeed.set(m_rotSpeed->getValue());
			editorSetting->save();
		}
	}
}