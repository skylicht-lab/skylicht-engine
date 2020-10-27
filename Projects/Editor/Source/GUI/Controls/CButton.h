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
#include "CTextContainer.h"
#include "CIcon.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CButton : public CBase
			{
			protected:
				CIcon *m_icon;
				CTextContainer *m_label;

				SGUIColor m_color;
				SGUIColor m_hoverColor;
				SGUIColor m_pressColor;
				SGUIColor m_focusColor;

				SGUIColor m_iconColor;
				SGUIColor m_labelColor;

				bool m_pressed;

			public:

				Listener OnPress;
				Listener OnRightPress;

			public:
				CButton(CBase *parent);
				virtual ~CButton();

				virtual void onBoundsChanged(const SRect& oldBounds);

				void setLabel(const std::wstring& text);

				virtual void renderUnder();

				virtual void onMouseClickLeft(float x, float y, bool bDown);

				virtual void onMouseClickRight(float x, float y, bool bDown);

				virtual void onMouseLeave();

				inline void setLabelColor(const SGUIColor& color)
				{
					m_label->setColor(color);
					m_labelColor = color;
				}

				inline void setLabelFontSize(EFontSize fontsize)
				{
					m_label->setFontSize(fontsize);
				}

				inline const std::wstring& getLabel()
				{
					return m_label->getString();
				}

				inline const SGUIColor& getLabelColor()
				{
					return m_label->getColor();
				}

				inline EFontSize getFontSize()
				{
					return m_label->getFontSize();
				}

				inline void showIcon(bool b)
				{
					m_icon->setHidden(!b);
				}

				inline void setIcon(ESystemIcon icon)
				{
					m_icon->setIcon(icon);
				}

				inline ESystemIcon getIcon()
				{
					return m_icon->getIcon();
				}

				inline void setIconColor(const SGUIColor& c)
				{
					m_icon->setColor(c);
					m_iconColor = c;
				}

				inline const SGUIColor& getIconColor()
				{
					return m_icon->getColor();
				}

				inline void setIconMargin(const SMargin& m)
				{
					m_icon->setMargin(m);
				}

				inline void setLabelMargin(const SMargin& m)
				{
					m_label->setMargin(m);
				}

				inline void setColor(const SGUIColor& c)
				{
					m_color = c;
				}

				inline void setHoverColor(const SGUIColor& c)
				{
					m_hoverColor = c;
				}

				inline void setPressColor(const SGUIColor& c)
				{
					m_pressColor = c;
				}

				inline void setFocusColor(const SGUIColor& c)
				{
					m_focusColor = c;
				}

				Listener OnTextChanged;
			};
		}
	}
}