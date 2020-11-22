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
#include "CCanvas.h"
#include "GUI/Input/CInput.h"
#include "GUI/Clipboard/CClipboard.h"
#include "GUI/Theme/CTheme.h"
#include "GUI/Theme/CThemeConfig.h"
#include "GUI/CGUIContext.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTextBox::CTextBox(CBase *base) :
				CScrollControl(base),
				m_press(false),
				m_editable(true),
				m_drawTextbox(true),
				m_textBoxColor(CThemeConfig::TextBoxColor)
			{
				setSize(100.0f, 20.0f);

				m_innerPanel->setMouseInputEnabled(false);

				setKeyboardInputEnabled(true);

				m_icon = new CIcon(this, ESystemIcon::Search);
				m_icon->setPos(2.0f, 0.0f);
				m_icon->setHidden(true);

				m_textHint = new CTextContainer(this);
				m_textHint->setPos(4.0f, 3.0f);
				m_textHint->setInnerPaddingRight(4.0f);
				m_textHint->setColor(CThemeConfig::TextHintColor);

				m_textContainer = new CTextContainer(this);
				m_textContainer->setPos(4.0f, 3.0f);
				m_textContainer->setInnerPaddingRight(4.0f);

				m_contextMenu = new CMenu(getCanvas());
				m_contextMenu->addItem(L"Cut", L"Ctrl + X");
				m_contextMenu->addItem(L"Copy", ESystemIcon::Copy, L"Ctrl + C");
				m_contextMenu->addItem(L"Paste", L"Ctrl + V");
				m_contextMenu->addSeparator();
				m_contextMenu->addItem(L"Select All", L"Ctrl + A");
				m_contextMenu->OnOpen = BIND_LISTENER(&CTextBox::onOpenMenuContext, this);
				m_contextMenu->OnCommand = BIND_LISTENER(&CTextBox::onOpenMenuCommand, this);

				setWrapMultiline(false);

				showScrollBar(false, false);

				setCursor(GUI::ECursorType::Beam);

				addAccelerator("Ctrl + C", BIND_LISTENER(&CTextBox::onCopy, this));
				addAccelerator("Ctrl + X", BIND_LISTENER(&CTextBox::onCut, this));
				addAccelerator("Ctrl + V", BIND_LISTENER(&CTextBox::onPaste, this));
				addAccelerator("Ctrl + A", BIND_LISTENER(&CTextBox::onSelectAll, this));
			}

			CTextBox::~CTextBox()
			{

			}

			void CTextBox::showIcon(ESystemIcon icon)
			{
				m_icon->setIcon(icon);
				m_textContainer->setPos(24.0f, 3.0f);
				m_textHint->setPos(24.0f, 3.0f);
			}

			void CTextBox::showIcon()
			{
				m_icon->setHidden(false);
				m_textContainer->setPos(24.0f, 3.0f);
				m_textHint->setPos(24.0f, 3.0f);
			}

			void CTextBox::hideIcon()
			{
				m_icon->setHidden(true);
				m_textContainer->setPos(4.0f, 3.0f);
				m_textHint->setPos(4.0f, 3.0f);
			}

			void CTextBox::renderUnder()
			{
				if (m_drawTextbox)
				{
					CTheme *theme = CTheme::getTheme();
					theme->drawTextbox(getRenderBounds(), m_textBoxColor);

					if (m_textContainer->isActivate())
						theme->drawTextboxBorder(getRenderBounds(), CThemeConfig::ButtonPressColor);
					else
					{
						if (isHovered())
							theme->drawTextboxBorder(getRenderBounds(), CThemeConfig::ButtonHoverColor);
						else
							theme->drawTextboxBorder(getRenderBounds(), CThemeConfig::TextBoxBorderColor);
					}
				}
				else
				{
					CScrollControl::renderUnder();
				}
			}

			void CTextBox::render()
			{
				if (!m_drawTextbox)
				{
					CScrollControl::render();
				}
			}

			void CTextBox::think()
			{
				if (m_press)
				{
					SPoint mousePos = CInput::getInput()->getMousePosition();

					// auto scrol if drag up/down
					if (m_canScrollV)
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

						m_textContainer->setCaretBegin(l, c);
						m_textContainer->resetCaretBlink();
					}
				}

				// hide hint
				if (m_textContainer->getString().length() > 0)
				{
					m_textHint->setHidden(true);
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

				m_textContainer->setActivate(true);
				m_textHint->setHidden(true);

				CScrollControl::onKeyboardFocus();
			}

			void CTextBox::onLostKeyboardFocus()
			{
				if (m_editable)
					m_textContainer->showCaret(false);
				m_textContainer->setActivate(false);

				if (m_textContainer->getString().length() == 0)
					m_textHint->setHidden(false);

				CScrollControl::onLostKeyboardFocus();
			}

			bool CTextBox::onKeyPress(EKey key, bool press)
			{
				if (press)
				{
					bool updateCaret = true;

					u32 line, pos;
					m_textContainer->getCaretBegin(line, pos);

					u32 totalLine = m_textContainer->getNumLine();

					switch (key)
					{
					case EKey::KEY_UP:
					{
						if (line > 0)
							line--;
					}
					break;
					case EKey::KEY_DOWN:
					{
						if (line < totalLine - 1)
							line++;
					}
					break;
					case EKey::KEY_LEFT:
					{
						u32 length = m_textContainer->getLine(line)->getLengthNoNewLine();
						pos = core::min_(pos, length);

						if (pos == 0)
						{
							if (line > 0)
							{
								// end prev line
								line--;
								pos = m_textContainer->getLine(line)->getLengthNoNewLine();
							}
						}
						else
						{
							if (CInput::getInput()->IsControlDown())
							{
								u32 from, to;
								m_textContainer->getWordAtPosition(line, pos - 1, from, to);
								pos = from;
							}
							else
							{
								pos--;
							}
						}
					}
					break;
					case EKey::KEY_RIGHT:
					{
						CText* text = m_textContainer->getLine(line);
						u32 length = text->getLengthNoNewLine();
						if (pos >= length && line < totalLine - 1)
						{
							// begin next line
							pos = 0;
							line++;
						}
						else
						{
							if (CInput::getInput()->IsControlDown())
							{
								u32 from, to;
								m_textContainer->getWordAtPosition(line, pos + 1, from, to);
								pos = to;
							}
							else
								pos++;
						}
					}
					break;
					case EKey::KEY_HOME:
						if (CInput::getInput()->IsControlDown())
						{
							line = 0;
							pos = 0;
						}
						else
						{
							pos = 0;
						}
						break;
					case EKey::KEY_END:
						if (CInput::getInput()->IsControlDown())
						{
							line = totalLine - 1;
							pos = m_textContainer->getLine(line)->getLengthNoNewLine();
						}
						else
						{
							pos = m_textContainer->getLine(line)->getLengthNoNewLine();
						}
						break;
					case EKey::KEY_PRIOR:
					{
						// page up
						u32 up = (u32)(height() / m_textContainer->getLine(line)->height());
						if (up > 1)
							up--;

						if (line > up)
							line -= up;
						else
							line = 0;
					}
					break;
					case EKey::KEY_NEXT:
					{
						// page down
						u32 down = (u32)(height() / m_textContainer->getLine(line)->height());
						if (down > 1)
							down--;

						if (line < totalLine - 1 - down)
							line += down;
						else
							line = totalLine - 1;
					}
					break;
					case EKey::KEY_DELETE:
						if (m_editable)
						{
							m_textContainer->doDelete();
							updateCaret = false;
						}
						break;
					default:
						updateCaret = false;
						break;
					}

					if (updateCaret == true)
					{
						// update new caret position
						m_textContainer->setCaretBegin(line, pos);
						m_textContainer->resetCaretBlink();

						if (!CInput::getInput()->IsShiftDown())
							m_textContainer->setCaretEnd(line, pos);

						// auto scroll to caret position
						scrollToLine(line, pos);
					}
				}

				return CBase::onKeyPress(key);
			}

			bool CTextBox::onChar(u32 c)
			{
				if (m_editable)
				{
					if (c == '\b')
					{
						m_textContainer->doBackspace();
					}
					else if (c == '\r')
					{
						if (m_textContainer->isWrapMultiline())
							m_textContainer->doInsertCharacter('\n');
					}
					else
					{
						m_textContainer->doInsertCharacter(c);
					}

					if (m_textContainer->isWrapMultiline() == false)
					{
						// single line, we scroll horizontal to caret
						u32 line = 0, pos = 0;
						m_textContainer->getCaretBegin(line, pos);
						scrollToLine(line, pos);
					}
				}

				return CBase::onChar(c);
			}

			void CTextBox::scrollToLine(u32 line, u32 pos)
			{
				CText *text = m_textContainer->getLine(line);

				SDimension s = text->getCharacterPosition(pos);
				SPoint localCaretPosition(s.Width, 0.0f);

				SPoint p = canvasPosToLocal(text->localPosToCanvas(localCaretPosition));

				if (p.Y < 0)
				{
					scrollVerticalOffset(p.Y);
				}
				else if (p.Y + text->height() > height())
				{
					scrollVerticalOffset(p.Y + text->height() - height());
				}

				if (p.X < 0)
				{
					scrollHorizontalOffset(p.X);
				}
				else if (p.X > width())
				{
					scrollHorizontalOffset(p.X + m_widthScrollExpand - width());
				}
			}

			void CTextBox::setEditable(bool b)
			{
				m_editable = b;

				if (CGUIContext::KeyboardFocus == this)
					m_textContainer->showCaret(b);
				else
					m_textContainer->showCaret(false);
			}

			void CTextBox::onMouseClickRight(float x, float y, bool down)
			{
				if (down == false)
				{
					m_contextMenu->open(SPoint(x, y));
				}
			}

			void CTextBox::onMouseClickLeft(float x, float y, bool down)
			{
				if (down)
				{
					GUI::SPoint pos = m_textContainer->canvasPosToLocal(SPoint(x, y));

					u32 l = 0;
					u32 c = 0;
					u32 p = m_textContainer->getClosestCharacter(pos, l, c);

					m_textContainer->setCaretBegin(l, c);
					m_textContainer->resetCaretBlink();

					if (!CInput::getInput()->IsShiftDown())
						m_textContainer->setCaretEnd(l, c);

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

				m_textContainer->setCaretBegin(l, to);
				m_textContainer->setCaretEnd(l, from);
				m_textContainer->resetCaretBlink();
			}

			void CTextBox::onMouseTripleClickLeft(float x, float y)
			{
				GUI::SPoint pos = m_textContainer->canvasPosToLocal(SPoint(x, y));

				u32 l = 0;
				u32 c = 0;
				u32 p = 0;
				u32 to = 0;

				p = m_textContainer->getClosestCharacter(pos, l, c);
				p = m_textContainer->getEndLinePosition(l, to);

				m_textContainer->setCaretBegin(l, to);
				m_textContainer->setCaretEnd(l, 0);
				m_textContainer->resetCaretBlink();
			}

			void CTextBox::onCopy(CBase *base)
			{
				std::wstring data;
				m_textContainer->getSelectString(data);
				if (data.size() > 0)
					CClipboard::get()->copyTextToClipboard(data);
			}

			void CTextBox::onCut(CBase *base)
			{
				if (m_editable)
				{
					std::wstring data;
					m_textContainer->getSelectString(data);
					if (data.size() > 0)
					{
						CClipboard::get()->copyTextToClipboard(data);
						m_textContainer->doDeleteTextInSelection();
					}
				}
			}

			void CTextBox::onPaste(CBase *base)
			{
				if (m_editable)
				{
					std::wstring text = CClipboard::get()->getTextFromClipboard();
					if (text.length() > 0)
					{
						m_textContainer->doInsertString(text);
					}

					if (m_textContainer->isWrapMultiline() == false)
					{
						// single line, we scroll horizontal to caret
						u32 line = 0, pos = 0;
						m_textContainer->getCaretBegin(line, pos);
						scrollToLine(line, pos);
					}
				}
			}

			void CTextBox::onSelectAll(CBase *base)
			{
				m_textContainer->setCaretEnd(0, 0);

				u32 totalLine = m_textContainer->getNumLine();
				u32 line = totalLine - 1;
				u32 pos = m_textContainer->getLine(line)->getLengthNoNewLine();
				m_textContainer->setCaretBegin(line, pos);
				scrollToLine(line, pos);
			}

			void CTextBox::onOpenMenuContext(CBase *base)
			{
				if (base != m_contextMenu)
					return;

				if (m_editable)
				{
					m_contextMenu->getItemByLabel(L"Cut")->setDisabled(false);
					m_contextMenu->getItemByLabel(L"Paste")->setDisabled(false);
				}
				else
				{
					m_contextMenu->getItemByLabel(L"Cut")->setDisabled(true);
					m_contextMenu->getItemByLabel(L"Paste")->setDisabled(true);
				}
			}

			void CTextBox::onOpenMenuCommand(CBase *base)
			{
				CMenuItem* item = dynamic_cast<CMenuItem*>(base);
				if (item != NULL)
				{
					std::wstring label = item->getLabel();
					if (label == L"Cut")
						onCut(base);
					else if (label == L"Copy")
						onCopy(base);
					else if (label == L"Paste")
						onPaste(base);
					else if (label == L"Select All")
						onSelectAll(base);

					focus();
				}
			}
		}
	}
}