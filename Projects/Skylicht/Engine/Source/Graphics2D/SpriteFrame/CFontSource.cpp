/*
!@
MIT License

Copyright (c) 2023 Skylicht Technology CO., LTD

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
#include "CFontSource.h"
#include "Utils/CPath.h"
#include "Utils/CStringImp.h"

namespace Skylicht
{
	CFontSource::CFontSource() :
		CAssetResource("CFontSource"),
		FontType(this, "fontType", CFontSource::GlyphFreeType),
		Source(this, "source"),
		FontSizePt(this, "fontSizePt", 20.0f),
		m_font(NULL),
		m_sizePt(0.0f)
	{
		// font source
		Source.Exts.push_back("ttf");
		Source.Exts.push_back("otf");
		Source.Exts.push_back("xml");

		FontSizePt.ClampMin = true;
		FontSizePt.Min = 1.0f;
		FontSizePt.ClampMax = true;
		FontSizePt.Max = 200.0f;

		FontType.addEnumString("GlyphFreeType", CFontSource::GlyphFreeType);
		FontType.addEnumString("SpriteFont", CFontSource::SpriteFont);
	}

	CFontSource::~CFontSource()
	{
		if (m_font)
			delete m_font;
	}

	IFont* CFontSource::initFont()
	{
		const std::string& fontPath = Source.get();
		if (fontPath.empty())
		{
			if (m_font)
			{
				delete m_font;
				m_font = NULL;
			}

			m_source.clear();
			m_sizePt = 0.0f;

			return m_font;
		}

		std::string fontName = CPath::getFileName(fontPath);
		std::string fontExt = CPath::getFileNameExt(fontPath);
		fontExt = CStringImp::toLower(fontExt);

		// re-init font
		if (FontType.get() == CFontSource::GlyphFreeType)
		{
			if (m_source == fontPath && m_sizePt == FontSizePt.get())
			{
				// no changed
				return m_font;
			}

			if (fontExt == "ttf" || fontExt == "otf")
			{
				if (CGlyphFreetype::getInstance()->initFont(fontName.c_str(), fontPath.c_str()))
				{
					if (m_font)
						delete m_font;

					m_font = new CGlyphFont(fontName.c_str(), FontSizePt.get());

					m_source = fontPath;
					m_sizePt = FontSizePt.get();
				}
			}
		}
		else
		{
			if (m_source == fontPath)
			{
				// no changed
				return m_font;
			}

			if (m_font)
				delete m_font;

			CSpriteFont* spriteFont = new CSpriteFont();
			if (spriteFont->loadFont(fontPath.c_str()))
				m_font = spriteFont;
			else
			{
				delete spriteFont;
				m_font = NULL;
			}

			m_source = fontPath;
			m_sizePt = 0.0f;
		}

		return m_font;
	}
}