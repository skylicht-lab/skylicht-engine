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
#include "CCylinder.h"
#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/CGroup.h"

namespace Skylicht
{
	namespace Particle
	{
		CCylinder::CCylinder(const core::vector3df& position,
			const core::vector3df& direction,
			float radius,
			float length) :
			CZone(Cylinder),
			m_position(position),
			m_direction(direction),
			m_radius(radius),
			m_length(length)
		{
			m_direction.normalize();
		}

		CCylinder::~CCylinder()
		{

		}

		void CCylinder::generatePosition(CParticle& particle, bool full, CGroup* group)
		{
			float cLength = random(0.0f, m_length) - m_length * 0.5f;
			float cAngle = random(0.0f, 2 * core::PI);
			float cRadius = full ? random(0.0f, m_radius) : m_radius;

			core::vector3df pos = group->getTransformPosition(m_position);
			core::vector3df dir = group->getTransformVector(m_direction);

			// We need at least two points to compute a base
			core::vector3df rPoint = pos + core::vector3df(10.0f, 10.0f, 10.0f);
			float dist = dir.dotProduct(rPoint);

			while (dist == 0.0f || dir * dist + pos == rPoint)
			{
				// avoid dist == 0, which leads to a div by zero.
				rPoint = pos + core::vector3df(random(-10.0f, 10.0f), 10.0f, random(-10.0f, 10.0f));
				dist = dir.dotProduct(rPoint);
			}

			core::vector3df p1 = dir * dist + pos;
			core::vector3df a = (rPoint - p1);
			a.Y = 0.0f;
			a.normalize();

			core::vector3df b = a.crossProduct(-dir);

			particle.Position = pos + cLength * dir + a * cRadius * cosf(cAngle) + b * cRadius * sinf(cAngle);
		}

		core::vector3df CCylinder::computeNormal(const core::vector3df& point, CGroup* group)
		{
			core::vector3df pos = group->getTransformPosition(m_position);
			core::vector3df dir = group->getTransformVector(m_direction);

			float dist = dir.dotProduct(point - pos);

			if (dist >= m_length * 0.5f)
				return dir;

			if (dist <= -m_length * 0.5f)
				return -dir;

			core::vector3df ext = point - (dir*dist + pos);
			ext.normalize();

			return ext;
		}
	}
}