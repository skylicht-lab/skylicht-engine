/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CTriangleSelector.h"

namespace Skylicht
{
	CTriangleSelector::CTriangleSelector(CEntity* entity) :
		m_entity(entity)
	{

	}

	CTriangleSelector::~CTriangleSelector()
	{

	}

	void CTriangleSelector::getTriangles(core::triangle3df* triangles, const core::matrix4* transform)
	{
		u32 cnt = m_triangles.size();

		core::matrix4 mat;
		if (transform)
			mat = *transform;

		for (u32 i = 0; i < cnt; ++i)
		{
			mat.transformVect(triangles[i].pointA, m_triangles[i].pointA);
			mat.transformVect(triangles[i].pointB, m_triangles[i].pointB);
			mat.transformVect(triangles[i].pointC, m_triangles[i].pointC);
		}
	}

	const core::aabbox3df& CTriangleSelector::getBBox()
	{
		m_bbox.reset(m_triangles[0].pointA);

		u32 cnt = m_triangles.size();
		for (u32 i = 0; i < cnt; ++i)
		{
			m_bbox.addInternalPoint(m_triangles[i].pointA);
			m_bbox.addInternalPoint(m_triangles[i].pointB);
			m_bbox.addInternalPoint(m_triangles[i].pointC);
		}

		return m_bbox;
	}
}