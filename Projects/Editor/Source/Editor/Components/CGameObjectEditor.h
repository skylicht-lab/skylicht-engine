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

#pragma once

#include "CComponentEditor.h"
#include "Activator/CEditorActivator.h"
#include "Reactive/CSubject.h"
#include "Reactive/CObserver.h"
#include "GUI/GUI.h"
#include "Selection/CSelectObject.h"

namespace Skylicht
{
	namespace Editor
	{
		class CGameObjectEditor :
			public CComponentEditor,
			public IObserver
		{
		public:
			CSubject<std::wstring> Name;
			CSubject<bool> Enable;
			CSubject<bool> Visible;
			CSubject<bool> Static;
			CSubject<bool> Lock;
			CSubject<std::string> Template;

			CSelectObject* SelectObject;
			IObserver* Observer;

		public:
			CGameObjectEditor();

			virtual ~CGameObjectEditor();

			virtual void initGUI(CGameObject* object, CSpaceProperty* ui);

			virtual void closeGUI();

			virtual void update();

			virtual void onNotify(ISubject* subject, IObserver* from);

		protected:

			void initObjectLayerMenu(CGameObject* object, GUI::CDropdownBox* dropDown, CSpaceProperty* ui);

			std::vector<CGameObject*> getSelected();
		};
	}
}