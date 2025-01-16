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
		m_noZDebug(NULL)
	{
	}

	CSceneDebug::~CSceneDebug()
	{
		if (m_noZDebug)
			delete m_noZDebug;
		clear();
	}

	CSceneDebug* CSceneDebug::getNoZDebug()
	{
		if (m_noZDebug == NULL)
			m_noZDebug = new CSceneDebug();
		return m_noZDebug;
	}

	void CSceneDebug::addCircle(const core::vector3df& pos, float radius, const core::vector3df& normal, const SColor& color)
	{
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
		m_linestrip.push_back(new SLineStripDebug());

		SLineStripDebug* line = m_linestrip.getLast();
		line->color = color;

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
			line->point.push_back(point);

			// inc rad
			rad = rad + radInc;
		}
	}

	void CSceneDebug::addEclipse(const core::vector3df& pos, float radiusZ, float radiusX, const core::vector3df& normal, const SColor& color)
	{
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
		m_linestrip.push_back(new SLineStripDebug());

		SLineStripDebug* line = m_linestrip.getLast();
		line->color = color;

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
			line->point.push_back(point);

			// inc rad
			rad = rad + radInc;
		}
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
		m_tri.push_back(new STriDebug());
		STriDebug* t = m_tri.getLast();
		t->tri = tri;
		t->color = color;
	}

	void CSceneDebug::addLine(const core::vector3df& v1, const core::vector3df& v2, const SColor& color)
	{
		m_lines.push_back(new SLineDebug());
		SLineDebug* line = m_lines.getLast();
		line->line.setLine(v1, v2);
		line->color = color;
	}

	void CSceneDebug::addLine(const core::line3df& line, const SColor& color)
	{
		m_lines.push_back(new SLineDebug());
		SLineDebug* l = m_lines.getLast();
		l->line = line;
		l->color = color;
	}

	void CSceneDebug::addLinestrip(const std::vector<core::vector3df>& point, const SColor& color)
	{
		m_linestrip.push_back(new SLineStripDebug());
		SLineStripDebug* l = m_linestrip.getLast();
		for (int i = 0, n = (int)point.size(); i < n; i++)
			l->point.push_back(point[i]);
		l->color = color;
	}

	void CSceneDebug::addLinestrip(const core::array<core::vector3df>& point, const SColor& color)
	{
		m_linestrip.push_back(new SLineStripDebug());
		SLineStripDebug* l = m_linestrip.getLast();
		l->point = point;
		l->color = color;
	}

	void CSceneDebug::addBoudingBox(const core::aabbox3df& box, const SColor& color)
	{
		m_boxs.push_back(new SBoxDebug());
		SBoxDebug* b = m_boxs.getLast();
		b->box = box;
		b->color = color;
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

	void CSceneDebug::addText(const core::vector3df& pos, const char* string, const SColor& color)
	{
		CTextBillboardManager* textMgr = CTextBillboardManager::getInstance();

		CRenderTextData* textData = textMgr->addText(pos, string);
		if (textData)
		{
			textData->Color = color;
			textData->VAlign = CRenderTextData::Middle;
			textData->HAlign = CRenderTextData::Center;
			m_texts.push_back(textData);
		}
	}

	void CSceneDebug::clearText()
	{
		CTextBillboardManager* textMgr = CTextBillboardManager::getInstance();

		for (u32 i = 0, n = m_texts.size(); i < n; i++)
			textMgr->remove(m_texts[i]);

		m_texts.set_used(0);
	}

	void CSceneDebug::clearLines()
	{
		for (u32 i = 0, n = m_lines.size(); i < n; i++)
			delete m_lines[i];
		m_lines.set_used(0);
	}

	void CSceneDebug::clearLineStrip()
	{
		for (u32 i = 0, n = m_linestrip.size(); i < n; i++)
			delete m_linestrip[i];
		m_linestrip.set_used(0);
	}

	void CSceneDebug::clearBoxs()
	{
		for (u32 i = 0, n = m_boxs.size(); i < n; i++)
			delete m_boxs[i];
		m_boxs.set_used(0);
	}

	void CSceneDebug::clearTri()
	{
		for (u32 i = 0, n = m_tri.size(); i < n; i++)
			delete m_tri[i];
		m_tri.set_used(0);
	}
}