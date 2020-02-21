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

#include "CCanvas.h"

#include "CGraphics2D.h"
#include "Material/Shader/CShaderManager.h"

#define MAX_VERTICES (128*4)
#define MAX_INDICES	(128*6)

namespace Skylicht
{

	CGraphics2D::CGraphics2D() :
		m_currentW(-1),
		m_currentH(-1),
		m_scaleRatio(1.0f)
	{
		m_driver = getVideoDriver();
	}

	void CGraphics2D::init()
	{
	}

	CGraphics2D::~CGraphics2D()
	{
	}

	core::dimension2du CGraphics2D::getScreenSize()
	{
		core::dimension2du screenSize = getVideoDriver()->getScreenSize();

		float w = (float)screenSize.Width * m_scaleRatio;
		float h = (float)screenSize.Height * m_scaleRatio;

		screenSize.set((int)w, (int)h);

		return screenSize;
	}

	bool CGraphics2D::isHD()
	{
		core::dimension2du size = getVideoDriver()->getScreenSize();
		if (size.Width > 1400 || size.Height > 1400)
			return true;
		return false;
	}

	bool CGraphics2D::isWideScreen()
	{
		core::dimension2du size = getVideoDriver()->getScreenSize();
		float r = size.Width / (float)size.Height;
		if (r >= 1.5f)
			return true;
		else
			return false;
	}

	void CGraphics2D::addCanvas(CCanvas *canvas)
	{
		if (std::find(m_canvas.begin(), m_canvas.end(), canvas) == m_canvas.end())
			m_canvas.push_back(canvas);
	}

	void CGraphics2D::removeCanvas(CCanvas *canvas)
	{
		std::vector<CCanvas*>::iterator i = std::find(m_canvas.begin(), m_canvas.end(), canvas);
		if (i != m_canvas.end())
			m_canvas.erase(i);
	}

	void CGraphics2D::render(CCamera *camera)
	{
		// sort canvas by depth
		struct {
			bool operator()(CCanvas* a, CCanvas* b) const
			{
				return a->getSortDepth() < b->getSortDepth();
			}
		} customLess;
		std::sort(m_canvas.begin(), m_canvas.end(), customLess);

		// render graphics
		for (CCanvas *canvas : m_canvas)
		{
			canvas->render(camera);
		}
	}
}