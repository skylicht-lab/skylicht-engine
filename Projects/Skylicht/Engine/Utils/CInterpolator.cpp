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
#include "CInterpolator.h"

namespace Skylicht
{
	CInterpolator::CInterpolator() :
		m_type(CInterpolator::Float)
	{

	}

	CInterpolator::~CInterpolator()
	{

	}

	SControlPoint& CInterpolator::addControlPoint(int layer)
	{
		m_controls[layer].push_back(SControlPoint());
		return m_controls[layer].back();
	}

	void CInterpolator::getMinMaxXY(core::vector2df& min, core::vector2df& max)
	{
		min.set(0.0f, 0.0f);
		max.set(0.0f, 0.0f);

		int id = 0;
		for (const auto& it : m_graph)
		{
			float X = it.X;
			float maxY = it.Value[0];
			float minY = it.Value[0];

			for (int i = 0; i < 4; i++)
			{
				maxY = core::max_(maxY, it.Value[i]);
				minY = core::min_(minY, it.Value[i]);
			}

			if (id++ == 0)
			{
				min.X = X;
				min.Y = minY;

				max.X = X;
				max.Y = maxY;
			}

			min.X = core::min_(min.X, X);
			max.X = core::max_(max.X, X);

			min.Y = core::min_(min.Y, minY);
			max.Y = core::max_(max.Y, maxY);
		}
	}

	float CInterpolator::interpolate(float x)
	{
		SInterpolatorEntry currentKey(x);
		std::set<SInterpolatorEntry>::const_iterator nextIt = m_graph.upper_bound(currentKey);

		if (nextIt == m_graph.end())
		{
			if (m_graph.empty())
			{
				// If the graph has no entry, sets the default value
				return 0.0f;
			}
			else
			{
				// Else sets the value of the last entry
				return (*(--nextIt)).Value[0];
			}
		}
		else if (nextIt == m_graph.begin())
		{
			// If the current X is lower than the first entry, sets the value to the first entry
			return (*nextIt).Value[0];
		}

		const SInterpolatorEntry& nextEntry = *nextIt;
		const SInterpolatorEntry& previousEntry = *(--nextIt);

		float y0 = previousEntry.Value[0];
		float y1 = nextEntry.Value[0];

		float ratioX = (x - previousEntry.X) / (nextEntry.X - previousEntry.X);
		return y0 + ratioX * (y1 - y0);
	}

	core::vector2df CInterpolator::interpolateVec2(float x)
	{
		SInterpolatorEntry currentKey(x);
		std::set<SInterpolatorEntry>::const_iterator nextIt = m_graph.upper_bound(currentKey);

		if (nextIt == m_graph.end())
		{
			if (m_graph.empty())
			{
				// If the graph has no entry, sets the default value
				return core::vector2df();
			}
			else
			{
				// Else sets the value of the last entry
				auto it = (*(--nextIt));
				return core::vector2df(it.Value[0], it.Value[1]);
			}
		}
		else if (nextIt == m_graph.begin())
		{
			// If the current X is lower than the first entry, sets the value to the first entry
			auto it = (*nextIt);
			return core::vector2df(it.Value[0], it.Value[1]);
		}

		const SInterpolatorEntry& nextEntry = *nextIt;
		const SInterpolatorEntry& previousEntry = *(--nextIt);

		float ratioX = (x - previousEntry.X) / (nextEntry.X - previousEntry.X);

		core::vector2df result;
		float* v = &result.X;

		for (int i = 0; i < 2; i++)
		{
			float y0 = previousEntry.Value[i];
			float y1 = nextEntry.Value[i];

			*v = y0 + ratioX * (y1 - y0);
			v++;
		}

		return result;
	}

	core::vector3df CInterpolator::interpolateVec3(float x)
	{
		SInterpolatorEntry currentKey(x);
		std::set<SInterpolatorEntry>::const_iterator nextIt = m_graph.upper_bound(currentKey);

		if (nextIt == m_graph.end())
		{
			if (m_graph.empty())
			{
				// If the graph has no entry, sets the default value
				return core::vector3df();
			}
			else
			{
				// Else sets the value of the last entry
				auto it = (*(--nextIt));
				return core::vector3df(it.Value[0], it.Value[1], it.Value[2]);
			}
		}
		else if (nextIt == m_graph.begin())
		{
			// If the current X is lower than the first entry, sets the value to the first entry
			auto it = (*nextIt);
			return core::vector3df(it.Value[0], it.Value[1], it.Value[2]);
		}

		const SInterpolatorEntry& nextEntry = *nextIt;
		const SInterpolatorEntry& previousEntry = *(--nextIt);

		float ratioX = (x - previousEntry.X) / (nextEntry.X - previousEntry.X);

		core::vector3df result;
		float* v = &result.X;

		for (int i = 0; i < 3; i++)
		{
			float y0 = previousEntry.Value[i];
			float y1 = nextEntry.Value[i];

			*v = y0 + ratioX * (y1 - y0);
			v++;
		}

		return result;
	}

	SColorf CInterpolator::interpolateColorf(float x)
	{
		SInterpolatorEntry currentKey(x);
		std::set<SInterpolatorEntry>::const_iterator nextIt = m_graph.upper_bound(currentKey);

		if (nextIt == m_graph.end())
		{
			if (m_graph.empty())
			{
				// If the graph has no entry, sets the default value
				return SColorf(0.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				// Else sets the value of the last entry
				auto it = (*(--nextIt));
				return SColorf(it.Value[0], it.Value[1], it.Value[2], it.Value[3]);
			}
		}
		else if (nextIt == m_graph.begin())
		{
			// If the current X is lower than the first entry, sets the value to the first entry
			auto it = (*nextIt);
			return SColorf(it.Value[0], it.Value[1], it.Value[2], it.Value[3]);
		}

		const SInterpolatorEntry& nextEntry = *nextIt;
		const SInterpolatorEntry& previousEntry = *(--nextIt);

		float ratioX = (x - previousEntry.X) / (nextEntry.X - previousEntry.X);

		SColorf result;
		float* v = &result.r;

		for (int i = 0; i < 4; i++)
		{
			float y0 = previousEntry.Value[i];
			float y1 = nextEntry.Value[i];

			*v = y0 + ratioX * (y1 - y0);
			v++;
		}

		return result;
	}

	core::vector2df bezier(
		const core::vector2df& p0,
		const core::vector2df& p1,
		const core::vector2df& p2,
		const core::vector2df& p3,
		float t)
	{
		// Copilot: Hi colilot, can you help me write the function draw Benzier (by 4 points) in C
		core::vector2df result;
		// Calculate the blending functions for each control point
		float u = 1 - t;
		result.X = u * u * u * p0.X + 3 * u * u * t * p1.X + 3 * u * t * t * p2.X + t * t * t * p3.X;
		result.Y = u * u * u * p0.Y + 3 * u * u * t * p1.Y + 3 * u * t * t * p2.Y + t * t * t * p3.Y;
		return result;
	}

	void CInterpolator::computeBezier(const SControlPoint& p1, const SControlPoint& p2, ArrayPoint2df& points, int bezierStep)
	{
		core::vector2df p1Right = p1.Right;
		core::vector2df p2Left = p2.Left;

		points.set_used(0);

		if (p1.Type == SControlPoint::Linear && p2.Type == SControlPoint::Linear)
		{
			core::vector2df pt = p1.Position;
			points.push_back(pt);

			pt = p2.Position;
			points.push_back(pt);
			return;
		}

		if (p1.Type == SControlPoint::Linear)
			p1Right.set(0.0f, 0.0f);

		if (p2.Type == SControlPoint::Linear)
			p2Left.set(0.0f, 0.0f);

		core::vector2df p[4];
		p[0] = p1.Position;
		p[1] = p1.Position + p1Right;
		p[2] = p2.Position + p2Left;
		p[3] = p2.Position;

		for (int i = 0; i <= bezierStep; i++)
		{
			float t = i / (float)bezierStep;
			core::vector2df pt = bezier(p[0], p[1], p[2], p[3], t);
			points.push_back(pt);
		}
	}

	void CInterpolator::computeLine(int layer, std::vector<ArrayPoint2df>& lines, int bezierStep)
	{
		std::vector<SControlPoint>& controls = m_controls[layer];
		int n = (int)controls.size();
		if (n < 2)
			return;

		lines.clear();

		for (int i = 0; i < n - 1; i++)
		{
			SControlPoint& p1 = controls[i];
			SControlPoint& p2 = controls[i + 1];

			lines.push_back(ArrayPoint2df());
			ArrayPoint2df& line = lines.back();

			computeBezier(p1, p2, line, bezierStep);
		}
	}

	void CInterpolator::generateGraph(int layer, int bezierStep)
	{
		std::vector<ArrayPoint2df> lines;
		computeLine(layer, lines, bezierStep);

		m_graph.clear();

		bool first = true;
		core::vector2df last;

		for (auto& it : lines)
		{
			for (u32 i = 0, n = it.size(); i < n; i++)
			{
				const core::vector2df& p = it[i];
				if (first || p != last)
				{
					addEntry(layer, p.X, p.Y);
					last = p;
					first = false;
				}
			}
		}
	}
}