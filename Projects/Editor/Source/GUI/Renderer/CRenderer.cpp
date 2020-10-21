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

			void CRenderer::addClipRegion(SRect clipRect)
			{
				clipRect.X = m_renderOffset.X;
				clipRect.Y = m_renderOffset.Y;

				SRect out = clipRect;
				SRect rect = out;
				SRect clip = m_rectClipRegion;

				if (rect.X < clip.X)
				{
					out.Width -= clip.X - out.X;
					out.X = clip.X;
				}

				if (rect.Y < clip.Y)
				{
					out.Height -= clip.Y - out.Y;
					out.Y = clip.Y;
				}

				if (rect.X + rect.Width > clip.X + clip.Width)
					out.Width = (clip.X + clip.Width) - out.X;

				if (rect.Y + rect.Height > clip.Y + clip.Height)
					out.Height = (clip.Y + clip.Height) - out.Y;

				m_rectClipRegion = out;
			}

			bool CRenderer::clipRegionVisible()
			{
				if (m_rectClipRegion.Width <= 0.0f || m_rectClipRegion.Height <= 0.0f)
					return false;

				return true;
			}
		}
	}
}