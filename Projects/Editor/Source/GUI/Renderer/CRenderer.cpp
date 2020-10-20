/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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
#include "CRenderer.h"

namespace Skylicht
{
	namespace Editor
	{
		namespace GUI
		{
			CRenderer *g_renderer = NULL;

			CRenderer* CRenderer::getRenderer()
			{
				return g_renderer;
			}

			void CRenderer::setRenderer(CRenderer* r)
			{
				g_renderer = r;
			}

			CRenderer::CRenderer()
			{

			}

			CRenderer::~CRenderer()
			{

			}

			void CRenderer::addClipRegion(core::rectf clipRect)
			{
				clipRect.UpperLeftCorner += m_renderOffset;
				clipRect.LowerRightCorner += m_renderOffset;

				struct SSimpleRect
				{
					float x, y, w, h;
				};

				SSimpleRect out = {
					clipRect.UpperLeftCorner.X,
					clipRect.UpperLeftCorner.Y,
					clipRect.getWidth(),
					clipRect.getHeight()
				};

				SSimpleRect rect = out;

				SSimpleRect clip = {
					m_rectClipRegion.UpperLeftCorner.X,
					m_rectClipRegion.UpperLeftCorner.Y,
					m_rectClipRegion.getWidth(),
					m_rectClipRegion.getHeight()
				};

				if (rect.x < clip.x)
				{
					out.w -= clip.x - out.x;
					out.x = clip.x;
				}

				if (rect.y < clip.y)
				{
					out.h -= clip.y - out.y;
					out.y = clip.y;
				}

				if (rect.x + rect.w > clip.x + clip.w)
					out.w = (clip.x + clip.w) - out.x;

				if (rect.y + rect.h > clip.y + clip.h)
					out.h = (clip.y + clip.h) - out.y;

				m_rectClipRegion = core::rectf(out.x, out.y, out.x + out.w, out.y + out.h);
			}

			bool CRenderer::clipRegionVisible()
			{
				if (m_rectClipRegion.getWidth() <= 0.0f || m_rectClipRegion.getHeight() <= 0.0f)
					return false;

				return true;
			}
		}
	}
}