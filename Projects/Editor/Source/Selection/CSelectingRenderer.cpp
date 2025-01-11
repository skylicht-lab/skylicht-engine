/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CSelectingRenderer.h"
#include "CSelecting.h"

#include "Graphics2D/CGraphics2D.h"

namespace Skylicht
{
	namespace Editor
	{

		CSelectingRenderer::CSelectingRenderer() :
			m_enable(true)
		{

		}

		CSelectingRenderer::~CSelectingRenderer()
		{

		}

		void CSelectingRenderer::beginQuery(CEntityManager* entityManager)
		{

		}

		void CSelectingRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
		{

		}

		void CSelectingRenderer::init(CEntityManager* entityManager)
		{

		}

		void CSelectingRenderer::update(CEntityManager* entityManager)
		{

		}

		void CSelectingRenderer::render(CEntityManager* entityManager)
		{

		}

		void CSelectingRenderer::postRender(CEntityManager* entityManager)
		{
			if (!m_enable)
				return;

			core::vector2di from, to;

			if (CSelecting::getInstance()->isDragSelect(from, to))
			{
				IVideoDriver* driver = getVideoDriver();

				CGraphics2D* g = CGraphics2D::getInstance();

				core::recti vp = driver->getViewPort();
				beginRender2D((float)vp.getWidth(), (float)vp.getHeight());

				core::rectf r((float)from.X, (float)from.Y, (float)to.X, (float)to.Y);

				g->draw2DRectangle(r, SColor(10, 255, 255, 255));
				g->draw2DRectangleOutline(r, SColor(150, 255, 255, 255));

				endRender2D();
			}
		}

	}
}