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

#include "CBase.h"
#include "CButton.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CIconTextItem : public CBase
			{
			protected:
				CIcon *m_icon;
				CTextContainer *m_label;

			public:
				CIconTextItem(CBase *base);

				virtual ~CIconTextItem();

				virtual void sizeToContents();

				inline CTextContainer* getTextContainer()
				{
					return m_label;
				}

				inline void setLabel(const std::wstring& text)
				{
					m_label->setString(text);
				}

				inline const std::wstring& getLabel()
				{
					return m_label->getString();
				}

				inline void setLabelFontSize(EFontSize fontsize)
				{
					m_label->setFontSize(fontsize);
				}

				inline void setLabelMargin(const SMargin& margin)
				{
					m_label->setMargin(margin);
				}

				inline const SGUIColor& getLabelColor()
				{
					return m_label->getColor();
				}

				virtual void setLabelColor(const SGUIColor& color)
				{
					m_label->setColor(color);
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
				}

				inline const SGUIColor& getIconColor()
				{
					return m_icon->getColor();
				}
			};
		}
	}
}