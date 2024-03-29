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

#include "Graphics2D/Atlas/CAtlas.h"
#include "CSpriteFrame.h"

namespace Skylicht
{
	class SKYLICHT_API CGlyphFont :
		public CSpriteFrame,
		public IFont
	{
	protected:
		float m_fontSizePt;

		std::map<int, SModuleOffset*> m_moduleOffset;

		std::string m_fontName;

	protected:

		SImage* getImage(CAtlas* atlas);

	public:
		CGlyphFont();

		CGlyphFont(const char* fontName, float sizePt);

		virtual ~CGlyphFont();

		inline void setFont(const char* fontName, float sizePt)
		{
			m_fontName = fontName;
			m_fontSizePt = sizePt;
		}

		inline const char* getFontName()
		{
			return m_fontName.c_str();
		}

		virtual SModuleOffset* getCharacterModule(int character);

		virtual void getListModule(const wchar_t* string, std::vector<int>& format, std::vector<SModuleOffset*>& output, std::vector<int>& outputFormat);

		virtual void updateFontTexture();

		std::vector<SImage*>& getImages()
		{
			return m_images;
		}
	};
}