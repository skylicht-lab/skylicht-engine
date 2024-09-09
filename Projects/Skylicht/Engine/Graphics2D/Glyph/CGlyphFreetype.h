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

#ifdef FT2_BUILD_LIBRARY
#include <ft2build.h>
#include FT_GLYPH_H
#endif

#include "Utils/CSingleton.h"
#include "Graphics2D/Atlas/CAtlas.h"
#include "Graphics2D/SpriteFrame/CSpriteAtlas.h"

namespace Skylicht
{
	struct SGlyphEntity
	{
		CAtlas* m_atlas;
		float m_advance;
		float m_uvX;
		float m_uvY;
		float m_uvW;
		float m_uvH;
		float m_offsetX;
		float m_offsetY;
	};

	struct SFaceEntity
	{
#ifdef FT2_BUILD_LIBRARY
		FT_Face m_face;
		FT_Byte* m_data;
#endif

		std::map<u32, SGlyphEntity*> m_ge;

#ifdef FT2_BUILD_LIBRARY
		SFaceEntity(FT_Face face, FT_Byte* data) :
			m_face(face),
			m_data(data)
		{
		}

		~SFaceEntity()
		{
			FT_Done_Face(m_face);

			cleanGlyphEntity();
		}
#endif

		void cleanGlyphEntity()
		{
			for (std::map<u32, SGlyphEntity*>::iterator it = m_ge.begin(), end = m_ge.end(); it != end; ++it)
				delete it->second;
			m_ge.clear();
		}
	};

	class SKYLICHT_API CGlyphFreetype
	{
	public:
		DECLARE_SINGLETON(CGlyphFreetype)

	protected:

#ifdef FT2_BUILD_LIBRARY
		FT_Library m_lib;
#endif
		std::map<std::string, SFaceEntity*> m_faceEntity;

		u32 m_width;
		u32 m_height;

		std::vector<CAtlas*> m_atlas;

	public:
		CGlyphFreetype();

		virtual ~CGlyphFreetype();

		bool initFont(const char* name, const char* path);

		void clearAtlas();

		static int sizePtToPx(float pt);

		static float sizePxToPt(int px);

		CAtlas* getCharImage(unsigned short code,
			const char* name,
			int fontSize,
			float* advance,
			float* uvX,
			float* uvY,
			float* uvW,
			float* uvH,
			float* offsetX, float* offsetY);

		CAtlas* getCharImage(
			CSpriteAtlas* external,
			unsigned short code,
			const char* name,
			int fontSize,
			float* advance,
			float* uvX,
			float* uvY,
			float* uvW,
			float* uvH,
			float* offsetX, float* offsetY);

	protected:
		CAtlas* addEmptyAtlas(ECOLOR_FORMAT color, int w, int h);

#ifdef FT2_BUILD_LIBRARY
		int putGlyphToTexture(const FT_GlyphSlot& glyph, float* uvx, float* uvy, float* uvW, float* uvH);

		CAtlas* putGlyphToTexture(CSpriteAtlas* external, const FT_GlyphSlot& glyph, float* uvx, float* uvy, float* uvW, float* uvH);
#endif
	};
}