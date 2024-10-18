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
#include "CButton.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CMenu;
			class CMenuItem : public CButton
			{
			protected:
				CTextContainer *m_accelerator;
				CIcon *m_submenuArrow;

				CMenu *m_menu;

				bool m_inMenuBar;
				bool m_isOpen;

			public:
				CMenuItem(CBase *parent);
				virtual ~CMenuItem();

				void setAccelerator(const std::wstring& accelerator);

				virtual void renderUnder();

				virtual void sizeToContents();

				virtual void layout();

				virtual void setLabelColor(const SGUIColor& color);

				virtual CMenu* getMenu();

				virtual void openMenu();

				virtual void closeMenu();

				inline void setInMenuBar(bool b)
				{
					m_inMenuBar = b;
				}

				inline bool isOpenMenu()
				{
					return m_isOpen;
				}

				inline bool haveSubMenu()
				{
					return m_menu != NULL;
				}
			};
		}
	}
}