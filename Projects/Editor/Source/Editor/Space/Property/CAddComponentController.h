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

#pragma once

#include "GUI/GUI.h"
#include "GameObject/CGameObject.h"
#include "Components/CComponentCategory.h"

namespace Skylicht
{
	namespace Editor
	{
		class CEditor;

		class CAddComponentController
		{
		protected:
			CEditor* m_editor;
			GUI::CMenu* m_menu;

			GUI::CTextBox* m_search;
			GUI::CLabel* m_folder;
			GUI::CButton* m_back;
			GUI::CListBox* m_list;

			float m_inputTimeout;
			bool m_changed;

			std::wstring m_searchString;

			const CComponentCategory::SCategory* m_category;

			CGameObject* m_gameObject;

		public:
			CAddComponentController(CEditor* editor, GUI::CMenu* menu);

			virtual ~CAddComponentController();

			inline void SetGameObject(CGameObject* gameObject)
			{
				m_gameObject = gameObject;
			}

			void focusSearch();

			void update();

		protected:

			void cancelSearch();

			void search(const std::string& searchString);

			void OnSearch(GUI::CBase* base);

			void OnCancel(GUI::CBase* base);

			void OnSelectCategory(GUI::CBase* item);

			void OnSelectComponent(GUI::CBase* item);

			void OnBack(GUI::CBase* button);

			void listCurrentCategory();

		};
	}
}