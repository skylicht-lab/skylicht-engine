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

#pragma once

#include <set>

namespace Skylicht
{
	namespace Particle
	{
		struct SInterpolatorEntry
		{
			float x;
			float y;

			SInterpolatorEntry() : x(0.0f), y(0.0f) {}

			SInterpolatorEntry(float x, float y) : x(x), y(y) {}
		};

		inline bool operator<(const SInterpolatorEntry& entry0, const SInterpolatorEntry& entry1)
		{
			return entry0.x < entry1.x;
		}

		inline bool operator==(const SInterpolatorEntry& entry0, const SInterpolatorEntry& entry1)
		{
			return entry0.x == entry1.x;
		}

		class CInterpolator
		{
		protected:
			std::set<SInterpolatorEntry> m_graph;

		public:
			CInterpolator();

			virtual ~CInterpolator();

			float interpolate(float x);

			inline std::set<SInterpolatorEntry>& getGraph()
			{
				return m_graph;
			}

			inline const std::set<SInterpolatorEntry>& getGraph() const
			{
				return m_graph;
			}

			inline bool addEntry(const SInterpolatorEntry& entry)
			{
				return m_graph.insert(entry).second;
			}

			inline bool addEntry(float x, float y)
			{
				return addEntry(SInterpolatorEntry(x, y));
			}

			inline void clearGraph()
			{
				m_graph.clear();
			}
		};
	}
}