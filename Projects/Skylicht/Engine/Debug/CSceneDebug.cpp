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
#include "CSceneDebug.h"

namespace Skylicht
{
	IMPLEMENT_SINGLETON(CSceneDebug);

	CSceneDebug::CSceneDebug() :
		m_nLine(0),
		m_nLineStrip(0),
		m_nBox(0),
		m_nTri(0),
		m_noZDebug(NULL)
	{
		m_lines = new SLineDebug[MAX_DEBUGGEOMETRY];
		m_linestrip = new SLineStripDebug[MAX_DEBUGGEOMETRY];
		m_boxs = new SBoxDebug[MAX_DEBUGGEOMETRY];
		m_tri = new STriDebug[MAX_DEBUGGEOMETRY];
	}

	CSceneDebug::~CSceneDebug()
	{
		delete[] m_lines;
		delete[] m_linestrip;
		delete[] m_boxs;
		delete[] m_tri;

		if (m_noZDebug)
			delete m_noZDebug;
	}

	CSceneDebug* CSceneDebug::getNoZDebug()
	{
		if (m_noZDebug == NULL)
			m_noZDebug = new CSceneDebug();
		return m_noZDebug;
	}

	void CSceneDebug::addCircle(const core::vector3df& pos, float radius, const core::vector3df& normal, const SColor& color)
	{
		if (m_nLineStrip >= MAX_DEBUGGEOMETRY)
			return;

		core::quaternion q;
		core::vector3df up(0.0f, 1.0f, 0.0f);
		core::vector3df n = normal;
		n.normalize();
		q.rotationFromTo(up, n);

		int step = 30;
		float rad = 0;
		float radInc = core::PI * 2 / step;

		core::vector3df	point;

		// clear point list
		m_linestrip[m_nLineStrip].point.set_used(0);
		m_linestrip[m_nLineStrip].color = color;

		for (int i = 0; i <= step; i++)
		{
			point.Y = 0.0;
			point.X = radius * sinf(rad);
			point.Z = radius * cosf(rad);

			// rotate
			point = q * point;

			// translate
			point += pos;

			// add point
			m_linestrip[m_nLineStrip].point.push_back(point);

			// inc rad
			rad = rad + radInc;
		}

		m_nLineStrip++;
	}

	void CSceneDebug::addEclipse(const core::vector3df& pos, float radiusZ, float radiusX, const core::vector3df& normal, const SColor& color)
	{
		if (m_nLineStrip >= MAX_DEBUGGEOMETRY)
			return;

		core::quaternion q;
		core::vector3df up(0.0f, 1.0f, 0.0f);
		core::vector3df n = normal;
		n.normalize();
		q.rotationFromTo(up, n);

		int		step = 30;
		float	rad = 0;
		float	radInc = core::PI * 2 / step;

		core::vector3df	point;

		// clear point list
		m_linestrip[m_nLineStrip].point.set_used(0);
		m_linestrip[m_nLineStrip].color = color;

		for (int i = 0; i <= step; i++)
		{
			point.Y = 0.0;
			point.X = radiusX * sinf(rad);
			point.Z = radiusZ * cosf(rad);

			// rotate
			point = q * point;

			// translate
			point += pos;

			// add point
			m_linestrip[m_nLineStrip].point.push_back(point);

			// inc rad
			rad = rad + radInc;
		}

		m_nLineStrip++;
	}

	void CSceneDebug::addSphere(const core::vector3df& pos, float radius, const SColor& color)
	{
		core::vector3df normal;

		normal = core::vector3df(0.0f, 1.0f, 0.0f);
		addCircle(pos, radius, normal, color);


		normal = core::vector3df(0.0f, 0.0f, 1.0f);
		addCircle(pos, radius, normal, color);
	}

	void CSceneDebug::addPosition(const core::vector3df& pos, float length, const SColor& color)
	{
		core::vector3df u(0.0f, 1.0f, 0.0f);
		core::vector3df f(0.0f, 0.0f, 1.0f);
		core::vector3df r(1.0f, 0.0f, 0.0f);
		addLine(pos - u * length, pos + u * length, color);
		addLine(pos - f * length, pos + f * length, color);
		addLine(pos - r * length, pos + r * length, color);
	}

	void CSceneDebug::addTri(const core::triangle3df& tri, const SColor& color)
	{
		if (m_nTri >= MAX_DEBUGGEOMETRY)
			return;

		m_tri[m_nTri].tri = tri;
		m_tri[m_nTri].color = color;
		m_nTri++;
	}

	void CSceneDebug::addLine(const core::vector3df& v1, const core::vector3df& v2, const SColor& color)
	{
		if (m_nLine >= MAX_DEBUGGEOMETRY)
			return;

		core::line3df line(v1, v2);
		m_lines[m_nLine].line = line;
		m_lines[m_nLine].color = color;
		m_nLine++;
	}

	void CSceneDebug::addLine(const core::line3df& line, const SColor& color)
	{
		if (m_nLine >= MAX_DEBUGGEOMETRY)
			return;

		m_lines[m_nLine].line = line;
		m_lines[m_nLine].color = color;
		m_nLine++;
	}

	void CSceneDebug::addLinestrip(const std::vector<core::vector3df>& point, const SColor& color)
	{
		if (m_nLineStrip >= MAX_DEBUGGEOMETRY)
			return;

		m_linestrip[m_nLineStrip].point.set_used(0);
		for (int i = 0, n = (int)point.size(); i < n; i++)
			m_linestrip[m_nLineStrip].point.push_back(point[i]);

		m_linestrip[m_nLineStrip].color = color;
		m_nLineStrip++;
	}

	void CSceneDebug::addLinestrip(const core::array<core::vector3df>& point, const SColor& color)
	{
		if (m_nLineStrip >= MAX_DEBUGGEOMETRY)
			return;

		m_linestrip[m_nLineStrip].point = point;
		m_linestrip[m_nLineStrip].color = color;
		m_nLineStrip++;
	}

	void CSceneDebug::addBoudingBox(const core::aabbox3df& box, const SColor& color)
	{
		if (m_nBox >= MAX_DEBUGGEOMETRY)
			return;

		m_boxs[m_nBox].box = box;
		m_boxs[m_nBox].color = color;
		m_nBox++;
	}

	void CSceneDebug::addTransformBBox(const core::aabbox3df& box, const SColor& color, const core::matrix4& mat)
	{
		core::vector3df edges[8];
		box.getEdges(edges);

		for (int i = 0; i < 8; i++)
			mat.transformVect(edges[i]);

		addLine(edges[5], edges[1], color);
		addLine(edges[1], edges[3], color);
		addLine(edges[3], edges[7], color);
		addLine(edges[7], edges[5], color);
		addLine(edges[0], edges[2], color);
		addLine(edges[2], edges[6], color);
		addLine(edges[6], edges[4], color);
		addLine(edges[4], edges[0], color);
		addLine(edges[1], edges[0], color);
		addLine(edges[3], edges[2], color);
		addLine(edges[7], edges[6], color);
		addLine(edges[5], edges[4], color);
	}

	void CSceneDebug::addTransform(const core::matrix4& mat, float vectorLength)
	{
		core::vector3df pos = mat.getTranslation();

		core::vector3df y(0.0f, 1.0f, 0.0f);
		core::vector3df x(1.0f, 0.0f, 0.0f);
		core::vector3df z(0.0f, 0.0f, 1.0f);

		mat.rotateVect(x);
		mat.rotateVect(y);
		mat.rotateVect(z);

		addLine(pos, pos + x * vectorLength, SColor(255, 255, 0, 0));
		addLine(pos, pos + y * vectorLength, SColor(255, 0, 255, 0));
		addLine(pos, pos + z * vectorLength, SColor(255, 0, 0, 255));
	}
}