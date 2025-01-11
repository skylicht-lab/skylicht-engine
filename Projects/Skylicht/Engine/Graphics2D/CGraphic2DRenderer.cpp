/*
!@
MIT License

Copyright (c) 2025 Skylicht Technology CO., LTD

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
#include "CGraphic2DRenderer.h"

#include "Graphics2D/CGraphics2D.h"

namespace Skylicht
{
	CGraphic2DRenderer::CGraphic2DRenderer()
	{

	}

	CGraphic2DRenderer::~CGraphic2DRenderer()
	{

	}

	void CGraphic2DRenderer::beginRender2D(float width, float height)
	{
		IVideoDriver* driver = getVideoDriver();

		m_saveProjection = driver->getTransform(video::ETS_PROJECTION);
		m_saveView = driver->getTransform(video::ETS_VIEW);

		core::matrix4 orthoMatrix;
		orthoMatrix.buildProjectionMatrixOrthoLH(width, -height, -1.0f, 1.0f);
		orthoMatrix.setTranslation(core::vector3df(-1, 1, 0));

		driver->setTransform(video::ETS_PROJECTION, orthoMatrix);
		driver->setTransform(video::ETS_VIEW, core::IdentityMatrix);
		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

	}

	void CGraphic2DRenderer::endRender2D()
	{
		IVideoDriver* driver = getVideoDriver();

		CGraphics2D::getInstance()->flush();

		driver->setTransform(video::ETS_PROJECTION, m_saveProjection);
		driver->setTransform(video::ETS_VIEW, m_saveView);
	}
}
