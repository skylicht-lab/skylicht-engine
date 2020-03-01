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

namespace Skylicht
{
	class CAtlas
	{
	protected:
		ITexture *m_texture;
		IImage *m_image;
		bool m_needUpdateTexture;

		int m_width;
		int m_height;

		std::vector<core::recti> m_rects;

	public:
		CAtlas(ECOLOR_FORMAT format, int width, int height);

		virtual ~CAtlas();

		core::recti createRect(int w, int h);

		core::recti devideRect(int rectID, int w, int h);

		void insertSortRect(core::recti r);

		static void calcCellSize(int *w, int *h);

		inline IImage* getImage()
		{
			return m_image;
		}

		ITexture* getTexture();

		inline bool needUpdateTexture()
		{
			return m_needUpdateTexture;
		}

		void updateTexture();

		void bitBltImage(IImage *img, int x, int y);
	};
}