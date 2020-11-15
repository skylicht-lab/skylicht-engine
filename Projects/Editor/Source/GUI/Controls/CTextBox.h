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
#include "CText.h"
#include "CTextContainer.h"
#include "CScrollControl.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CTextBox : public CScrollControl
			{
			protected:
				CBase *m_container;
				CTextContainer *m_textContainer;

			public:
				CTextBox(CBase *base);

				virtual ~CTextBox();

				virtual void layout();

				virtual void postLayout();

				inline void setString(const std::wstring& string)
				{
					m_textContainer->setString(string);
				}

				inline const std::wstring& getString()
				{
					return m_textContainer->getString();
				}

				inline void setFontSize(EFontSize size)
				{
					m_textContainer->setFontSize(size);
				}

				inline EFontSize getFontSize()
				{
					return m_textContainer->getFontSize();
				}

				inline u32 getLength()
				{
					return m_textContainer->getLength();
				}

				inline bool isWrapMultiline()
				{
					return m_textContainer->isWrapMultiline();
				}

				inline void setWrapMultiline(bool b)
				{
					m_textContainer->setWrapMultiline(b);
				}

				inline void setColor(const SGUIColor& color)
				{
					m_textContainer->setColor(color);
				}

				inline const SGUIColor& getColor()
				{
					return m_textContainer->getColor();
				}
			};
		}
	}
}