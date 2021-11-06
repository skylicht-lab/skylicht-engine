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

#pragma once

#include "SkylichtEngine.h"
#include "Editor/Space/CSpace.h"
#include "Editor/Components/CComponentEditor.h"
#include "Reactive/CSubject.h"
#include "Reactive/CObserver.h"

#include "CAddComponentController.h"

namespace Skylicht
{
	namespace Editor
	{
		class CSpaceProperty : public CSpace
		{
		public:
			struct SGroup
			{
				CComponentEditor* Owner;
				GUI::CBase* GroupUI;
				std::vector<IObserver*> Observer;

				SGroup()
				{
					Owner = NULL;
					GroupUI = NULL;
				}

				void releaseObserver()
				{
					for (IObserver* o : Observer)
						delete o;
					Observer.clear();
				}
			};

		protected:
			GUI::CBase* m_content;
			GUI::CIcon* m_icon;
			GUI::CLabel* m_label;

			GUI::CMenu* m_addComponentMenu;
			CAddComponentController* m_addComponentController;

			GUI::CMenu* m_componentContextMenu;
			GUI::CMenuItem* m_menuUp;
			GUI::CMenuItem* m_menuDown;
			GUI::CMenuItem* m_menuRemove;
			CComponentEditor* m_menuContextEditor;

			std::vector<SGroup*> m_groups;
			std::vector<CComponentEditor*> m_releaseComponents;

		public:
			CSpaceProperty(GUI::CWindow* window, CEditor* editor);

			virtual ~CSpaceProperty();

			void OnComponentCommand(GUI::CBase* base);

			virtual void update();

			virtual void refresh();

			void addNumberInput(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<float>* value, float step);

			void addNumberInput(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<int>* value, int step = 1);

			void addNumberInput(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<u32>* value, int step = 1);

			void addTextBox(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<std::wstring>* value);

			void addCheckBox(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<bool>* value);

			void addComboBox(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<std::wstring>* value, const std::vector<std::wstring>& listValue);

			void addSlider(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<float>* value, float min, float max);

			void addColorPicker(GUI::CBoxLayout* boxLayout, const wchar_t* name, CSubject<SColor>* value);

			GUI::CDropdownBox* addDropBox(GUI::CBoxLayout* boxLayout, const wchar_t* name, const std::wstring& value);

			inline void setLabel(const wchar_t* label)
			{
				m_label->setString(label);
			}

			inline void setIcon(GUI::ESystemIcon icon)
			{
				m_icon->setIcon(icon);
			}

			GUI::CCollapsibleGroup* addGroup(const wchar_t* label, CComponentEditor* editor);

			GUI::CCollapsibleGroup* addGroup(const char* label, CComponentEditor* editor);

			GUI::CButton* addButton(const wchar_t* label);

			void popupComponentMenu(CGameObject* gameObject, GUI::CBase* position);

			void removeGroupByOwner(CComponentEditor* editor);

			void addComponent(CComponentEditor* editor, CComponentSystem* component, bool autoRelease = false);

			void addComponent(CComponentEditor* editor, CGameObject* gameobject);

			void clearAllGroup();

			SGroup* getGroupByLayout(GUI::CBoxLayout* layout);

			GUI::CBoxLayout* createBoxLayout(GUI::CCollapsibleGroup* group);
		};
	}
}