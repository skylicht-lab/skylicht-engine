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
#include "CGlyphFreetype.h"

// From SDL_ttf: Handy routines for converting from fixed point
#define FT_CEIL(X)  (((X + 63) & -64) / 64)

// Glyph metrics:
// --------------
//
//                       xmin                     xmax
//                        |                         |
//                        |<-------- width -------->|
//                        |                         |
//              |         +-------------------------+----------------- ymax
//              |         |    ggggggggg   ggggg    |     ^        ^
//              |         |   g:::::::::ggg::::g    |     |        |
//              |         |  g:::::::::::::::::g    |     |        |
//              |         | g::::::ggggg::::::gg    |     |        |
//              |         | g:::::g     g:::::g     |     |        |
//    offsetX  -|-------->| g:::::g     g:::::g     |  offsetY     |
//              |         | g:::::g     g:::::g     |     |        |
//              |         | g::::::g    g:::::g     |     |        |
//              |         | g:::::::ggggg:::::g     |     |        |
//              |         |  g::::::::::::::::g     |     |      height
//              |         |   gg::::::::::::::g     |     |        |
//  baseline ---*---------|---- gggggggg::::::g-----*--------      |
//            / |         |             g:::::g     |              |
//     origin   |         | gggggg      g:::::g     |              |
//              |         | g:::::gg   gg:::::g     |              |
//              |         |  g::::::ggg:::::::g     |              |
//              |         |   gg:::::::::::::g      |              |
//              |         |     ggg::::::ggg        |              |
//              |         |         gggggg          |              v
//              |         +-------------------------+----------------- ymin
//              |                                   |
//              |------------- advanceX ----------->|

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CGlyphFreetype);

	CGlyphFreetype::CGlyphFreetype() :
		m_width(1024),
		m_height(1024)
	{
#ifdef FT2_BUILD_LIBRARY
		int error = FT_Init_FreeType(&m_lib);
		if (error)
			os::Printer::log("FreeType provider: can't init FreeType!  error = %d\n", ELL_ERROR);

		addEmptyAtlas(ECF_A8R8G8B8, m_width, m_height);
#endif
	}

	CGlyphFreetype::~CGlyphFreetype()
	{
		for (CAtlas* a : m_atlas)
			delete a;
		m_atlas.clear();

		for (std::map<std::string, SFaceEntity*>::iterator i = m_faceEntity.begin(), end = m_faceEntity.end(); i != end; i++)
		{
			if (i->second != NULL)
			{
#ifdef FT2_BUILD_LIBRARY
				delete[]i->second->m_data;
#endif
				delete i->second;
			}
		}
		m_faceEntity.clear();

#ifdef FT2_BUILD_LIBRARY
		int error = FT_Done_FreeType(m_lib);
		if (error)
			os::Printer::log("FreeType provider: can't close FreeType!  error = %d\n", ELL_ERROR);
		m_lib = NULL;
#endif
	}

	bool CGlyphFreetype::initFont(const char* name, const char* path)
	{
		if (m_faceEntity.find(name) != m_faceEntity.end())
			return true;

#ifdef FT2_BUILD_LIBRARY
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();
		io::IReadFile* readFile = fs->createAndOpenFile(path);
		if (readFile != NULL)
		{
			long dataSize = readFile->getSize();
			FT_Byte* data = new FT_Byte[dataSize];
			readFile->read(data, dataSize);

			FT_Face face = NULL;
			FT_Error error = FT_New_Memory_Face(m_lib, data, dataSize, 0, &face);
			if (error != 0)
			{
				char log[512];
				sprintf(log, "[CGlyphFreetype] initFont '%s' failed: no support encode!", name);
				os::Printer::log(log);

				delete[]data;
				return false;
			}

			m_faceEntity[name] = new SFaceEntity(face, data);

			readFile->drop();
			return true;
		}
		else
		{
			char log[512];
			sprintf(log, "[CGlyphFreetype] initFont '%s' failed: file not found!", name);
			os::Printer::log(log);
		}
#endif
		return false;
	}

	void CGlyphFreetype::clearAtlas()
	{
		for (CAtlas* a : m_atlas)
			delete a;
		m_atlas.clear();

		for (std::map<std::string, SFaceEntity*>::iterator i = m_faceEntity.begin(), end = m_faceEntity.end(); i != end; i++)
			i->second->cleanGlyphEntity();

		addEmptyAtlas(ECF_A8R8G8B8, m_width, m_height);
	}

	int CGlyphFreetype::sizePtToPx(float pt)
	{
		return (int)(pt * (8.0f / 6.0f));
	}

	float CGlyphFreetype::sizePxToPt(int px)
	{
		return ((float)px * (6.0f / 8.0f));
	}

	CAtlas* CGlyphFreetype::getCharImage(unsigned short code,
		const char* name,
		int fontSize,
		float* advance,
		float* uvX, float* uvY, float* uvW, float* uvH, float* offsetX, float* offsetY)
	{
#ifdef FT2_BUILD_LIBRARY
		SFaceEntity* fe = m_faceEntity[name];
		if (fe == NULL)
			return NULL;

		u32 key = (fontSize << 16) | code;
		SGlyphEntity* ge = fe->m_ge[key];

		if (ge == NULL)
		{
			FT_Size_RequestRec req;
			req.type = FT_SIZE_REQUEST_TYPE_REAL_DIM;
			req.width = 0;
			req.height = (uint32_t)fontSize * 64;
			req.horiResolution = 0;
			req.vertResolution = 0;
			FT_Request_Size(fe->m_face, &req);

			if (FT_Load_Char(fe->m_face, code, FT_LOAD_RENDER))
				return NULL;

			const FT_GlyphSlot& g = fe->m_face->glyph;
			int atlasID = putGlyphToTexture(g, uvX, uvY, uvW, uvH);

			ge = new SGlyphEntity();
			ge->m_atlas = m_atlas[atlasID];
			ge->m_advance = (float)FT_CEIL(g->advance.x);
			ge->m_uvX = *uvX;
			ge->m_uvY = *uvY;
			ge->m_uvW = *uvW;
			ge->m_uvH = *uvH;

			// Glyph metrics
			// https://docs.microsoft.com/en-us/typography/opentype/spec/gpos
			float height = (float)FT_CEIL(g->metrics.vertAdvance);
			ge->m_offsetX = (float)FT_CEIL(g->metrics.horiBearingX);
			ge->m_offsetY = -(float)FT_CEIL(g->metrics.horiBearingY) + height;

			fe->m_ge[key] = ge;
		}

		if (ge != NULL)
		{
			*uvX = ge->m_uvX;
			*uvY = ge->m_uvY;
			*uvW = ge->m_uvW;
			*uvH = ge->m_uvH;
			*advance = ge->m_advance;
			*offsetX = ge->m_offsetX;
			*offsetY = ge->m_offsetY;
			return ge->m_atlas;
		}
		else
		{
			*uvX = 0;
			*uvY = 0;
			*uvW = 0;
			*uvH = 0;
			*advance = 0;
			*offsetX = 0;
			*offsetY = 0;
			return NULL;
		}
#else
		* uvX = 0;
		*uvY = 0;
		*uvW = 0;
		*uvH = 0;
		*advance = 0;
		*offsetX = 0;
		*offsetY = 0;
		return NULL;
#endif
	}

	CAtlas* CGlyphFreetype::getCharImage(
		CSpriteAtlas* external,
		unsigned short code,
		const char* name,
		int fontSize,
		float* advance,
		float* uvX,
		float* uvY,
		float* uvW,
		float* uvH,
		float* offsetX, float* offsetY)
	{
#ifdef FT2_BUILD_LIBRARY
		SFaceEntity* fe = m_faceEntity[name];
		if (fe == NULL)
			return NULL;

		u32 key = (fontSize << 16) | code;
		SGlyphEntity* ge = fe->m_ge[key];

		if (ge == NULL)
		{
			FT_Size_RequestRec req;
			req.type = FT_SIZE_REQUEST_TYPE_REAL_DIM;
			req.width = 0;
			req.height = (uint32_t)fontSize * 64;
			req.horiResolution = 0;
			req.vertResolution = 0;
			FT_Request_Size(fe->m_face, &req);

			if (FT_Load_Char(fe->m_face, code, FT_LOAD_RENDER))
				return NULL;

			const FT_GlyphSlot& g = fe->m_face->glyph;
			CAtlas* atlas = putGlyphToTexture(external, g, uvX, uvY, uvW, uvH);

			ge = new SGlyphEntity();
			ge->m_atlas = atlas;
			ge->m_advance = (float)FT_CEIL(g->advance.x);
			ge->m_uvX = *uvX;
			ge->m_uvY = *uvY;
			ge->m_uvW = *uvW;
			ge->m_uvH = *uvH;

			// Glyph metrics
			// https://docs.microsoft.com/en-us/typography/opentype/spec/gpos
			float height = (float)FT_CEIL(g->metrics.vertAdvance);
			ge->m_offsetX = (float)FT_CEIL(g->metrics.horiBearingX);
			ge->m_offsetY = -(float)FT_CEIL(g->metrics.horiBearingY) + height;

			fe->m_ge[key] = ge;
		}

		if (ge != NULL)
		{
			*uvX = ge->m_uvX;
			*uvY = ge->m_uvY;
			*uvW = ge->m_uvW;
			*uvH = ge->m_uvH;
			*advance = ge->m_advance;
			*offsetX = ge->m_offsetX;
			*offsetY = ge->m_offsetY;
			return ge->m_atlas;
		}
		else
		{
			*uvX = 0;
			*uvY = 0;
			*uvW = 0;
			*uvH = 0;
			*advance = 0;
			*offsetX = 0;
			*offsetY = 0;
			return NULL;
		}
#else
		* uvX = 0;
		*uvY = 0;
		*uvW = 0;
		*uvH = 0;
		*advance = 0;
		*offsetX = 0;
		*offsetY = 0;
		return NULL;
#endif
	}

#ifdef FT2_BUILD_LIBRARY
	int CGlyphFreetype::putGlyphToTexture(const FT_GlyphSlot& glyph, float* uvX, float* uvY, float* uvW, float* uvH)
	{
		int glyphW = glyph->bitmap.width;
		int glyphH = glyph->bitmap.rows;

		int cellW = glyphW;
		int cellH = glyphH;

		CAtlas::calcCellSize(&cellW, &cellH);

		int atlasID = -1;
		core::recti region;

		for (u32 i = 0, n = (u32)m_atlas.size(); i < n; i++)
		{
			region = m_atlas[i]->createRect(cellW, cellH);

			if (region.getWidth() != 0 && region.getHeight() != 0)
			{
				atlasID = i;
				break;
			}
		}

		if (atlasID == -1)
		{
			addEmptyAtlas(ECF_A8R8G8B8, m_width, m_height);

			atlasID = (int)(m_atlas.size() - 1);
			region = m_atlas[atlasID]->createRect(cellW, cellH);
		}

		// draw character at region
		int x = region.UpperLeftCorner.X;
		int y = region.UpperLeftCorner.Y;

		x += (cellW - glyphW) / 2;
		y += (cellH - glyphH) / 2;

		*uvX = x / (float)m_width;
		*uvY = y / (float)m_height;
		*uvW = glyphW / (float)m_width;
		*uvH = glyphH / (float)m_height;

		unsigned char* alpha = glyph->bitmap.buffer;

		// need convert to A8R8G8B8
		IImage* img = getVideoDriver()->createImage(ECF_A8R8G8B8, core::dimension2du(glyphW, glyphH));
		unsigned char* src = (unsigned char*)img->lock();
		unsigned char* p = src;

		for (int i = 0, size = glyphH * glyphW; i < size; i++)
		{
			*p++ = 255;
			*p++ = 255;
			*p++ = 255;
			*p++ = *alpha++;
		}

		img->unlock();

		m_atlas[atlasID]->bitBltImage(img, x, y);

		img->drop();

		return atlasID;
	}

	CAtlas* CGlyphFreetype::putGlyphToTexture(CSpriteAtlas* external, const FT_GlyphSlot& glyph, float* uvX, float* uvY, float* uvW, float* uvH)
	{
		int glyphW = glyph->bitmap.width;
		int glyphH = glyph->bitmap.rows;

		int cellW = glyphW;
		int cellH = glyphH;

		CAtlas::calcCellSize(&cellW, &cellH);

		core::recti region;

		SImage* imageAtlas = external->createAtlasRect(cellW, cellH, region);
		if (imageAtlas == NULL)
			return NULL;

		CAtlas* atlas = imageAtlas->Atlas;

		// draw character at region
		int x = region.UpperLeftCorner.X;
		int y = region.UpperLeftCorner.Y;

		x += (cellW - glyphW) / 2;
		y += (cellH - glyphH) / 2;

		*uvX = x / (float)m_width;
		*uvY = y / (float)m_height;
		*uvW = glyphW / (float)m_width;
		*uvH = glyphH / (float)m_height;

		unsigned char* alpha = glyph->bitmap.buffer;

		// need convert to A8R8G8B8
		IImage* img = getVideoDriver()->createImage(ECF_A8R8G8B8, core::dimension2du(glyphW, glyphH));
		unsigned char* src = (unsigned char*)img->lock();
		unsigned char* p = src;

		for (int i = 0, size = glyphH * glyphW; i < size; i++)
		{
			*p++ = 255;
			*p++ = 255;
			*p++ = 255;
			*p++ = *alpha++;
		}

		img->unlock();

		atlas->bitBltImage(img, x, y);

		img->drop();

		return atlas;
	}
#endif

	CAtlas* CGlyphFreetype::addEmptyAtlas(ECOLOR_FORMAT color, int w, int h)
	{
		CAtlas* newAtlas = new CAtlas(color, w, h);
		m_atlas.push_back(newAtlas);
		return newAtlas;
	}
}