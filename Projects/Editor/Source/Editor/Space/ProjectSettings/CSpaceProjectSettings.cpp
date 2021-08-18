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

namespace Skylicht
{
	namespace Editor
	{
		CSpaceProjectSettings::CSpaceProjectSettings(GUI::CWindow* window, CEditor* editor) :
			CSpace(window, editor),
			m_menuGroup(NULL)
		{
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

			addMenuButton(L"Layer name");
			addMenuButton(L"Collision");

			GUI::CBase* infoBase = new GUI::CBase(mainSplit);
			infoBase->enableRenderFillRect(true);
			infoBase->setFillRectColor(GUI::SGUIColor(255, 55, 55, 55));

			m_infoContainer = new GUI::CScrollControl(infoBase);
			m_infoContainer->dock(GUI::EPosition::Fill);
			m_infoContainer->enableRenderFillRect(false);
			m_infoContainer->enableScroll(false, true);
			m_infoContainer->showScrollBar(false, true);
			m_infoContainer->enableModifyChildWidth(true);

			GUI::CCollapsibleGroup* colapsible = new GUI::CCollapsibleGroup(m_infoContainer);
			colapsible->dock(GUI::EPosition::Top);
			colapsible->getHeader()->setFillRectColor(GUI::SGUIColor(255, 65, 65, 65));
			colapsible->getHeader()->setLabel(L"Layer Name");
			colapsible->setExpand(true);

			mainSplit->setControl(infoBase, 0, 1);
		}

		CSpaceProjectSettings::~CSpaceProjectSettings()
		{
			if (m_menuGroup != NULL)
				delete m_menuGroup;
		}

		void CSpaceProjectSettings::addMenuButton(const wchar_t* label)
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
		}

		void CSpaceProjectSettings::onSelectMenu(GUI::CBase* button)
		{
			m_menuGroup->selectButton((GUI::CButton*)button);
		}
	}
}