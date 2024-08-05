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

#pragma once

#include "CGUIElement.h"
#include "Graphics2D/SpriteFrame/IFont.h"

#define MAX_FORMATCOLOR 32

namespace Skylicht
{
	class CFontSource;

	class SKYLICHT_API CGUIText : public CGUIElement
	{
		friend class CCanvas;
	public:
		typedef std::vector<int> ArrayInt;
		typedef std::vector<SModuleOffset*> ArrayModuleOffset;

	protected:
		IFont* m_font;
		IFont* m_customfont;

		int m_charPadding;
		int m_charSpacePadding;
		int m_linePadding;

		std::string m_text;
		std::wstring m_textw;
		ArrayInt m_textFormat;

		std::string m_textId;
		std::wstring m_textwId;

		EGUIVerticalAlign TextVertical;
		EGUIHorizontalAlign TextHorizontal;

		int m_textHeight;
		int m_textOffsetY;
		int m_textSpaceWidth;

		bool m_multiLine;
		bool m_centerRotate;

		SColor m_colorFormat[MAX_FORMATCOLOR];

		bool m_updateTextRender;

		std::vector<ArrayModuleOffset> m_arrayCharRender;
		std::vector<ArrayInt> m_arrayCharFormat;

		std::string m_fontSource;
		std::string m_fontGUID;

		CFontSource* m_fontData;
		int m_fontChanged;

		float m_lastWidth;
		float m_lastHeight;
	protected:
		CGUIText(CCanvas* canvas, CGUIElement* parent, IFont* font);
		CGUIText(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect, IFont* font);

		virtual void renderText(ArrayModuleOffset& string, ArrayInt& format, int posX, int posY);

		void init();

	public:
		virtual ~CGUIText();

		virtual void render(CCamera* camera);

		void splitText(std::vector<ArrayModuleOffset>& split, std::vector<ArrayInt>& format, int width);

		void setTextAlign(EGUIHorizontalAlign h, EGUIVerticalAlign v)
		{
			TextVertical = v;
			TextHorizontal = h;
		}

		/*
		* Example:
		* <1>PLAYERA <0>kill <1>PLAYERB <0>with <2>AK47
		* With format:
		* <1> -> RED
		* <0> -> WHITE
		* <2> -> YELLOW
		*/
		void setFormatText(const char* formatText);

		// setColorFormat
		// set color for function setFormatText
		void setColorFormat(int id, const SColor& c)
		{
			if (id < MAX_FORMATCOLOR)
				m_colorFormat[id] = c;
		}

		void setText(const char* text);
		void setTextStrim(const char* text);
		void setText(const wchar_t* text);
		void setTextStrim(const wchar_t* text);

		void setTextId(const char* textId);
		void setTextId(const wchar_t* textId);

		int getStringWidth(const char* text);
		int getCharWidth(wchar_t c);

		const char* getText()
		{
			return m_text.c_str();
		}

		const wchar_t* getTextW()
		{
			return m_textw.c_str();
		}

		const char* getTextId()
		{
			return m_textId.c_str();
		}

		const wchar_t* getTextwId()
		{
			return m_textwId.c_str();
		}

		inline void setCharPadding(int charPadding)
		{
			m_charPadding = charPadding;
			m_charSpacePadding = charPadding;
		}

		inline void setLinePadding(int linePadding)
		{
			m_linePadding = linePadding;
		}

		inline void setMultiLine(bool b)
		{
			m_multiLine = b;
		}

		inline void setCenterRotate(bool b)
		{
			m_centerRotate = b;
		}

		void setFontSource(const char* fontSource);

		DECLARE_GETTYPENAME(CGUIText)

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

	protected:

		void initFont(IFont* font);

		IFont* getCurrentFont();
	};
}
