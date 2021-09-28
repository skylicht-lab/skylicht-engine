/*
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
#include "CAddComponentController.h"
#include "Editor/CEditor.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	namespace Editor
	{
		CAddComponentController::CAddComponentController(CEditor* editor, GUI::CMenu* menu) :
			m_editor(editor),
			m_menu(menu),
			m_gameObject(NULL)
		{
			GUI::CBase* searchPanel = new GUI::CBase(menu);

			m_search = new GUI::CTextBox(searchPanel);
			m_search->showIcon();
			m_search->setStringHint(L"Search");
			m_search->setMargin(GUI::SMargin(10.0f, 10.0f, 10.0f, 10.0f));
			m_search->dock(GUI::EPosition::Top);

			searchPanel->setHeight(40.0f);
			searchPanel->dock(GUI::EPosition::Top);

			GUI::CBase* categoryPanel = new GUI::CBase(menu);
			categoryPanel->setHeight(20.0f);
			categoryPanel->setFillRectColor(GUI::CThemeConfig::CollapsibleColor);
			categoryPanel->enableRenderFillRect(true);
			categoryPanel->dock(GUI::EPosition::Top);

			GUI::CBase* left = new GUI::CBase(categoryPanel);
			left->setWidth(20.0f);
			left->dock(GUI::EPosition::Left);

			m_back = new GUI::CButton(left);
			m_back->setWidth(20.0f);
			m_back->setPadding(GUI::SPadding(0.0f, 0.0f, 0.0f, 0.0f));
			m_back->dock(GUI::EPosition::Left);
			m_back->setIcon(GUI::ESystemIcon::TriangleLeft);
			m_back->showIcon(true);
			m_back->showLabel(false);
			m_back->enableDrawBackground(false);
			m_back->setHidden(true);
			m_back->OnPress = BIND_LISTENER(&CAddComponentController::OnBack, this);

			m_folder = new GUI::CLabel(categoryPanel);
			m_folder->setPadding(GUI::SPadding(0.0f, 2.0f, -20.0f, 0.0f));
			m_folder->setTextAlignment(GUI::ETextAlign::TextCenter);
			m_folder->dock(GUI::EPosition::Fill);

			m_list = new GUI::CListBox(menu);
			m_list->setHeight(250.0f);
			m_list->dock(GUI::EPosition::Top);

			m_category = CComponentCategory::getInstance()->getCategory();
			listCurrentCategory();
		}

		CAddComponentController::~CAddComponentController()
		{

		}

		void CAddComponentController::listCurrentCategory()
		{
			m_list->removeAllItem();

			for (const CComponentCategory::SCategory* subCategory : m_category->Childs)
			{
				std::wstring name = CStringImp::convertUTF8ToUnicode(subCategory->Name.c_str());
				GUI::CListRowItem* item = m_list->addItem(name.c_str(), GUI::ESystemIcon::Folder);
				item->tagString(subCategory->Name);
				item->setIconColor(GUI::CThemeConfig::FolderColor);
				item->OnDoubleLeftMouseClick = BIND_LISTENER(&CAddComponentController::OnSelectCategory, this);
			}

			for (const CComponentCategory::SComponent* component : m_category->Components)
			{
				std::wstring name = CStringImp::convertUTF8ToUnicode(component->Name.c_str());
				GUI::CListRowItem* item = m_list->addItem(name.c_str(), GUI::ESystemIcon::ResFile);
				item->tagString(component->ComponentName);
				item->OnDoubleLeftMouseClick = BIND_LISTENER(&CAddComponentController::OnSelectComponent, this);
			}

			m_list->setScrollVertical(0.0f);
			m_folder->setString(m_category->Name);
		}

		void CAddComponentController::OnSelectCategory(GUI::CBase* item)
		{
			std::string name = item->getTagString();
			m_category = m_category->getChild(name.c_str());

			if (m_category->Parent != NULL)
			{
				m_back->setHidden(false);
			}
			else
				m_back->setHidden(true);

			listCurrentCategory();
		}

		void CAddComponentController::OnBack(GUI::CBase* button)
		{
			m_category = m_category->Parent;

			if (m_category->Parent != NULL)
				m_back->setHidden(false);
			else
				m_back->setHidden(true);

			listCurrentCategory();
		}

		void CAddComponentController::OnSelectComponent(GUI::CBase* item)
		{
			const std::string& typeName = item->getTagString();
			m_gameObject->addComponentByTypeName(typeName.c_str());

			m_menu->closeMenu();
			m_editor->refresh();
		}
	}
}