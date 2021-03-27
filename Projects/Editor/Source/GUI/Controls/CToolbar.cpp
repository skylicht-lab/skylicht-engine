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

#include "pch.h"
#include "CToolbar.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CToolbar::CToolbar(CBase *parent) :
				CBase(parent)
			{
				setHeight(25.0f);

				setPadding(SPadding(1.0f, 2.0f, 1.0f, -3.0f));
				enableRenderFillRect(true);
				setFillRectColor(GUI::CThemeConfig::WindowBackgroundColor);

				dock(EPosition::Top);
			}

			CToolbar::~CToolbar()
			{

			}

			CButton* CToolbar::addButton(const std::wstring& label, ESystemIcon icon, bool inRightToolbar)
			{
				CButton *newButton = new CButton(this);
				newButton->setLabel(label);
				newButton->setIcon(icon);
				newButton->setPadding(SPadding(0.0f, 0.0f, 6.0f, 0.0f));

				if (icon != ESystemIcon::None)
					newButton->showIcon(true);

				if (inRightToolbar)
					newButton->dock(EPosition::Right);
				else
					newButton->dock(EPosition::Left);

				newButton->enableDrawBackground(false);
				newButton->sizeToContents();

				return newButton;
			}

			CButton* CToolbar::addButton(ESystemIcon icon, bool inRightToolbar)
			{
				CButton *newButton = new CButton(this);
				newButton->setSize(20.0f, 20.0f);
				newButton->setPadding(SPadding(0.0f, 0.0f, 0.0f, 0.0f));
				newButton->showLabel(false);

				if (icon != ESystemIcon::None)
					newButton->showIcon(true);

				newButton->setIcon(icon);

				if (inRightToolbar)
					newButton->dock(EPosition::Right);
				else
					newButton->dock(EPosition::Left);

				newButton->enableDrawBackground(false);

				return newButton;
			}

			CBase* CToolbar::addSpace(bool inRightToolbar)
			{
				CBase *space = new CBase(this);
				space->setWidth(5.0f);

				if (inRightToolbar)
					space->dock(EPosition::Right);
				else
					space->dock(EPosition::Left);

				return space;
			}

			CBase* CToolbar::addControl(CBase *control, bool inRightToolbar)
			{
				control->setParent(this);

				if (inRightToolbar)
					control->dock(EPosition::Right);
				else
					control->dock(EPosition::Left);

				return control;
			}
		}
	}
}