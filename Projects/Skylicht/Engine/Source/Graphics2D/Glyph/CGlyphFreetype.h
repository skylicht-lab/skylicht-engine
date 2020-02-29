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

#if defined(USE_FREETYPE)
#include <ft2build.h>
#include FT_OUTLINE_H
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "Utils/CGameSingleton.h"

namespace Skylicht
{
	struct SGlyphEntity
	{
		IImage *m_bitmap;
		float m_advance;
		core::rectf m_bounds;
		float m_uvX;
		float m_uvY;
	};

	struct SFaceEntity
	{
		FT_Face m_face;
		FT_Byte *m_data;

		std::map<int, SGlyphEntity*> m_ge;

		SFaceEntity(FT_Face face) :
			m_face(face)
		{
		}

		~SFaceEntity()
		{
			FT_Done_Face(m_face);

			for (std::map<int, SGlyphEntity*>::iterator it = m_ge.begin(), end = m_ge.end(); it != end; ++it)
			{
				delete it->second;
			}
		}
	};

	class CGlyphFreetype : public CGameSingleton<CGlyphFreetype>
	{
	protected:
		FT_Library m_lib;

	public:
		CGlyphFreetype();

		virtual ~CGlyphFreetype();

		bool initFont(const char *name, const char *path);

		IImage *getCharImage(unsigned short code,
			const char *name,
			int fontSize,
			core::rectf *bounds,
			float *advance,
			float *uvX,
			float *uvY);
	};
}

#endif