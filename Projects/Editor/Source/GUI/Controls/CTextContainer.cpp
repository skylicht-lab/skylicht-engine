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
#include "CTextContainer.h"
#include "GUI/Renderer/CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CTextContainer::CTextContainer(CBase *parent) :
				CBase(parent),
				m_wrapMultiLine(false),
				m_textChange(true),
				m_fontSize(EFontSize::SizeNormal)
			{
				setMouseInputEnabled(false);
				enableClip(true);
			}

			CTextContainer::~CTextContainer()
			{

			}

			void CTextContainer::setWrap(bool b)
			{
				if (m_wrapMultiLine == b)
					return;

				m_wrapMultiLine = b;
				m_textChange = true;
				invalidate();
			}

			void CTextContainer::setFontSize(EFontSize size)
			{
				if (m_fontSize == size)
					return;

				m_fontSize = size;
				m_textChange = true;
				invalidate();
			}

			void CTextContainer::setString(const std::wstring& string)
			{
				if (m_string == string)
					return;

				m_string = string;
				m_textChange = true;
				invalidate();
			}

			void CTextContainer::layout()
			{
				if (m_textChange == true)
				{
					refreshSize();
					m_textChange = false;
				}
			}

			void CTextContainer::refreshSize()
			{
				removeAllLines();

				// this is not yet layout, so calc from parent (Label, Edit,...)
				float w = m_parent->width() - m_parent->getPadding().Left - m_parent->getPadding().Right;
				float x = 0.0f;
				float y = 0.0f;
				std::wstring strLine;

				if (m_wrapMultiLine == true)
				{
					// multi line
					std::vector<std::wstring> words;

					splitWords(m_string, words, w);
					words.push_back(L"");

					for (auto&& it = words.begin(); it != words.end(); ++it)
					{
						bool finishLine = false;
						bool wrapped = false;

						// If this word is a newline - make a newline (we still add it to the
						// text)
						if ((*it).c_str()[0] == '\n')
							finishLine = true;

						// Does adding this word drive us over the width?
						strLine += *it;
						SDimension p = CRenderer::getRenderer()->measureText(m_fontSize, strLine);

						if (p.Width > w)
						{
							finishLine = true;
							wrapped = true;
						}

						// If this is the last word then finish the line
						if (--words.end() == it)
							finishLine = true;

						if (finishLine)
						{
							CText* t = new CText(this);
							t->setFontSize(m_fontSize);
							t->setColor(m_color);

							if (wrapped)
							{
								t->setString(strLine.substr(0, strLine.length() - (*it).length()));
								// newline should start with the word that was too big
								strLine = *it;
							}
							else
							{
								t->setString(strLine.substr(0, strLine.length()));
								//new line is empty
								strLine.clear();
							}
							t->setBounds(x, y, w, p.Height);
							m_lines.push_back(t);

							// Position the newline
							y += p.Height;
							x = 0;
						}
					}
				}
				else
				{
					// compute size of text
					SDimension p = CRenderer::getRenderer()->measureText(m_fontSize, m_string);

					// single line
					CText* t = new CText(this);
					t->setFontSize(m_fontSize);
					t->setColor(m_color);
					t->setBounds(x, y, p.Width, p.Height);
					t->setString(m_string);
					m_lines.push_back(t);
				}

				// Size to children height and parent width
				{
					SDimension childsize = getChildrenSize();
					setSize(w, childsize.Height);
				}

				invalidateParent();
				invalidate();
			}

			void CTextContainer::splitWords(std::wstring s, std::vector<std::wstring>& lines, float lineWidth)
			{
				std::wstring str;

				for (size_t i = 0; i < s.length(); i++)
				{
					if (s[i] == '\n')
					{
						if (!str.empty())
							lines.push_back(str);

						lines.push_back(L"\n");
						str.clear();
						continue;
					}

					if (s[i] == ' ')
					{
						str += s[i];
						lines.push_back(str);
						str.clear();
						continue;
					}

					str += s[i];

					// if adding character makes the word bigger than the textbox size
					SDimension p = CRenderer::getRenderer()->measureText(m_fontSize, str);
					if (p.Width > lineWidth)
					{
						//split words
						str.pop_back();
						lines.push_back(str);
						str.clear();
						--i;
						continue;
					}
				}

				if (!str.empty())
					lines.push_back(str);
			}

			void CTextContainer::removeAllLines()
			{
				std::list<CText*>::iterator i = m_lines.begin(), end = m_lines.end();
				while (i != end)
				{
					CText *t = (*i);
					removeChild(t);
					delete t;
					++i;
				}
				m_lines.clear();
			}
		}
	}
}