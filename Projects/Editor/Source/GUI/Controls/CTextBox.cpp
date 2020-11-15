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
#include "CTextBox.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTextBox::CTextBox(CBase *base) :
				CScrollControl(base),
				m_caretPosition(0),
				m_caretEnd(0)
			{
				m_innerPanel->setMouseInputEnabled(false);

				m_textContainer = new CTextContainer(this);
				m_textContainer->setPos(4.0f, 4.0f);
				m_textContainer->setInnerPaddingRight(4.0f);
				m_textContainer->showCaret(true);

				showScrollBar(false, false);
			}

			CTextBox::~CTextBox()
			{

			}

			void CTextBox::layout()
			{
				CScrollControl::layout();
			}

			void CTextBox::postLayout()
			{
				CScrollControl::postLayout();
			}

			void CTextBox::setCaretBegin(u32 pos)
			{
				m_caretPosition = pos;
				if (m_caretPosition > m_textContainer->getLength())
					m_caretPosition = m_textContainer->getLength();
			}

			void CTextBox::setCaretEnd(u32 pos)
			{
				m_caretEnd = pos;
				if (m_caretEnd > m_textContainer->getLength())
					m_caretEnd = m_textContainer->getLength();
			}

			void CTextBox::onMouseClickLeft(float x, float y, bool down)
			{
				GUI::SPoint pos = m_textContainer->canvasPosToLocal(SPoint(x, y));

				u32 l = 0;
				u32 c = 0;
				u32 p = m_textContainer->getClosestCharacter(pos, l, c);

				setCaretBegin(p);
				setCaretEnd(p);

				m_textContainer->setCaretBegin(l, c);
				m_textContainer->setCaretEnd(l, c);
			}
		}
	}
}