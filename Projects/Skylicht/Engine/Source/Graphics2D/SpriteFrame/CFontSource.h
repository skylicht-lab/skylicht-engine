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

#pragma once

#include "Serializable/CAssetResource.h"

#include "Graphics2D/Glyph/CGlyphFreetype.h"
#include "Graphics2D/SpriteFrame/CSpriteFont.h"
#include "Graphics2D/SpriteFrame/CGlyphFont.h"

namespace Skylicht
{
	class SKYLICHT_API CFontSource : public CAssetResource
	{
	public:
		enum EFontType
		{
			GlyphFreeType = 0,
			SpriteFont
		};

	public:
		CEnumProperty<EFontType> FontType;
		CFilePathProperty Source;
		CFloatProperty FontSizePt;

	protected:
		IFont* m_font;

		std::string m_path;
		std::string m_source;
		float m_sizePt;

		int m_revision;
		
	public:
		CFontSource();

		virtual ~CFontSource();

		IFont* initFont();

		inline IFont* getFont()
		{
			return m_font;
		}

		inline const void setPath(const char* path)
		{
			m_path = path;
		}

		inline const char* getPath()
		{
			return m_path.c_str();
		}
		
		inline int getChangeRevision()
		{
			return m_revision;
		}
	};
}
