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
#include "CGlyphFont.h"
#include "Graphics2D/Glyph/CGlyphFreetype.h"

namespace Skylicht
{
	CGlyphFont::CGlyphFont() :
		m_charPadding(0.0f),
		m_spacePadding(0.0f),
		m_fontName("Segoe UI Light"), // default font
		m_fontSizePt(24)
	{

	}

	CGlyphFont::~CGlyphFont()
	{

	}

	SImage* CGlyphFont::getImage(CAtlas *atlas)
	{
		// find atlas image
		for (SImage& img : m_images)
		{
			if (img.Texture == atlas->getTexture())
			{
				return &img;
			}
		}

		// add new image
		m_images.push_back(SImage());

		SImage* img = &m_images.back();
		img->Texture = atlas->getTexture();
		img->Atlas = atlas;

		return img;
	}

	SModuleOffset* CGlyphFont::getCharacterModule(int character)
	{
		int fontSize = CGlyphFreetype::getFontPtToPx(m_fontSizePt);
		u32 key = (fontSize << 16) | (u16)character;

		SModuleOffset* c = m_moduleOffset[key];
		if (c != NULL)
			return c;

		float advance = 0.0f, x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;

		CGlyphFreetype *glyphFreetype = CGlyphFreetype::getInstance();
		CAtlas *atlas = glyphFreetype->getCharImage(
			(u16)character,
			m_fontName.c_str(),
			fontSize,
			&advance,
			&x, &y, &w, &h);

		if (atlas != NULL)
		{
			SImage* img = getImage(atlas);

			m_frames.push_back(SFrame());
			SFrame &frame = m_frames.back();

			frame.Image = img;
			frame.ID = character;
			frame.ModuleOffset.push_back(SModuleOffset());

			c = &frame.ModuleOffset.back();
			c->Character = character;
			c->XAdvance = advance;
			c->OffsetX = 0;
			c->OffsetY = 0;

			core::dimension2du size = atlas->getImage()->getDimension();

			m_moduleRect.push_back(SModuleRect());
			SModuleRect &module = m_moduleRect.back();
			module.X = x * size.Width;
			module.Y = y * size.Height;
			module.W = w * size.Width;
			module.H = h * size.Height;

			c->Module = &module;
			c->Frame = &frame;

			// bounding rect
			frame.BoudingRect.UpperLeftCorner.set(module.X, module.Y);
			frame.BoudingRect.LowerRightCorner.set(module.X + module.W, module.Y + module.H);

			m_moduleOffset[key] = c;
			m_frames.push_back(frame);
		}

		return c;
	}

	void CGlyphFont::getListModule(const wchar_t *string, std::vector<int>& format, std::vector<SModuleOffset*>& output, std::vector<int>& outputFormat)
	{
		IFont::getListModule(string, format, output, outputFormat);

		for (SImage& img : m_images)
		{
			if (img.Atlas != NULL)
				img.Atlas->updateTexture();
		}
	}
}