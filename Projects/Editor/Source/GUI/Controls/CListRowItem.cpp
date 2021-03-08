/*
!@
MIT License

CopyRight (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRight HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CListRowItem.h"
#include "CListBox.h"
#include "GUI/Theme/CThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CListRowItem::CListRowItem(CBase* base) :
				CButton(base),
				m_owner(base)
			{
				setIsToggle(true);

				setHoverColor(CThemeConfig::ListItemBackgroundHoverColor);
				setPressColor(CThemeConfig::ListItemBackgroundFocusColor);

				m_icon->setMargin(SMargin(0.0f, 0.0f, 5.0f, 0.0f));
				m_label->setMargin(SMargin(0.0f, 4.0f, 0.0f, 0.0f));
				m_label->setColor(CThemeConfig::ButtonTextColor);

				setSize(80.0f, 20.0f);
				setMargin(SMargin(0.0f, 0.0f, 0.0f, 0.0f));
				setPadding(SPadding(8.0f, 0.0f, 0.0f, 0.0f));

				showIcon(true);

				m_textEditHelper = new CTextEditHelper(this, new CTextBox(this), m_label);
			}

			CListRowItem::~CListRowItem()
			{
				delete m_textEditHelper;
			}

			void CListRowItem::layout()
			{
				CBase::layout();

				// compute child size
				m_label->sizeToContents();

				// fit to child size
				sizeToChildren(true, false);
			}

			void CListRowItem::onBoundsChanged(const SRect& oldBounds)
			{
				CBase::onBoundsChanged(oldBounds);
			}

			void CListRowItem::renderUnder()
			{
				if (isHovered() || m_toggleStatus)
				{
					renderBackground();
				}
			}

			void CListRowItem::renderBackground()
			{
				SGUIColor c = m_color;

				if (m_pressed == true)
					c = m_pressColor;
				else
					c = m_hoverColor;

				if (isToggle())
				{
					if (m_toggleStatus)
					{
						if (m_owner->isKeyboardFocus() == true)
							c = m_pressColor;
						else
							c = CThemeConfig::ListItemBackgroundUnfocusColor;
					}
				}

				SRect r = getRenderBounds();
				r.X = r.X + 1.0f;
				r.Y = r.Y + 1.0f;
				r.Width = r.Width - 2.0f;
				r.Height = r.Height - 1.0f;

				CTheme::getTheme()->drawButton(r, c);
			}

			void CListRowItem::onMouseClickRight(float x, float y, bool down)
			{
				if (isDisabled())
					return;

				CButton::onMouseClickRight(x, y, down);

				CListBox* listBox = dynamic_cast<CListBox*>(m_owner);
				if (listBox->OnItemContextMenu != nullptr)
					listBox->OnItemContextMenu(this);
			}
		}
	}
}