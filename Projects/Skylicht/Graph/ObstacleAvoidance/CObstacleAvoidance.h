/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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

namespace Skylicht
{
	namespace Graph
	{
		struct SObstacleCircle
		{
			core::vector3df Center;
			float Radius;
		};

		class CObstacleAvoidance
		{
		protected:
			core::array<core::line3df> m_segments;

		public:
			CObstacleAvoidance();

			virtual ~CObstacleAvoidance();

			void addSegment(const core::vector3df& begin, const core::vector3df& end);

			void addSegments(const core::array<core::line3df>& segments);

			void clear();

			void copySegments(CObstacleAvoidance* toTarget, const core::aabbox3df& box);

			inline core::array<core::line3df>& getSegments()
			{
				return m_segments;
			}

			inline bool empty()
			{
				return m_segments.empty();
			}

			inline bool isLineHit(const core::line3df& line, float h = 1.0f)
			{
				return isLineHit(line.start, line.end, h);
			}

			bool isLineHit(const core::vector3df& a, const core::vector3df& b, float h = 1.0f);

			core::vector3df collide(const core::vector3df& position, const core::vector3df& vel, float stepHeight = 0.3f, int recursionDepth = 0);
		};
	}
}