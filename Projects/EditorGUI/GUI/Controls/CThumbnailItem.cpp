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
#include "CThumbnailItem.h"
#include "CListBase.h"
#include "GUI/Theme/ThemeConfig.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CThumbnailItem::CThumbnailItem(CListBase* parent, float itemWidth, float itemHeight, bool haveLargeIcon) :
				CListItemBase(parent),
				m_largeIcon(NULL)
			{
				setIsToggle(true);
				setPadding(SPadding(0.0f, 0.0f, 0.0f, 0.0f));

				setHoverColor(ThemeConfig::ListItemBackgroundHoverColor);
				setPressColor(ThemeConfig::ListItemBackgroundFocusColor);

				m_label->dock(EPosition::Bottom);
				m_label->setTextAlignment(GUI::TextCenter);
				m_label->setAutoTrimText(true);

				m_render = new CBase(this);
				m_render->setTransparentMouseInput(true);
				m_render->setMargin(SMargin(5.0f, 5.0f, 5.0f, 5.0f));
				m_render->dock(EPosition::Fill);

				if (haveLargeIcon)
				{
					m_largeIcon = new CIcon(m_render, GUI::None, true);
					m_largeIcon->dock(EPosition::Center);
				}

				setSize(itemWidth, itemHeight);
			}

			CThumbnailItem::~CThumbnailItem()
			{

			}

			void CThumbnailItem::renderUnder()
			{
				if (m_renderDragDrop)
					return;

				if (isHovered() || m_toggleStatus)
					renderBackground();
			}

			void CThumbnailItem::renderBackground()
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
							c = ThemeConfig::ListItemBackgroundUnfocusColor;
					}
				}

				SRect r = getRenderBounds();
				r.X = r.X + 1.0f;
				r.Y = r.Y + 1.0f;
				r.Width = r.Width - 2.0f;
				r.Height = r.Height - 1.0f;

				CTheme::getTheme()->drawButton(r, c);
			}

			void CThumbnailItem::showIcon(bool b)
			{
				CListItemBase::showIcon(b);
				if (m_largeIcon)
					m_largeIcon->setHidden(!b);
			}

			void CThumbnailItem::setIcon(ESystemIcon icon)
			{
				CListItemBase::setIcon(icon);
				if (m_largeIcon)
					m_largeIcon->setIcon(icon);
			}

			void CThumbnailItem::setIconColor(const SGUIColor& c)
			{
				CListItemBase::setIconColor(c);
				if (m_largeIcon)
					m_largeIcon->setColor(c);
			}
		}
	}
}