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
	namespace Particle
	{
		CInterpolator::CInterpolator() :
			m_lastValue(0.0f)
		{

		}

		CInterpolator::~CInterpolator()
		{

		}

		float CInterpolator::interpolate(float x)
		{
			SInterpolatorEntry currentKey(x, 0.0f);

			std::set<SInterpolatorEntry>::const_iterator nextIt = m_graph.upper_bound(currentKey);

			if (nextIt == m_graph.end())
			{
				if (m_graph.empty())
				{
					// If the graph has no entry, sets the default value
					m_lastValue = 0.0f;
					return m_lastValue;
				}
				else
				{
					// Else sets the value of the last entry
					m_lastValue = (*(--nextIt)).y;
					return m_lastValue;
				}
			}
			else if (nextIt == m_graph.begin())
			{
				// If the current X is lower than the first entry, sets the value to the first entry
				m_lastValue = (*nextIt).y;
				return m_lastValue;
			}

			const SInterpolatorEntry& nextEntry = *nextIt;
			const SInterpolatorEntry& previousEntry = *(--nextIt);

			float y0 = previousEntry.y;
			float y1 = nextEntry.y;

			float ratioX = (currentKey.x - previousEntry.x) / (nextEntry.x - previousEntry.x);
			m_lastValue = y0 + ratioX * (y1 - y0);
			return m_lastValue;
		}
	}
}