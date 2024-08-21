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
#include "CGUIMask.h"
#include "Graphics2D/CGraphics2D.h"

namespace Skylicht
{
	CGUIMask::CGUIMask(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect) :
		CGUIElement(canvas, parent, rect),
		m_drawMask(false)
	{

	}

	CGUIMask::~CGUIMask()
	{

	}

	void CGUIMask::update(CCamera* camera)
	{
		updateClipRect(camera);
	}

	void CGUIMask::updateClipRect(CCamera* camera)
	{
		const core::rectf& r = getRect();
		m_drawMask = false;

		float z = 0.0f;
		if (camera->getProjectionType() != CCamera::OrthoUI)
			z = 0.1f;

		m_topLeft.set(r.UpperLeftCorner.X, r.UpperLeftCorner.Y, z);
		m_bottomRight.set(r.LowerRightCorner.X, r.LowerRightCorner.Y, z);

		m_transform->World.transformVect(m_topLeft);
		m_transform->World.transformVect(m_bottomRight);
	}

	void CGUIMask::applyParentClip(CGUIMask* parent)
	{
		if (parent == NULL || parent == this)
			return;

		if (m_topLeft.X < parent->m_topLeft.X)
			m_topLeft.X = parent->m_topLeft.X;
		if (m_topLeft.Y < parent->m_topLeft.Y)
			m_topLeft.Y = parent->m_topLeft.Y;

		if (m_bottomRight.X > parent->m_bottomRight.X)
			m_bottomRight.X = parent->m_bottomRight.X;
		if (m_bottomRight.Y > parent->m_bottomRight.Y)
			m_bottomRight.Y = parent->m_bottomRight.Y;
	}

	void CGUIMask::render(CCamera* camera)
	{
		CGUIElement::render(camera);
	}

	void CGUIMask::beginMaskTest(CCamera* camera)
	{
		if (m_drawMask == false)
			drawMask(camera);

		// depth test for clip mask
		CGraphics2D::getInstance()->beginDepthTest();
	}

	void CGUIMask::drawMask(CCamera* camera)
	{
		if (m_drawMask == false)
		{
			CGraphics2D* g = CGraphics2D::getInstance();

			// draw depth for mask
			g->beginDrawDepth();
			g->draw2DRectangle(m_topLeft, m_bottomRight, SColor(255, 0, 0, 0));
			g->endDrawDepth();
		}

		m_drawMask = true;
	}

	void CGUIMask::endMaskTest()
	{
		// end depth test
		CGraphics2D::getInstance()->endDepthTest();
	}
}