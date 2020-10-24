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

#include "IFont.h"

#if defined(USE_FREETYPE)

#include "Graphics2D/Atlas/CAtlas.h"

namespace Skylicht
{
	class CGlyphFont : public IFont
	{
	protected:
		float m_charPadding;
		float m_spacePadding;

		std::map<int, SModuleOffset*> m_moduleOffset;

		std::list<SImage> m_images;
		std::list<SModuleRect> m_moduleRect;
		std::list<SFrame> m_frames;

		std::string m_fontName;
		int m_fontSizePt;

	protected:

		SImage* getImage(CAtlas *atlas);

	public:
		CGlyphFont();

		CGlyphFont(const char *fontName, int sizePt);

		virtual ~CGlyphFont();

		inline void setFont(const char *fontName, int sizePt)
		{
			m_fontName = fontName;
			m_fontSizePt = sizePt;
		}

		inline const char *getFontName()
		{
			return m_fontName.c_str();
		}

		virtual float getCharPadding()
		{
			return m_charPadding;
		}

		virtual void setCharPadding(float padding)
		{
			m_charPadding = padding;
			m_spacePadding = padding;
		}

		virtual float getSpacePadding()
		{
			return m_spacePadding;
		}

		virtual void setSpacePadding(float padding)
		{
			m_spacePadding = padding;
		}

		virtual SModuleOffset* getCharacterModule(int character);

		virtual void getListModule(const wchar_t *string, std::vector<int>& format, std::vector<SModuleOffset*>& output, std::vector<int>& outputFormat);

		virtual void updateFontTexture();

		std::list<SImage>& getImages()
		{
			return m_images;
		}
	};
}

#endif