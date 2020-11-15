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

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			class CTextContainer : public CBase
			{
			public:
				typedef std::list<CText*> ListTextControl;

			public:
				CTextContainer(CBase *parent);

				virtual ~CTextContainer();

				virtual void render();

				void setString(const std::wstring& string);

				u32 getClosestCharacter(const SPoint& point, u32& outLine, u32& outChar);

				void setCaretBegin(u32 line, u32 c);

				void setCaretEnd(u32 line, u32 c);

				inline void showCaret(bool b)
				{
					m_showCaret = b;
				}

				inline void setCaretBlinkSpeed(float ms)
				{
					m_caretBlinkSpeed = ms;
				}

				inline const std::wstring& getString()
				{
					return m_string;
				}

				void setFontSize(EFontSize size);

				inline EFontSize getFontSize()
				{
					return m_fontSize;
				}

				inline u32 getLength()
				{
					return m_string.size();
				}

				void setWrapMultiline(bool b);

				inline bool isWrapMultiline()
				{
					return m_wrapMultiLine;
				}

				void setColor(const SGUIColor& color);

				inline const SGUIColor& getColor()
				{
					return m_color;
				}

				virtual void layout();

				void sizeToContents();

				inline void setInnerPaddingRight(float f)
				{
					m_paddingRight = f;
				}

			protected:

				void removeAllLines();

				bool splitWords(std::wstring string, std::vector<std::wstring>& lines, float lineWidth);

			protected:
				std::wstring m_string;

				bool m_wrapMultiLine;

				bool m_textChange;

				float m_paddingRight;

				SGUIColor m_color;

				EFontSize m_fontSize;

				ListTextControl m_lines;

				u32 m_caretBeginLine;

				u32 m_caretBeginPosition;

				u32 m_caretEndLine;

				u32 m_caretEndPosition;

				SRect m_caretRect;

				float m_caretBlinkSpeed;

				float m_caretBlink;

				bool m_showCaret;
			};
		}
	}
}