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

#include "GUI/Controls/CBase.h"
#include "GUI/Controls/CLabel.h"
#include "GUI/Controls/CIcon.h"
#include "GUI/Controls/CIconButton.h"
#include "GUI/Controls/CResizableControl.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CWindow : public CResizableControl
			{
			protected:
				CDragger *m_titleBar;
				CLabel *m_title;
				CIcon *m_icon;
				CIconButton *m_close;

				bool m_childStyle;
			public:
				CWindow(CBase* parent, float x, float y, float w, float h);

				virtual ~CWindow();

				virtual void onCloseWindow();

				virtual void renderUnder();

				virtual void touch();

				virtual void onChildTouched(CBase* child);

				void setStyleChild(bool b);

				void setCaption(const std::wstring& text)
				{
					m_title->setString(text);
				}

				const std::wstring& getCaption()
				{
					return m_title->getString();
				}

				inline void showIcon(bool b)
				{
					m_icon->setHidden(!b);
				}

				void setIcon(ESystemIcon icon)
				{
					m_icon->setIcon(icon);
					m_icon->setHidden(false);
				}

			protected:

				void onCloseButtonPress(CBase *sender);

			};
		}
	}
}