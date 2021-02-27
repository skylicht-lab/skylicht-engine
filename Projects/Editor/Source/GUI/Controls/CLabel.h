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

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CLabel : public CBase
			{
			protected:
				CTextContainer* m_text;

			public:
				CLabel(CBase* parent);
				virtual ~CLabel();

				virtual void sizeToContents();

				virtual void onBoundsChanged(const SRect& oldBounds);

				void setString(const std::wstring& text);

				void setColor(const SGUIColor& color);

				void setFontSize(EFontSize fontsize);

				void setWrapMultiline(bool b)
				{
					m_text->setWrapMultiline(b);
				}

				inline const std::wstring& getString()
				{
					return m_text->getString();
				}

				inline const SGUIColor& getColor()
				{
					return m_text->getColor();
				}

				inline EFontSize getFontSize()
				{
					return m_text->getFontSize();
				}

				inline void setTextAlignment(ETextAlign align)
				{
					m_text->setTextAlignment(align);
				}

				inline ETextAlign getTextAlignment()
				{
					return m_text->getTextAlignment();
				}

				Listener OnTextChanged;
			};
		}
	}
}