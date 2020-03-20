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
#include "CAtlas.h"

namespace Skylicht
{
	CAtlas::CAtlas(ECOLOR_FORMAT format, int width, int height) :
		m_width(width),
		m_height(height),
		m_needUpdateTexture(true),
		m_texture(NULL)
	{
		m_image = getVideoDriver()->createImage(format, core::dimension2du(width, height));

		u32 bpp = m_image->getBitsPerPixel();
		u32 size = m_width * m_height * bpp / 8;
		u8 *data = (u8*)m_image->lock();
		memset(data, 0, size);
		m_image->unlock();

		core::recti r(0, 0, width, height);
		m_rects.push_back(r);
	}

	CAtlas::~CAtlas()
	{
		m_image->drop();

		if (m_texture != NULL)
			getVideoDriver()->removeTexture(m_texture);
	}

	void CAtlas::calcCellSize(int *w, int *h)
	{
		const int cellSize = 8;

		int cellW = cellSize;
		while (cellW < *w)
		{
			cellW += cellSize;
		}
		*w = cellW + 2;

		int cellH = cellSize;
		while (cellH < *h)
		{
			cellH += cellSize;
		}
		*h = cellH + 2;
	}

	core::recti CAtlas::createRect(int w, int h)
	{
		core::recti ret(0, 0, 0, 0);

		for (int i = 0, n = m_rects.size(); i < n; i++)
		{
			if (m_rects[i].getWidth() >= w &&
				m_rects[i].getHeight() >= h)
			{
				// found region & devide this region
				ret = devideRect(i, w, h);
				return ret;
			}
		}

		return ret;
	}

	core::recti CAtlas::devideRect(int rectID, int w, int h)
	{
		core::recti r = m_rects[rectID];

		// remove this region
		m_rects.erase(m_rects.begin() + rectID);

		// result
		core::recti ret = r;
		ret.LowerRightCorner.X = r.UpperLeftCorner.X + w;
		ret.LowerRightCorner.Y = r.UpperLeftCorner.Y + h;

		// duc.phamhong note
		// divide region r
		//
		// *---+-----------*
		// | r |           |
		// | e |   right   |
		// | t |           |
		// +---+-----------*
		// |               |
		// |     bottom    |
		// |               |
		// *---------------*
		//
		core::recti right, bottom;
		right.UpperLeftCorner.X = ret.LowerRightCorner.X;
		right.LowerRightCorner.X = right.UpperLeftCorner.X + r.getWidth() - w;
		right.UpperLeftCorner.Y = ret.UpperLeftCorner.Y;
		right.LowerRightCorner.Y = ret.LowerRightCorner.Y;

		bottom.UpperLeftCorner.X = r.UpperLeftCorner.X;
		bottom.LowerRightCorner.X = r.LowerRightCorner.X;
		bottom.UpperLeftCorner.Y = ret.LowerRightCorner.Y;
		bottom.LowerRightCorner.Y = bottom.UpperLeftCorner.Y + r.getHeight() - h;

		// add new region (sort by height)
		if (right.getWidth() > 0 && right.getHeight() > 0)
			insertSortRect(right);

		if (bottom.getWidth() > 0 && bottom.getHeight() > 0)
			insertSortRect(bottom);

		return ret;
	}

	void CAtlas::insertSortRect(core::recti r)
	{
		bool add = false;
		int h = r.getHeight();

		for (int i = m_rects.size() - 1; i >= 0; i--)
		{
			if (h > m_rects[i].getHeight())
			{
				add = true;
				m_rects.insert(m_rects.begin() + (i + 1), r);
				break;
			}
		}

		if (add == false)
			m_rects.insert(m_rects.begin(), r);
	}

	ITexture* CAtlas::getTexture()
	{
		if (m_texture == NULL)
			m_texture = getVideoDriver()->addTexture("atlas", m_image);

		return m_texture;
	}

	void CAtlas::updateTexture()
	{
		if (m_texture == NULL)
		{
			getTexture();
			return;
		}

		if (m_needUpdateTexture)
		{
			void *data = m_texture->lock();
			memcpy(data, m_image->lock(), m_width * m_height * 4);
			m_texture->unlock();
			m_texture->regenerateMipMapLevels();
			m_needUpdateTexture = false;

			/*
			static int test = 0;
			char name[64];
			sprintf(name, "C:\\SVN\\test_%d.png", test++);
			getVideoDriver()->writeImageToFile(m_image, name);
			*/
		}
	}

	void CAtlas::bitBltImage(IImage *img, int x, int y)
	{
		img->copyTo(m_image, core::vector2di(x, y));
		m_needUpdateTexture = true;
	}
}
