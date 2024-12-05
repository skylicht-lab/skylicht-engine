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

#include "pch.h"
#include "CObstacleAvoidance.h"

#include "Debug/CSceneDebug.h"

namespace Skylicht
{
	namespace Graph
	{
		CObstacleAvoidance::CObstacleAvoidance()
		{

		}

		CObstacleAvoidance::~CObstacleAvoidance()
		{

		}

		void CObstacleAvoidance::addSegment(const core::vector3df& begin, const core::vector3df& end)
		{
			m_segments.push_back(core::line3df());
			core::line3df& segment = m_segments.getLast();
			segment.start = begin;
			segment.end = end;
		}

		void CObstacleAvoidance::addSegments(const core::array<core::line3df>& segments)
		{
			for (u32 i = 0, n = segments.size(); i < n; i++)
			{
				m_segments.push_back(core::line3df());
				core::line3df& segment = m_segments.getLast();
				segment.start = segments[i].start;
				segment.end = segments[i].end;
			}
		}

		void CObstacleAvoidance::clear()
		{
			m_segments.set_used(0);
		}

		// ref: https://github.com/recastnavigation/recastnavigation/blob/main/DetourCrowd/Source/DetourObstacleAvoidance.cpp
		// perp product of the two vectors
		float dtVperp2D(const core::vector3df& u, const core::vector3df& v)
		{
			return u.Z * v.X - u.X * v.Z;
		}

		static int isectRaySeg(
			const core::vector3df& position,
			const core::vector3df& velocity,
			const core::vector3df& segBegin,
			const core::vector3df& segEnd,
			float& t)
		{
			core::vector3df v = segEnd - segBegin;
			core::vector3df w = position - segBegin;

			float d = dtVperp2D(velocity, v);
			if (core::abs_(d) < 1e-6f)
				return 0;

			d = 1.0f / d;
			t = dtVperp2D(v, w) * d;

			if (t < 0 || t > 1)
				return 0;

			float s = dtVperp2D(velocity, w) * d;
			if (s < 0 || s > 1)
				return 0;

			return 1;
		}

		bool CObstacleAvoidance::isLineHit(const core::vector3df& a, const core::vector3df& b, float h, float& outT)
		{
			core::line3df* segs = m_segments.pointer();
			core::vector3df v = b - a;
			outT = 0.0f;

			for (u32 i = 0, n = m_segments.size(); i < n; i++)
			{
				core::line3df& s = segs[i];

				if (fabs(s.start.Y - a.Y) < h && fabs(s.end.Y - a.Y) < h)
				{
					int intersection = isectRaySeg(a, v, s.start, s.end, outT);
					if (intersection)
					{
						return true;
					}
				}
			}
			return false;
		}

		void CObstacleAvoidance::copySegments(CObstacleAvoidance* toTarget, const core::aabbox3df& box)
		{
			core::line3df* segs = m_segments.pointer();
			core::line3df line;

			for (u32 i = 0, n = m_segments.size(); i < n; i++)
			{
				core::line3df& s = segs[i];
				line.setLine(s.start, s.end);
				if (box.intersectsWithLine(line))
				{
					toTarget->addSegment(s.start, s.end);
				}
			}
		}

		core::vector3df CObstacleAvoidance::collide(const core::vector3df& position, const core::vector3df& vel, float stepHeight, int recursionDepth)
		{
			if (recursionDepth >= 2)
			{
				// stop if can't resolve
				return position;
			}

			core::line3df* segs = m_segments.pointer();

			core::vector3df intersectionPoint;
			core::vector3df intersectionNormal;

			core::line3df* intersectionSeg = NULL;
			float tmin = 2.0f;

			core::vector3df r = vel;
			r.normalize();

			for (u32 i = 0, n = m_segments.size(); i < n; i++)
			{
				core::line3df& s = segs[i];

				float bY = fabsf(s.start.Y - position.Y);
				float eY = fabsf(s.end.Y - position.Y);

				if (bY > stepHeight && eY > stepHeight)
					continue;

				float t = 0.0f;
				int intersection = isectRaySeg(position, vel, s.start, s.end, t);
				if (intersection)
				{
					if (t < tmin)
					{
						intersectionPoint = position + vel * t;
						intersectionSeg = &s;
						tmin = t;
					}
				}
			}

			core::vector3df newDestinationPoint;

			if (intersectionSeg)
			{
				core::vector3df v = intersectionSeg->end - intersectionSeg->start;

				intersectionNormal = v.crossProduct(core::vector3df(0.0f, 1.0f, 0.0f));
				intersectionNormal.normalize();

				float d = r.dotProduct(intersectionNormal);
				if (d < 0.0f)
					intersectionNormal *= -1.0f;

				core::vector3df destinationPoint = position + vel;
				newDestinationPoint = destinationPoint - (intersectionNormal * vel.getLength() * (1.0f - tmin));

				core::vector3df newVel = newDestinationPoint - intersectionPoint;
				newDestinationPoint = collide(intersectionPoint - r * 0.01f, newVel, stepHeight, recursionDepth + 1);
			}
			else
			{
				newDestinationPoint = position + vel;
			}

			/*
			if (intersectionSeg != NULL)
			{
				CSceneDebug* debug = CSceneDebug::getInstance()->getNoZDebug();
				debug->addLine(
					intersectionPoint,
					intersectionPoint + intersectionNormal * 0.5f,
					SColor(255, 255, 255, 0));

				debug->addLine(
					position,
					position + vel,
					SColor(255, 0, 255, 0));
			}
			*/

			return newDestinationPoint;
		}
	}
}