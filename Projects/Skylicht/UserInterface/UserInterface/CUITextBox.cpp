/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "CUIContainer.h"
#include "CUITextBox.h"
#include "CUIEventManager.h"

namespace Skylicht
{
	namespace UI
	{
		CUITextBox::CUITextBox(CUIContainer* container, CGUIElement* element) :
			CUIBase(container, element),
			m_background(NULL),
			m_text(NULL),
			m_editable(true),
			m_maxLength(64)
		{
			CCanvas* canvas = getCanvas();
			if (canvas)
			{
				m_background = canvas->getGUIByPath(element, "Background");
				m_text = dynamic_cast<CGUIText*>(canvas->getGUIByPath(element, "Text"));
				if (m_text)
					m_text->setEnableTextFormnat(false);
			}
		}

		CUITextBox::CUITextBox(CUIContainer* container, CGUIElement* element, CGUIElement* background, CGUIText* text) :
			CUIBase(container, element),
			m_background(background),
			m_text(text),
			m_editable(true),
			m_maxLength(64)
		{
			if (m_text)
				m_text->setEnableTextFormnat(false);
		}

		CUITextBox::~CUITextBox()
		{

		}

		void CUITextBox::setText(const char* text)
		{
			if (m_text)
				m_text->setText(text);
		}

		void CUITextBox::setText(const wchar_t* text)
		{
			if (m_text)
				m_text->setText(text);
		}

		const char* CUITextBox::getText()
		{
			if (m_text)
				return m_text->getText();
			return "";
		}

		const wchar_t* CUITextBox::getTextW()
		{
			if (m_text)
				return m_text->getTextW();
			return L"";
		}

		int CUITextBox::getTextLength()
		{
			if (m_text)
				return m_text->getTextLength();
			return 0;
		}

		void CUITextBox::onPointerHover(int pointerId, float pointerX, float pointerY)
		{
			if (!m_isPointerHover)
			{
				gui::ICursorControl* cursorControl = getIrrlichtDevice()->getCursorControl();
				cursorControl->setActiveIcon(gui::ECI_IBEAM);
			}

			CUIBase::onPointerHover(pointerId, pointerX, pointerY);
		}

		void CUITextBox::onPointerOut(int pointerId, float pointerX, float pointerY)
		{
			CUIBase::onPointerOut(pointerId, pointerX, pointerY);

			gui::ICursorControl* cursorControl = getIrrlichtDevice()->getCursorControl();
			cursorControl->setActiveIcon(gui::ECI_NORMAL);
		}

		void CUITextBox::onPointerDown(int pointerId, float pointerX, float pointerY)
		{
			if (m_pointerId == -1)
			{
				CUIBase::onPointerDown(pointerId, pointerX, pointerY);

				if (m_text)
				{
					int l, c;
					m_text->getClosestCharacter(pointerX, pointerY, l, c);

					m_text->showCaret(true);
					m_text->setCaret(l, c);

					CUIEventManager* eventMgr = CUIEventManager::getInstance();
					eventMgr->setFocus(this);
					eventMgr->setCapture(m_pointerId, this);
				}
			}
		}

		void CUITextBox::onPointerUp(int pointerId, float pointerX, float pointerY)
		{
			if (pointerId == m_pointerId)
			{
				if (m_text)
					CUIEventManager::getInstance()->setCapture(m_pointerId, NULL);

				CUIBase::onPointerUp(pointerId, pointerX, pointerY);
			}
		}

		void CUITextBox::onPointerMove(int pointerId, float pointerX, float pointerY)
		{
			CUIBase::onPointerMove(pointerId, pointerX, pointerY);
			if (m_isPointerDown && m_pointerId == pointerId && m_text)
			{
				int l, c;
				m_text->getClosestCharacter(pointerX, pointerY, l, c);
				m_text->setCaret(l, c);
			}
		}

		void CUITextBox::onLostFocus()
		{
			m_text->showCaret(false);
		}

		void CUITextBox::onKeyEvent(const SEvent& event)
		{
			CUIBase::onKeyEvent(event);

			if (!m_text)
				return;

			if (event.KeyInput.PressedDown)
			{
				bool onChar = true;
				bool updateCaret = true;

				int line = 0, pos = 0;
				m_text->getCaretPosition(line, pos);

				int totalLine = m_text->getNumLine();

				switch (event.KeyInput.Key)
				{
				case irr::KEY_UP:
				{
					if (line > 0)
						line--;
				}
				break;
				case irr::KEY_DOWN:
				{
					if (line < totalLine - 1)
						line++;
				}
				break;
				case irr::KEY_LEFT:
				{
					int length = m_text->getNumCharacter(line);
					pos = core::min_(pos, length);

					if (pos == 0)
					{
						if (line > 0)
						{
							// end prev line
							line--;
							pos = m_text->getNumCharacter(line);
						}
					}
					else
					{
						if (event.KeyInput.Control)
						{
							int from, to;
							m_text->getWordAtPosition(line, pos - 1, from, to);
							pos = from;
						}
						else
						{
							pos--;
						}
					}
				}
				break;
				case irr::KEY_RIGHT:
				{
					int length = m_text->getNumCharacter(line);
					if (pos >= length && line < totalLine - 1)
					{
						// begin next line
						pos = 0;
						line++;
					}
					else
					{
						if (event.KeyInput.Control)
						{
							int from, to;
							m_text->getWordAtPosition(line, pos + 1, from, to);
							pos = to;
						}
						else
							pos++;
					}
				}
				break;
				case irr::KEY_HOME:
					if (event.KeyInput.Control)
					{
						line = 0;
						pos = 0;
					}
					else
					{
						pos = 0;
					}
					break;
				case irr::KEY_END:
					if (event.KeyInput.Control)
					{
						line = totalLine - 1;
						pos = m_text->getNumCharacter(line);
					}
					else
					{
						pos = m_text->getNumCharacter(line);
					}
					break;
				case irr::KEY_PRIOR:
				{
					// page up
					/*
					u32 up = (u32)(height() / m_textContainer->getLine(line)->height());
					if (up > 1)
						up--;

					if (line > up)
						line -= up;
					else
						line = 0;
					*/
				}
				break;
				case irr::KEY_NEXT:
				{
					// page down
					/*
					u32 down = (u32)(height() / m_textContainer->getLine(line)->height());
					if (down > 1)
						down--;

					if (line < totalLine - 1 - down)
						line += down;
					else
						line = totalLine - 1;
					*/
				}
				break;
				case irr::KEY_BACK:
					if (m_editable)
					{
						m_text->doBackspace();

						updateCaret = false;
						onChar = false;

						if (OnTextChanged != nullptr)
							OnTextChanged(this);
					}
					break;
				case irr::KEY_DELETE:
					if (m_editable)
					{
						m_text->doDelete();

						updateCaret = false;
						onChar = false;

						if (OnTextChanged != nullptr)
							OnTextChanged(this);
					}
					break;
				default:
					updateCaret = false;
					break;
				}

				if (updateCaret == true)
				{
					// update new caret position
					m_text->setCaret(line, pos);

					if (event.KeyInput.Shift)
					{
						// update select caret
					}

					// auto scroll to caret position
					// scrollToLine(line, pos);
				}
				else if (onChar && event.KeyInput.Char > 0)
				{
					if (m_text->getTextLength() < m_maxLength)
						m_text->insert(event.KeyInput.Char);
				}
			}
		}
	}
}


