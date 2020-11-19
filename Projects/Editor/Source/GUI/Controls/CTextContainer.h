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

				virtual void renderUnder();

				void setString(const std::wstring& string);

				void getSelectString(std::wstring& string);

				void doDeleteTextInSelection();

				void doBackspace();

				void doDelete();

				void doInsertCharacter(wchar_t c);

				void doInsertString(const std::wstring& string);

				u32 getClosestCharacter(const SPoint& point, u32& outLine, u32& outChar);

				u32 getWordAtPosition(u32 line, u32 charPosition, u32& from, u32& to);

				u32 getLineAtPosition(u32 line, u32 charPosition, u32& to);

				void setCaretBegin(u32 line, u32 c);

				void setCaretEnd(u32 line, u32 c);

				u32 getCaretBeginAt();

				u32 getCaretEndAt();

				inline void resetCaretBlink()
				{
					m_caretBlink = 0.0f;
				}

				inline void getCaretBegin(u32& line, u32& c)
				{
					line = m_caretBeginLine;
					c = m_caretBeginPosition;
				}

				inline void getCaretEnd(u32& line, u32& c)
				{
					line = m_caretEndLine;
					c = m_caretEndPosition;
				}

				inline u32 getNumLine()
				{
					return m_lines.size();
				}

				CText* getLine(u32 line)
				{
					if (line >= m_lines.size())
						return NULL;

					ListTextControl::iterator i = m_lines.begin();
					std::advance(i, line);
					return (*i);
				}

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

				bool isCharacter(wchar_t c);

				void removeAllLines();

				bool splitWords(std::wstring string, std::vector<std::wstring>& lines, float lineWidth);

				void drawSelection(CText *line, u32 from, u32 to);

				u32 getCharacterPositionAt(u32 line, u32 pos);

				bool updateCaretPosition();

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

				u32 m_caretBegin;

				u32 m_caretEnd;

				SRect m_caretRect;

				float m_caretBlinkSpeed;

				float m_caretBlink;

				bool m_showCaret;

				bool m_moveCaretToEnd;
			};
		}
	}
}