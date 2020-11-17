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
#include "GUI/Input/CInput.h"
#include "GUI/CGUIContext.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTextBox::CTextBox(CBase *base) :
				CScrollControl(base),
				m_caretPosition(0),
				m_caretEnd(0),
				m_press(false),
				m_editable(true)
			{
				m_innerPanel->setMouseInputEnabled(false);

				setKeyboardInputEnabled(true);

				m_textContainer = new CTextContainer(this);
				m_textContainer->setPos(4.0f, 4.0f);
				m_textContainer->setInnerPaddingRight(4.0f);

				showScrollBar(false, false);
				setCursor(GUI::ECursorType::Beam);
			}

			CTextBox::~CTextBox()
			{

			}

			void CTextBox::think()
			{
				if (m_press)
				{
					SPoint mousePos = CInput::getInput()->getMousePosition();

					// auto scrol if drag up/down
					if (m_canScrollV && !m_vertical->isHidden())
					{
						float dt = CGUIContext::getDeltaTime();
						float delta = 0.0f;
						SPoint pos = canvasPosToLocal(mousePos);

						if (pos.Y < 0.0f)
						{
							float speed = fabsf(pos.Y);
							delta = -0.0005f * dt * speed;
						}
						else if (pos.Y > height())
						{
							float speed = fabsf(pos.Y - height());
							delta = 0.0005f * dt * speed;
						}

						if (delta != 0.0f)
							m_vertical->setScroll(m_vertical->getScroll() + m_vertical->getNudgeAmount() * delta);
					}

					// update drag selection
					{
						GUI::SPoint pos = m_textContainer->canvasPosToLocal(mousePos);

						u32 l = 0;
						u32 c = 0;
						u32 p = m_textContainer->getClosestCharacter(pos, l, c);

						setCaretBegin(p);
						m_textContainer->setCaretBegin(l, c);
					}
				}
			}

			void CTextBox::layout()
			{
				CScrollControl::layout();
			}

			void CTextBox::postLayout()
			{
				CScrollControl::postLayout();
			}

			void CTextBox::onKeyboardFocus()
			{
				if (m_editable)
					m_textContainer->showCaret(true);
			}

			void CTextBox::onLostKeyboardFocus()
			{
				if (m_editable)
					m_textContainer->showCaret(false);
			}

			void CTextBox::setEditable(bool b)
			{
				m_editable = b;

				if (CGUIContext::KeyboardFocus == this)
					m_textContainer->showCaret(b);
				else
					m_textContainer->showCaret(false);
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
				if (down)
				{
					GUI::SPoint pos = m_textContainer->canvasPosToLocal(SPoint(x, y));

					u32 l = 0;
					u32 c = 0;
					u32 p = m_textContainer->getClosestCharacter(pos, l, c);

					setCaretBegin(p);
					m_textContainer->setCaretBegin(l, c);

					if (!CInput::getInput()->IsShiftDown())
					{
						setCaretEnd(p);
						m_textContainer->setCaretEnd(l, c);
					}

					CInput::getInput()->setCapture(this);
				}
				else
				{
					CInput::getInput()->setCapture(NULL);
				}

				m_press = down;
			}

			void CTextBox::onMouseDoubleClickLeft(float x, float y)
			{
				GUI::SPoint pos = m_textContainer->canvasPosToLocal(SPoint(x, y));

				u32 l = 0;
				u32 c = 0;
				u32 from = 0;
				u32 to = 0;
				u32 p = 0;

				p = m_textContainer->getClosestCharacter(pos, l, c);
				p = m_textContainer->getWordAtPosition(l, c, from, to);

				setCaretBegin(p);
				setCaretEnd(p - (to - from));

				m_textContainer->setCaretBegin(l, to);
				m_textContainer->setCaretEnd(l, from);
			}

			void CTextBox::onMouseTripleClickLeft(float x, float y)
			{
				GUI::SPoint pos = m_textContainer->canvasPosToLocal(SPoint(x, y));

				u32 l = 0;
				u32 c = 0;
				u32 p = 0;
				u32 to = 0;

				p = m_textContainer->getClosestCharacter(pos, l, c);
				p = m_textContainer->getLineAtPosition(l, c, to);

				setCaretBegin(p);
				setCaretEnd(p - to);

				m_textContainer->setCaretBegin(l, to);
				m_textContainer->setCaretEnd(l, 0);
			}
		}
	}
}