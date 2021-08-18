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
#include "CIconTextItem.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CIconTextItem::CIconTextItem(CBase* base) :
				CBase(base)
			{
				setMouseInputEnabled(false);

				m_icon = new CIcon(this, ESystemIcon::None);
				m_icon->dock(EPosition::Left);
				m_icon->setMargin(SMargin(0.0f, 0.0f, 5.0f, 0.0f));

				m_label = new CTextContainer(this);
				m_label->setMargin(SMargin(0.0f, 4.0f, 0.0f, 0.0f));
				m_label->dock(EPosition::Fill);
				m_label->setColor(CThemeConfig::ButtonTextColor);

				setSize(80.0f, 20.0f);
				setMargin(SMargin(0.0f, 0.0f, 0.0f, 0.0f));
				setPadding(SPadding(8.0f, 0.0f, -8.0f, 0.0f));
			}

			CIconTextItem::~CIconTextItem()
			{

			}

			void CIconTextItem::sizeToContents()
			{
				float h = height();

				m_label->sizeToContents();

				float w = m_padding.Left + m_padding.Right + m_label->width();
				if (m_icon->isHidden() == false)
					w = w + m_icon->width();

				setSize(w, h);
			}
		}
	}
}