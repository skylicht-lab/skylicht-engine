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
#include "CSpaceProjectSettings.h"
#include "Editor/CEditor.h"

namespace Skylicht
{
	namespace Editor
	{
		CSpaceProjectSettings::CSpaceProjectSettings(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_menuGroup(NULL),
			m_gui(NULL)
		{
			GUI::CBase* bottom = new GUI::CBase(window);
			bottom->setHeight(50.0f);
			bottom->setPadding(GUI::SPadding(23.0f, 15.0f, -23.0f, -15.0f));
			bottom->dock(GUI::EPosition::Bottom);
			bottom->enableRenderFillRect(true);
			bottom->setFillRectColor(GUI::SGUIColor(255, 65, 65, 65));

			m_apply = new GUI::CButton(bottom);
			m_apply->dock(GUI::EPosition::Right);
			m_apply->setLabel(L"Apply");
			m_apply->setTextAlignment(GUI::ETextAlign::TextCenter);
			m_apply->setWidth(100.0f);
			m_apply->setDisabled(true);
			m_apply->enableDrawDisable(true);
			m_apply->OnPress = BIND_LISTENER(&CSpaceProjectSettings::onApply, this);

			GUI::CButton* cancel = new GUI::CButton(bottom);
			cancel->dock(GUI::EPosition::Right);
			cancel->setLabel(L"Cancel");
			cancel->setTextAlignment(GUI::ETextAlign::TextCenter);
			cancel->setWidth(100.0f);
			cancel->setMargin(GUI::SMargin(0.0f, 0.0f, 10.0f, 0.0f));
			cancel->OnPress = [window](GUI::CBase* button)
			{
				window->onCloseWindow();
			};

			GUI::CButton* ok = new GUI::CButton(bottom);
			ok->dock(GUI::EPosition::Right);
			ok->setLabel(L"OK");
			ok->setTextAlignment(GUI::ETextAlign::TextCenter);
			ok->setWidth(100.0f);
			ok->setMargin(GUI::SMargin(0.0f, 0.0f, 10.0f, 0.0f));
			ok->OnPress = BIND_LISTENER(&CSpaceProjectSettings::onOK, this);


			GUI::CSplitter* mainSplit = new GUI::CSplitter(window);
			mainSplit->dock(GUI::EPosition::Fill);
			mainSplit->enableRenderFillRect(true);
			mainSplit->setFillRectColor(GUI::SGUIColor(255, 75, 75, 75));
			mainSplit->setNumberRowCol(1, 2);
			mainSplit->setColWidth(0, 200.0f);
			mainSplit->setWeakCol(1);

			GUI::CBase* menuBase = new GUI::CBase(mainSplit);
			menuBase->setPadding(GUI::SPadding(5.0f, 5.0f, -5.0f, -5.0f));

			m_menuContainer = new GUI::CScrollControl(menuBase);
			m_menuContainer->dock(GUI::EPosition::Fill);
			m_menuContainer->enableScroll(false, true);
			m_menuContainer->showScrollBar(false, true);
			m_menuContainer->enableModifyChildWidth(true);
			m_menuContainer->enableRenderFillRect(false);
			m_menuContainer->getVerticalScroll()->enableRenderFillRect(false);
			mainSplit->setControl(menuBase, 0, 0);

			m_infoContainer = new GUI::CBase(mainSplit);
			m_infoContainer->enableRenderFillRect(true);
			m_infoContainer->setFillRectColor(GUI::SGUIColor(255, 55, 55, 55));

			addMenuButton(L"Layer name");
			addMenuButton(L"Collision");

			setSelectMenu(0);

			mainSplit->setControl(m_infoContainer, 0, 1);
		}

		CSpaceProjectSettings::~CSpaceProjectSettings()
		{
			for (CProjectSettingUI* ui : m_guis)
				delete ui;

			if (m_menuGroup != NULL)
				delete m_menuGroup;
		}

		void CSpaceProjectSettings::addMenuButton(const std::wstring& label)
		{
			if (m_menuGroup == NULL)
				m_menuGroup = new GUI::CToggleGroup();

			GUI::CFlatButton* button = new GUI::CFlatButton(m_menuContainer);
			button->setLabel(label);
			button->dock(GUI::EPosition::Top);

			GUI::SMargin m = button->getMargin();
			m.Right = m.Right + 1.0f;
			button->setMargin(m);

			button->OnPress = BIND_LISTENER(&CSpaceProjectSettings::onSelectMenu, this);

			m_menuGroup->addButton(button);

			if (m_menuButtons.size() == 0)
				m_menuGroup->selectButton(button);
			else
				m_menuButtons[m_menuButtons.size() - 1]->enableRenderBorder(true, true, true, false);

			m_menuButtons.push_back(button);

			GUI::CScrollControl* info = new GUI::CScrollControl(m_infoContainer);
			info->dock(GUI::EPosition::Fill);
			info->enableRenderFillRect(false);
			info->enableScroll(false, true);
			info->showScrollBar(false, true);
			info->enableModifyChildWidth(true);

			if (label == L"Layer name")
				m_guis.push_back(new CLayerNameUI(this, info, label));
			else
				m_guis.push_back(new CProjectSettingUI(this, info, label));

			m_guis.back()->setHidden(true);
		}

		void CSpaceProjectSettings::setSelectMenu(int id)
		{
			if (m_gui != NULL)
				m_gui->setHidden(true);

			m_menuGroup->selectButton(m_menuButtons[id]);

			m_gui = m_guis[id];
			m_gui->setHidden(false);
		}

		void CSpaceProjectSettings::enableApply()
		{
			if (m_gui != NULL)
				m_gui->setModify(true);

			m_apply->setDisabled(false);
		}

		void CSpaceProjectSettings::onSelectMenu(GUI::CBase* button)
		{
			if (m_gui != NULL)
				m_gui->setHidden(true);

			int i = m_menuGroup->selectButton((GUI::CButton*)button);
			if (i >= 0)
			{
				m_gui = m_guis[i];
				m_gui->setHidden(false);
			}
		}

		void CSpaceProjectSettings::onOK(GUI::CBase* button)
		{
			for (CProjectSettingUI* ui : m_guis)
				ui->save();

			m_window->onCloseWindow();
			m_editor->refresh();
		}

		void CSpaceProjectSettings::onApply(GUI::CBase* button)
		{
			for (CProjectSettingUI* ui : m_guis)
			{
				if (ui->isModify())
				{
					ui->save();
					ui->setModify(false);
				}
			}

			button->setDisabled(true);
			m_editor->refresh();
		}
	}
}