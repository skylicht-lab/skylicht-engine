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

	SControlPoint& CInterpolator::addControlPoint()
	{
		m_controls.push_back(SControlPoint());
		return m_controls.back();
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
}