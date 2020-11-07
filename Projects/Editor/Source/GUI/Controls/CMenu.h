/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
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

#include "CBase.h"
#include "CMenuItem.h"
#include "CMenuSeparator.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CMenu : public CBase
			{
			protected:
				bool m_isOpenSubMenu;

			public:
				CMenu(CBase *parent);
				virtual ~CMenu();

				virtual void layout();

				virtual void renderUnder();

				CMenuItem* getItemByPath(const std::wstring& path);
				CMenuItem* getItemByLabel(const std::wstring& label);

				CMenuItem* addItemByPath(const std::wstring& path);
				CMenuItem* addItemByPath(const std::wstring& path, const std::wstring& accelerator);
				CMenuItem* addItemByPath(const std::wstring& path, ESystemIcon icon);
				CMenuItem* addItemByPath(const std::wstring& path, ESystemIcon icon, const std::wstring& accelerator);

				CMenuItem* addItem(ESystemIcon icon);
				CMenuItem* addItem(const std::wstring& label, ESystemIcon icon, const std::wstring& accelerator);
				CMenuItem* addItem(const std::wstring& label);
				CMenuItem* addItem(const std::wstring& label, const std::wstring& accelerator);
				CMenuItem* addItem(const std::wstring& label, ESystemIcon icon);

				CMenuSeparator* addSeparator();

				virtual void onMenuItemHover(CBase *item);

				virtual void openMenu(CMenuItem *item);

				virtual void closeMenu();

				virtual bool isMenuComponent()
				{
					return true;
				}

				virtual void onMenuItemPress(CBase *item);

			public:

				Listener OnCommand;

			protected:

				virtual void onAddItem(CMenuItem *item);
			};
		}
	}
}