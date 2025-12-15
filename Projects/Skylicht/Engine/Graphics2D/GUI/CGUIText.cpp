/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CGUIText.h"
#include "Utils/CStringImp.h"
#include "Graphics2D/CGraphics2D.h"
#include "Graphics2D/SpriteFrame/CFontManager.h"
#include "Graphics2D/CCanvas.h"

namespace Skylicht
{
	CGUIText::CGUIText(CCanvas* canvas, CGUIElement* parent, IFont* font) :
		CGUIElement(canvas, parent),
		TextVertical(EGUIVerticalAlign::Top),
		TextHorizontal(EGUIHorizontalAlign::Left),
		m_multiLine(true),
		m_centerRotate(false),
		m_strim(false),
		m_charPadding(0),
		m_charSpacePadding(0),
		m_linePadding(0),
		m_updateTextRender(true),
		m_font(NULL),
		m_customfont(font),
		m_fontData(NULL),
		m_lastWidth(0.0f),
		m_lastHeight(0.0f),
		m_fontChanged(-1),
		m_enableTextFormat(true)
#ifdef HAVE_CARET
		, m_showCaret(false),
		m_setCaret(-1),
		m_caretShader(0),
		m_caretBlink(0.0f),
		m_caretBlinkSpeed(500.0f)
#endif
	{
		m_enableMaterial = true;

		init();
	}

	CGUIText::CGUIText(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect, IFont* font) :
		CGUIElement(canvas, parent, rect),
		TextVertical(EGUIVerticalAlign::Top),
		TextHorizontal(EGUIHorizontalAlign::Left),
		m_multiLine(true),
		m_centerRotate(false),
		m_strim(false),
		m_charPadding(0),
		m_charSpacePadding(0),
		m_linePadding(0),
		m_updateTextRender(true),
		m_font(NULL),
		m_customfont(font),
		m_fontData(NULL),
		m_lastWidth(0.0f),
		m_lastHeight(0.0f),
		m_fontChanged(-1),
		m_enableTextFormat(true)
#ifdef HAVE_CARET
		, m_showCaret(false),
		m_caretShader(0),
		m_caretBlink(0.0f),
		m_caretBlinkSpeed(500.0f)
#endif
	{
		init();
	}

	void CGUIText::init()
	{
		m_textOffsetY = 0;
		m_textHeight = 50;

#ifdef HAVE_CARET
		m_caretShader = CShaderManager::getInstance()->getShaderIDByName("VertexColorAlpha");
#endif

		for (int i = 0; i < MAX_FORMATCOLOR; i++)
			m_colorFormat[i].set(255, 255, 255, 255);
	}

	CGUIText::~CGUIText()
	{

	}

	void CGUIText::initFont(IFont* font)
	{
		// get text height
		SModuleOffset* moduleCharA = font->getCharacterModule((int)'A');
		if (moduleCharA)
		{
			m_textHeight = (int)moduleCharA->OffsetY + (int)moduleCharA->Module->H;
			m_textOffsetY = (int)moduleCharA->OffsetY;
		}
	}

	IFont* CGUIText::getCurrentFont()
	{
		if (m_customfont)
			return m_customfont;

		if (m_fontData)
			return m_fontData->getFont();

		return NULL;
	}

	void CGUIText::setFormatText(const char* formatText)
	{
		if (!m_enableTextFormat)
		{
			// just render all like "<formatId>text"
			setText(formatText);
			return;
		}

		int i = 0;
		int j = 0;

		char number[64];
		number[0] = 0;

		int iNumber = 0;
		int currentFormat = 0;


		int numUnicode = CStringImp::getUnicodeStringSize(formatText);
		wchar_t* textw = new wchar_t[numUnicode];
		CStringImp::convertUTF8ToUnicode(formatText, textw);

		m_textw = L"";
		m_textFormat.clear();

		// processing
		while (textw[i] != 0)
		{
			if (textw[i] == '<')
			{
				// todo need parse variable
				iNumber = 0;
				i++;

				while (textw[i] != 0)
				{
					if (textw[i] == '>')
					{
						number[iNumber] = 0;
						currentFormat = atoi(number);

						if (currentFormat >= MAX_FORMATCOLOR)
							currentFormat = 0;

						break;
					}
					else
					{
						// copy {value} to number
						number[iNumber++] = (char)textw[i];
					}
					i++;
				}
			}
			else
			{
				m_textw += textw[i];
				m_textFormat.push_back(currentFormat);
			}
			++i;
		}

		// utf8 string
		int numUTF = CStringImp::getUTF8StringSize(m_textw.c_str());

		char* texta = new char[numUTF + 2];
		CStringImp::convertUnicodeToUTF8(m_textw.c_str(), texta);
		m_text = texta;

		delete[]textw;
		delete[]texta;

		m_updateTextRender = true;
	}

	void CGUIText::setTextId(const char* textId)
	{
		m_textId = textId;
		m_textwId = CStringImp::convertUTF8ToUnicode(textId);
	}

	void CGUIText::setTextId(const wchar_t* textId)
	{
		m_textw = textId;
		m_textId = CStringImp::convertUnicodeToUTF8(textId);
	}

	void CGUIText::setText(const char* text)
	{
		m_text = text;
		int numUnicode = CStringImp::getUnicodeStringSize(text);

		m_textw = CStringImp::convertUTF8ToUnicode(text);

		int i = 0;
		m_textFormat.clear();
		while (m_textw[i] != 0)
		{
			m_textFormat.push_back(0);
			++i;
		}

		m_updateTextRender = true;
	}

	void CGUIText::setTextStrim(const char* text)
	{
		int numUnicode = CStringImp::getUnicodeStringSize(text);

		wchar_t* textw = new wchar_t[numUnicode];
		CStringImp::convertUTF8ToUnicode(text, textw);

		int i = 0;
		int stringWidth = 0;
		int maxStringWidth = (int)getRect().getWidth();

		while (textw[i] != 0)
		{
			int w = getCharWidth(textw[i]);

			if (stringWidth + w < maxStringWidth)
				stringWidth += w;
			else
			{
				// todo: add ... at the end of string
				textw[i] = 0;
				if (i - 1 > 0)
					textw[i - 1] = L'.';
				if (i - 2 > 0)
					textw[i - 2] = L'.';
				if (i - 3 > 0)
					textw[i - 3] = L'.';
				break;
			}
			i++;
		}

		// unicode string
		m_textw = textw;

		// string format
		i = 0;
		m_textFormat.clear();
		while (textw[i] != 0)
		{
			m_textFormat.push_back(0);
			++i;
		}

		// utf8 string
		int numUTF = CStringImp::getUTF8StringSize(textw);
		char* texta = new char[numUTF + 2];
		CStringImp::convertUnicodeToUTF8(textw, texta);
		m_text = texta;

		delete[]texta;
		delete[]textw;

		m_updateTextRender = true;
	}

	void CGUIText::setTextStrim(const wchar_t* text)
	{
		int numUnicode = CStringImp::length<const wchar_t>(text);

		wchar_t* textw = new wchar_t[numUnicode];
		CStringImp::copy<wchar_t, const wchar_t>(textw, text);

		int i = 0;
		int stringWidth = 0;
		int maxStringWidth = (int)getRect().getWidth();

		while (textw[i] != 0)
		{
			int w = getCharWidth(textw[i]);

			if (stringWidth + w < maxStringWidth)
				stringWidth += w;
			else
			{
				// todo: add ... at the end of string
				textw[i] = 0;
				if (i - 1 > 0)
					textw[i - 1] = L'.';
				if (i - 2 > 0)
					textw[i - 2] = L'.';
				if (i - 3 > 0)
					textw[i - 3] = L'.';
				break;
			}
			i++;
		}

		// unicode string
		m_textw = textw;

		// string format
		i = 0;
		m_textFormat.clear();
		while (textw[i] != 0)
		{
			m_textFormat.push_back(0);
			++i;
		}

		// utf8 string
		int numUTF = CStringImp::getUTF8StringSize(textw);
		char* texta = new char[numUTF + 2];
		CStringImp::convertUnicodeToUTF8(textw, texta);
		m_text = texta;

		delete[]texta;
		delete[]textw;

		m_updateTextRender = true;
	}

	void CGUIText::setText(const wchar_t* text)
	{
		m_textw = text;
		int numUTF = CStringImp::getUTF8StringSize(text);

		char* texta = new char[numUTF + 2];
		CStringImp::convertUnicodeToUTF8(text, texta);
		m_text = texta;

		delete[]texta;

		int i = 0;
		m_textFormat.clear();
		while (text[i] != 0)
		{
			m_textFormat.push_back(0);
			++i;
		}

		m_updateTextRender = true;
	}

	void CGUIText::setDefaultText(const char* text)
	{
		m_defaultText = text;
		setText(text);
	}

	void CGUIText::setDefaultText(const wchar_t* text)
	{
		m_defaultText = CStringImp::convertUnicodeToUTF8(text);
		setText(text);
	}

	int CGUIText::getCharWidth(wchar_t c)
	{
		std::wstring string;
		ArrayInt format;

		string += c;
		format.push_back(0);

		std::vector<SModuleOffset*>	listModule;
		std::vector<int> listFormat;

		m_font->getListModule(string.c_str(), format, listModule, listFormat);

		int stringWidth = 0;
		for (int i = 0, n = (int)listModule.size(); i < n; i++)
		{
			SModuleOffset* moduleOffset = listModule[i];
			if (moduleOffset->Character == ' ')
				stringWidth += ((int)moduleOffset->XAdvance + m_charSpacePadding);
			else
				stringWidth += ((int)moduleOffset->XAdvance + m_charPadding);
		}

		return stringWidth;
	}

	int CGUIText::getStringWidth(const char* text)
	{
		ArrayInt format;

		wchar_t wtext[1024] = { 0 };
		CStringImp::convertUTF8ToUnicode(text, wtext);

		int i = 0;
		while (wtext[i] != 0)
		{
			format.push_back(0);
			++i;
		}

		int stringWidth = 0;

		std::vector<SModuleOffset*>	listModule;
		std::vector<int> listFormat;

		IFont* font = getCurrentFont();
		if (font == NULL)
			return stringWidth;

		font->getListModule(wtext, format, listModule, listFormat);

		for (int i = 0, n = (int)listModule.size(); i < n; i++)
		{
			SModuleOffset* moduleOffset = listModule[i];
			if (moduleOffset->Character == ' ')
				stringWidth += ((int)moduleOffset->XAdvance + m_charSpacePadding);
			else
				stringWidth += ((int)moduleOffset->XAdvance + m_charPadding);
		}

		return stringWidth;
	}

	int CGUIText::getStringWidth(const wchar_t* wtext)
	{
		ArrayInt format;

		int i = 0;
		while (wtext[i] != 0)
		{
			format.push_back(0);
			++i;
		}

		int stringWidth = 0;

		std::vector<SModuleOffset*>	listModule;
		std::vector<int> listFormat;

		IFont* font = getCurrentFont();
		if (font == NULL)
			return stringWidth;

		font->getListModule(wtext, format, listModule, listFormat);

		for (int i = 0, n = (int)listModule.size(); i < n; i++)
		{
			SModuleOffset* moduleOffset = listModule[i];
			if (moduleOffset->Character == ' ')
				stringWidth += ((int)moduleOffset->XAdvance + m_charSpacePadding);
			else
				stringWidth += ((int)moduleOffset->XAdvance + m_charPadding);
		}

		return stringWidth;
	}

#ifdef HAVE_CARET
	void CGUIText::updateCaret()
	{
		if (m_showCaret)
		{
			m_caretBlink = m_caretBlink + getTimeStep();
			if (m_caretBlink > 2.0f * m_caretBlinkSpeed)
				m_caretBlink = 0.0f;
		}
		else
		{
			m_caretBlink = 0.0f;
		}
	}

	void CGUIText::updateSetCaret()
	{
		bool set = false;
		int numLine = (int)m_arrayCharId.size();

		for (int l = 0; l < numLine; l++)
		{
			ArrayInt& line = m_arrayCharId[l];

			for (int i = 0, n = (int)line.size() - 1; i < n; i++)
			{
				if (m_setCaret <= line[i] || (line[i] <= m_setCaret && m_setCaret < line[i + 1]))
				{
					m_caret.set(i, l);
					set = true;
					break;
				}
			}

			if (set)
				break;
		}

		if (set == false)
		{
			int lastLine = numLine - 1;
			ArrayInt& line = m_arrayCharId[lastLine];
			if (line.size() == 0)
			{
				m_caret.set(0, lastLine);
			}
			else
			{
				if (line.back() == m_setCaret)
					m_caret.set((int)line.size() - 1, lastLine);
				else
					m_caret.set((int)line.size(), lastLine);
			}
		}
	}

	void CGUIText::setCaret(int charPos)
	{
		m_setCaret = charPos;
		m_caretBlink = 0.0f;
		m_updateTextRender = true;
	}

	int CGUIText::getNumCharacter(int line)
	{
		if (line < 0 || line >= (int)m_arrayCharRender.size())
			return 0;

		return (int)m_arrayCharRender[line].size();
	}

	bool CGUIText::isCharacter(wchar_t c)
	{
		if ('a' <= c && c <= 'z')
			return true;

		if ('A' <= c && c <= 'Z')
			return true;

		if ('0' <= c && c <= '9')
			return true;

		return false;
	}

	bool CGUIText::getWordAtPosition(int line, int charPosition, int& from, int& to)
	{
		if (line < 0 || line >= (int)m_arrayCharRender.size())
			return false;

		from = charPosition;
		to = charPosition;

		ArrayModuleOffset& string = m_arrayCharRender[line];
		int numCharacter = (int)string.size();

		for (int i = 0; i < numCharacter; i++)
		{
			if (charPosition < numCharacter)
			{
				bool isCharacterGroup = isCharacter(string[charPosition]->Character);

				from = to = charPosition;
				while (from > 0)
				{
					from--;
					bool g = isCharacter(string[from]->Character);
					if (g != isCharacterGroup)
					{
						from++;
						break;
					}
				}

				while (to < numCharacter)
				{
					bool g = isCharacter(string[to]->Character);
					if (g != isCharacterGroup)
					{
						break;
					}
					to++;
				}
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	void CGUIText::doBackspace()
	{
		int numLine = (int)m_arrayCharRender.size();

		if (m_caret.Y > numLine)
			return;

		if (m_caret.X == 0)
		{
			if (m_caret.Y == 0)
				return;
			else
			{
				m_caret.Y--;
				m_caret.X = (int)m_arrayCharRender[m_caret.Y].size();
			}
		}
		ArrayModuleOffset& string = m_arrayCharRender[m_caret.Y];

		ArrayInt& id = m_arrayCharId[m_caret.Y];
		m_caret.X--;

		int charId = id[m_caret.X];
		if (charId < 0)
			charId = 0;

		m_textw.erase(m_textw.begin() + charId);
		m_textFormat.erase(m_textFormat.begin() + charId);

		m_text = CStringImp::convertUnicodeToUTF8(m_textw.c_str());

		setCaret(charId);
		updateSplitText();
	}

	void CGUIText::doDelete()
	{
		int numLine = (int)m_arrayCharRender.size();

		if (m_caret.Y > numLine)
			return;

		if (m_caret.X >= m_arrayCharRender[m_caret.Y].size())
		{
			if (m_caret.Y < numLine - 1)
			{
				// delete first char at next line
				m_caret.X = 0;
				m_caret.Y++;
			}
			else
				return;
		}

		ArrayModuleOffset& string = m_arrayCharRender[m_caret.Y];

		ArrayInt& id = m_arrayCharId[m_caret.Y];
		int charId = id[m_caret.X];

		m_textw.erase(m_textw.begin() + charId);
		m_textFormat.erase(m_textFormat.begin() + charId);

		m_text = CStringImp::convertUnicodeToUTF8(m_textw.c_str());

		setCaret(charId);
		updateSplitText();
	}

	void CGUIText::insert(wchar_t c)
	{
		int numLine = (int)m_arrayCharRender.size();
		if (m_caret.Y >= numLine)
			return;

		ArrayModuleOffset& string = m_arrayCharRender[m_caret.Y];
		if (m_caret.X >= string.size() && m_caret.Y == numLine - 1)
		{
			m_textw.push_back(c);
			m_textFormat.push_back(0);

			m_text = CStringImp::convertUnicodeToUTF8(m_textw.c_str());

			setCaret((int)m_textw.size());
		}
		else if (m_caret.X >= 0)
		{
			int charId = 0;
			ArrayInt& id = m_arrayCharId[m_caret.Y];
			if (m_caret.X >= id.size())
			{
				m_caret.Y++;
				m_caret.X = 0;
				ArrayInt& id = m_arrayCharId[m_caret.Y];
				charId = id[m_caret.X];
			}
			else
			{
				charId = id[m_caret.X];
			}

			m_textw.insert(m_textw.begin() + charId, c);
			m_textFormat.insert(m_textFormat.begin() + charId, 0);

			m_text = CStringImp::convertUnicodeToUTF8(m_textw.c_str());

			setCaret(charId + 1);
		}

		updateSplitText();
	}
#endif

	void CGUIText::render(CCamera* camera)
	{
		IFont* font = getCurrentFont();

		if (!font || getColor().getAlpha() == 0)
		{
			CGUIElement::render(camera);
			return;
		}

#ifdef HAVE_CARET
		updateCaret();
#endif

		// fix: sometime font address pointer is not change (so font != m_font is not correct)
		bool fontChanged = false;
		if (m_fontData)
		{
			if (m_fontData->getChangeRevision() != m_fontChanged)
			{
				m_fontChanged = m_fontData->getChangeRevision();
				fontChanged = true;
			}
		}

		if (fontChanged || font != m_font)
		{
			initFont(font);
			m_font = font;
			m_updateTextRender = true;
		}

		const core::rectf& rect = getRect();

		if (m_lastWidth != rect.getWidth() || m_lastHeight != rect.getHeight())
		{
			m_lastWidth = rect.getWidth();
			m_lastHeight = rect.getHeight();
			m_updateTextRender = true;
			if (m_strim && !m_defaultText.empty())
				setTextStrim(m_defaultText.c_str());
		}

		if (m_updateTextRender == true)
		{
			updateSplitText();
			m_updateTextRender = false;
		}

		m_font->updateFontTexture();

		// calc multiline height
		int textHeight = (int)m_arrayCharRender.size() * (m_textHeight + m_linePadding);
		textHeight -= m_linePadding;

		int x = (int)rect.UpperLeftCorner.X;
		int y = (int)rect.UpperLeftCorner.Y;

		// calc text algin vertial
		if (TextVertical == EGUIVerticalAlign::Middle)
			y = y + ((int)m_lastHeight - textHeight - m_textOffsetY) / 2;
		else if (TextVertical == EGUIVerticalAlign::Bottom)
			y = y + (int)m_lastHeight - textHeight;

		if (m_centerRotate == true)
			y = y - textHeight / 2;

		// render
		for (int i = 0, n = (int)m_arrayCharRender.size(); i < n; i++)
		{
			// render text
			renderText(m_arrayCharRender[i], m_arrayCharFormat[i], x, y, i);

			// new line
			y += (m_textHeight + m_linePadding);
		}

		CGUIElement::render(camera);
	}

	void CGUIText::renderText(ArrayModuleOffset& string, ArrayInt& stringFormat, int posX, int posY, int line)
	{
		CGraphics2D* g = CGraphics2D::getInstance();

		int x = posX;
		int y = posY;

		int stringWidth = 0;
		int numCharacter = (int)string.size();

		for (int i = 0; i < numCharacter; i++)
		{
			SModuleOffset* moduleOffset = string[i];
			if (moduleOffset->Character == ' ')
				stringWidth += ((int)moduleOffset->XAdvance + m_charSpacePadding);
			else
				stringWidth += ((int)moduleOffset->XAdvance + m_charPadding);
		}

		// text align
		if (TextHorizontal == EGUIHorizontalAlign::Center)
			x = x + ((int)getRect().getWidth() - stringWidth) / 2;
		else if (TextHorizontal == EGUIHorizontalAlign::Right)
			x = x + (int)getRect().getWidth() - stringWidth;

		if (m_centerRotate == true)
			x = x - stringWidth / 2;

		// draw bbox
		// CGraphics::getInstance()->addRectBatch(core::recti(x,y,x+stringWidth,y+stringHeight), SColor(255,255,0,255), AbsoluteTransformation);

#ifdef HAVE_CARET
		if (m_showCaret && m_caretBlink <= m_caretBlinkSpeed)
		{
			int cx = x;

			if (line == m_caret.Y)
			{
				float top = 0;
				float bottom = top + m_textHeight;

				for (int i = 0; i < numCharacter; i++)
				{
					SModuleOffset* moduleOffset = string[i];
					if (moduleOffset->OffsetY < top)
						top = moduleOffset->OffsetY;

					if (moduleOffset->OffsetY + moduleOffset->Module->H > bottom)
						bottom = moduleOffset->OffsetY + moduleOffset->Module->H;
				}

				for (int i = 0; i < numCharacter; i++)
				{
					SModuleOffset* moduleOffset = string[i];

					if (i == m_caret.X)
						break;

					if (moduleOffset->Character == ' ')
						cx += ((int)moduleOffset->XAdvance + m_charSpacePadding);
					else
						cx += ((int)moduleOffset->XAdvance + m_charPadding);
				}

				g->addRectangleBatch(
					core::rectf(
						(float)cx,
						(float)y,
						(float)cx + 2.0f,
						(float)y + bottom
					),
					core::rectf(0.0f, 0.0f, 1.0f, 1.0f),
					getColor(),
					m_transform->World,
					m_caretShader
				);
			}
		}
#endif

		// render string
		for (int i = 0; i < numCharacter; i++)
		{
			SModuleOffset* moduleOffset = string[i];
			int format = stringFormat[i];

			// shadow
			//if (EnableShadow == true)
			//	g->addModuleBatch(moduleOffset, ShadowColor, AbsoluteTransformation, x + ShadowOffsetX, y + ShadowOffsetY, MaterialID);

			// render text with base color if format = 0
			if (format == 0)
				g->addModuleBatch(moduleOffset, getColor(), m_transform->World, (float)x, (float)y, getShaderID(), getMaterial());
			else
				g->addModuleBatch(moduleOffset, m_colorFormat[format], m_transform->World, (float)x, (float)y, getShaderID(), getMaterial());

			if (moduleOffset->Character == ' ')
				x += ((int)moduleOffset->XAdvance + m_charSpacePadding);
			else
				x += ((int)moduleOffset->XAdvance + m_charPadding);
		}
	}

	void CGUIText::updateSplitText()
	{
		// encode string to list modules & format
		m_arrayCharRender.clear();
		m_arrayCharFormat.clear();
		m_arrayCharId.clear();

		if (m_multiLine == true)
		{
			// split to multi line
			splitText(m_arrayCharRender, m_arrayCharFormat, m_arrayCharId, (int)m_lastWidth);
		}
		else
		{
			// copy all string
			ArrayModuleOffset modules;
			ArrayInt fm;
			ArrayInt p;

			const wchar_t* lpString = m_textw.c_str();
			ArrayInt& lpFormat = m_textFormat;
			int i = 0;

			while (lpString[i] != 0)
			{
				SModuleOffset* c = m_font->getCharacterModule((int)lpString[i]);
				if (c != NULL)
				{
					modules.push_back(c);
					fm.push_back(lpFormat[i]);
					p.push_back(i);
				}
				i++;
			}

			m_arrayCharRender.push_back(modules);
			m_arrayCharFormat.push_back(fm);
			m_arrayCharId.push_back(p);
		}

#ifdef HAVE_CARET
		if (m_setCaret >= 0)
		{
			updateSetCaret();
			m_setCaret = -1;
		}
#endif
	}

	void CGUIText::splitText(std::vector<ArrayModuleOffset>& split, std::vector<ArrayInt>& format, std::vector<ArrayInt>& id, int width)
	{
		split.clear();
		format.clear();
		id.clear();

		const wchar_t* lpString = m_textw.c_str();
		ArrayInt& lpFormat = m_textFormat;

		if (m_font == NULL || lpString == NULL)
		{
			m_arrayCharRender.push_back(ArrayModuleOffset());
			m_arrayCharFormat.push_back(ArrayInt());
			m_arrayCharId.push_back(ArrayInt());
			return;
		}

		int i = 0;
		int x = 0;

		int begin = 0;
		int lastSpace = 0;

		while (lpString[i] != 0)
		{
			// new line
			if (lpString[i] == '\n')
			{
				ArrayModuleOffset modules;
				ArrayInt fm;
				ArrayInt p;

				// loop all string on sentence
				for (int j = begin; j < i; j++)
				{
					SModuleOffset* c = m_font->getCharacterModule((int)lpString[j]);
					if (c != NULL)
					{
						modules.push_back(c);
						fm.push_back(lpFormat[j]);
						p.push_back(j);
					}
				}

				// split text
				split.push_back(modules);
				format.push_back(fm);
				id.push_back(p);

				// seek next char
				i++;

				begin = i;
				lastSpace = i;
				x = 0;
				continue;
			}
			else if (lpString[i] == '\\')
			{
				if (lpString[i + 1] == 'n')
				{
					ArrayModuleOffset modules;
					ArrayInt fm;
					ArrayInt p;

					// loop all string on sentence
					for (int j = begin; j < i; j++)
					{
						SModuleOffset* c = m_font->getCharacterModule((int)lpString[j]);
						if (c != NULL)
						{
							modules.push_back(c);
							fm.push_back(lpFormat[j]);
							p.push_back(j);
						}
					}

					// split text
					split.push_back(modules);
					format.push_back(fm);
					id.push_back(p);

					// seek to ansi text
					i += 2;

					begin = i;
					lastSpace = i;
					x = 0;
					continue;
				}
			}

			SModuleOffset* c = m_font->getCharacterModule((int)lpString[i]);
			if (c == NULL)
			{
				i++;
				continue;
			}

			// if found special character
			if (lpString[i] == ' ')
			{
				lastSpace = i;
				i++;

				// space
				x = x + (int)c->XAdvance + m_charPadding;
				continue;
			}

			// ------------------

			if (c)
				x = x + (int)c->XAdvance + m_charPadding;

			// need split
			if (x >= width)
			{
				ArrayModuleOffset modules;
				ArrayInt fm;
				ArrayInt p;

				// ducph: fix stuck loop
				if (begin == lastSpace)
					lastSpace = i + 1;

				for (int j = begin; j < lastSpace; j++)
				{
					SModuleOffset* c = m_font->getCharacterModule((int)lpString[j]);
					if (c != NULL)
					{
						modules.push_back(c);
						fm.push_back(lpFormat[j]);
						p.push_back(j);
					}
				}

				split.push_back(modules);
				format.push_back(fm);
				id.push_back(p);

				i = lastSpace;

				begin = i;
				lastSpace = i;
				x = 0;
				continue;
			}
			i++;
		}

		ArrayModuleOffset modules;
		ArrayInt fm;
		ArrayInt p;

		for (int j = begin; j < i; j++)
		{
			SModuleOffset* c = m_font->getCharacterModule((int)lpString[j]);
			if (c != NULL)
			{
				modules.push_back(c);
				fm.push_back(lpFormat[j]);
				p.push_back(j);
			}
		}

		split.push_back(modules);
		format.push_back(fm);
		id.push_back(p);
	}

	CObjectSerializable* CGUIText::createSerializable()
	{
		CObjectSerializable* object = CGUIElement::createSerializable();
		object->autoRelease(new CBoolProperty(object, "multiline", m_multiLine));
		object->autoRelease(new CIntProperty(object, "charPadding", m_charPadding));
		object->autoRelease(new CIntProperty(object, "charSpacePadding", m_charSpacePadding));
		object->autoRelease(new CIntProperty(object, "linePadding", m_linePadding));

		CEnumProperty<EGUIVerticalAlign>* verticalAlign = new CEnumProperty<EGUIVerticalAlign>(object, "textVerticle", TextVertical);
		verticalAlign->addEnumString("Top", EGUIVerticalAlign::Top);
		verticalAlign->addEnumString("Middle", EGUIVerticalAlign::Middle);
		verticalAlign->addEnumString("Bottom", EGUIVerticalAlign::Bottom);
		object->autoRelease(verticalAlign);

		CEnumProperty<EGUIHorizontalAlign>* horizontalAlign = new CEnumProperty<EGUIHorizontalAlign>(object, "textHorizontal", TextHorizontal);
		horizontalAlign->addEnumString("Left", EGUIHorizontalAlign::Left);
		horizontalAlign->addEnumString("Center", EGUIHorizontalAlign::Center);
		horizontalAlign->addEnumString("Right", EGUIHorizontalAlign::Right);
		object->autoRelease(horizontalAlign);

		object->autoRelease(new CBoolProperty(object, "strim", m_strim));

		object->autoRelease(new CFilePathProperty(object, "font", m_fontSource.c_str(), "font"));

		if (m_fontData)
			m_fontGUID = m_fontData->getGUID();

		CStringProperty* fontGUID = new CStringProperty(object, "font.guid", m_fontGUID.c_str());
		fontGUID->setHidden(true);
		object->autoRelease(fontGUID);

		object->autoRelease(new CStringProperty(object, "text", m_defaultText.c_str()));
		object->autoRelease(new CStringProperty(object, "textId", m_textId.c_str()));

		return object;
	}

	void CGUIText::loadSerializable(CObjectSerializable* object)
	{
		CGUIElement::loadSerializable(object);

		m_multiLine = object->get<bool>("multiline", true);
		m_charPadding = object->get<int>("charPadding", 0);
		m_charSpacePadding = object->get<int>("charSpacePadding", 0);
		m_linePadding = object->get<int>("linePadding", 0);

		TextVertical = object->get<EGUIVerticalAlign>("textVerticle", EGUIVerticalAlign::Top);
		TextHorizontal = object->get<EGUIHorizontalAlign>("textHorizontal", EGUIHorizontalAlign::Left);

		m_strim = object->get("strim", false);

		m_fontSource = object->get<std::string>("font", std::string(""));
		m_fontGUID = object->get<std::string>("font.guid", std::string(""));

		// load path first
		m_fontData = CFontManager::getInstance()->loadFontSource(m_fontSource.c_str());
		if (!m_fontData)
			m_fontData = CFontManager::getInstance()->getFontById(m_fontGUID.c_str());

		// init font
		if (m_fontData)
		{
			m_fontData->initFont();
			m_fontGUID = m_fontData->getGUID();
		}

		std::string value = object->get<std::string>("text", std::string(""));
		m_defaultText = value;

		value = object->get<std::string>("textId", std::string(""));
		setTextId(value.c_str());

		if (m_canvas->OnLocalize != nullptr)
		{
			m_canvas->OnLocalize(this);
		}
		else
		{
			if (m_strim)
				setTextStrim(m_defaultText.c_str());
			else
				setText(m_defaultText.c_str());
		}

		m_updateTextRender = true;
	}

	void CGUIText::setFontSource(const char* fontSource)
	{
		m_fontSource = fontSource;

		m_fontData = CFontManager::getInstance()->loadFontSource(m_fontSource.c_str());
		if (m_fontData)
		{
			m_fontData->initFont();
			m_fontGUID = m_fontData->getGUID();
		}

		m_updateTextRender = true;
	}

	void CGUIText::getClosestCharacter(float posX, float posY, int& line, int& character)
	{
		line = 0;
		character = 0;

		if (m_arrayCharRender.size() == 0)
			return;

		// calc multiline height
		int textHeight = (int)m_arrayCharRender.size() * (m_textHeight + m_linePadding);
		textHeight -= m_linePadding;

		const core::rectf& rect = getRect();
		int x = (int)rect.UpperLeftCorner.X;
		int y = (int)rect.UpperLeftCorner.Y;
		int h = m_textHeight + m_linePadding;

		// calc text algin vertial
		if (TextVertical == EGUIVerticalAlign::Middle)
			y = y + ((int)m_lastHeight - textHeight - m_textOffsetY) / 2;
		else if (TextVertical == EGUIVerticalAlign::Bottom)
			y = y + (int)m_lastHeight - textHeight;

		if (m_centerRotate == true)
			y = y - textHeight / 2;

		core::vector3df linePos;

		const core::matrix4& render = getCanvas()->getRenderWorldTransform();
		core::matrix4 world = render * m_transform->World;

		for (int i = 0, n = (int)m_arrayCharRender.size(); i < n; i++)
		{
			y += h;
			linePos.set((float)x, (float)y, 0.0f);
			world.transformVect(linePos);

			line = i;

			if (posY < linePos.Y)
			{
				break;
			}
		}

		getClosestCharacter(posX, x, y - h, line, world, character);
	}

	void CGUIText::getClosestCharacter(float posX, int x, int y, int line, const core::matrix4& world, int& character)
	{
		ArrayModuleOffset& string = m_arrayCharRender[line];
		int stringWidth = 0;

		for (int i = 0, n = (int)string.size(); i < n; i++)
		{
			SModuleOffset* moduleOffset = string[i];
			if (moduleOffset->Character == ' ')
				stringWidth += ((int)moduleOffset->XAdvance + m_charSpacePadding);
			else
				stringWidth += ((int)moduleOffset->XAdvance + m_charPadding);
		}

		// text align
		if (TextHorizontal == EGUIHorizontalAlign::Center)
			x = x + ((int)getRect().getWidth() - stringWidth) / 2;
		else if (TextHorizontal == EGUIHorizontalAlign::Right)
			x = x + (int)getRect().getWidth() - stringWidth;

		if (m_centerRotate == true)
			x = x - stringWidth / 2;

		float minX = 300000.0f;
		float testX = 0.0f;
		core::vector3df p;

		int numCharacter = (int)string.size();
		for (int i = 0; i < numCharacter; i++)
		{
			p.set((float)x, (float)y, 0.0f);
			world.transformVect(p);

			SModuleOffset* moduleOffset = string[i];

			float dx = fabsf(posX - p.X);
			if (dx < minX)
			{
				minX = dx;
				testX = posX - p.X;
				character = i;
			}

			if (moduleOffset->Character == ' ')
				x += ((int)moduleOffset->XAdvance + m_charSpacePadding);
			else
				x += ((int)moduleOffset->XAdvance + m_charPadding);
		}

		if (character == numCharacter - 1 && minX >= string.back()->Module->W)
		{
			// end last char
			character++;
		}
	}
}
