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
#include "CPolyLine.h"
#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/CGroup.h"

namespace Skylicht
{
	namespace Particle
	{
		CPolyLine::CPolyLine(const core::array<core::vector3df>& point) :
			CZone(PolyLine),
			m_length(0.0f),
			m_normal(0.0f, 1.0f, 0.0f)
		{
			setLine(point);
		}

		CPolyLine::~CPolyLine()
		{

		}

		void CPolyLine::setLine(const core::array<core::vector3df>& point)
		{
			if (point.size() < 2)
				return;

			m_point = point;
			m_segments.set_used(0);
			m_ratios.set_used(0);
			m_length = 0.0f;

			for (u32 i = 0, n = (u32)point.size(); i < n - 1; i++)
			{
				float d = point[i].getDistanceFrom(point[i + 1]);
				m_length = m_length + d;
				m_segments.push_back(d);
			}

			float d = 0.0f;

			for (u32 i = 0, n = (u32)m_segments.size(); i < n; i++)
			{
				d = d + m_segments[i];
				m_ratios.push_back(d / m_length);
			}
		}

		void CPolyLine::generatePosition(CParticle& particle, bool full, CGroup* group)
		{
			float ratio = random(0.0f, 1.0f);

			int segmentId = 0;
			float length = 0.0f;
			float l = 0.0f;

			for (u32 i = 0, n = (u32)m_segments.size(); i < n; i++)
			{
				segmentId = i;

				if (ratio < m_ratios[i])
				{
					l = (m_ratios[i] - ratio) * m_length;
					break;
				}
			}

			const core::vector3df& p1 = m_point[segmentId];
			const core::vector3df& p2 = m_point[segmentId + 1];

			core::vector3df pos = group->getTransformPosition(p1);

			core::vector3df direction = group->getTransformPosition(p2) - pos;
			direction.normalize();

			particle.Position = pos + direction * l;
		}

		core::vector3df CPolyLine::computeNormal(const core::vector3df& point, CGroup* group)
		{
			return m_normal;
		}
	}
}