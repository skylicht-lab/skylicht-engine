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
#include "CLine.h"
#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/CGroup.h"

namespace Skylicht
{
	namespace Particle
	{
		CLine::CLine(const core::vector3df& p1, const core::vector3df& p2) :
			CZone(Line)
		{
			setLine(p1, p2);
		}

		CLine::~CLine()
		{

		}

		void CLine::setLine(const core::vector3df& p1, const core::vector3df& p2)
		{
			m_p1 = p1;
			m_p2 = p2;
		}

		void CLine::generatePosition(CParticle& particle, bool full, CGroup* group)
		{
			core::vector3df pos = group->getTransformPosition(m_p1);
			core::vector3df direction = group->getTransformPosition(m_p2) - pos;

			float ratio = random(0.0f, 1.0f);
			particle.Position = pos + direction * ratio;
		}

		core::vector3df CLine::computeNormal(const core::vector3df& point, CGroup* group)
		{
			core::vector3df pos = group->getTransformPosition(m_p1);
			core::vector3df direction = group->getTransformPosition(m_p2) - pos;

			float d = -direction.dotProduct(point);
			float sqrNorm = direction.getLengthSQ();
			float t = 0.0f;
			if (sqrNorm > 0.0f)
			{
				t = -(direction.dotProduct(pos) + d) / sqrNorm;

				// t is clamped to the segment
				if (t < 0.0f)
					t = 0.0f;
				else if (t > 1.0f)
					t = 1.0f;
			}

			core::vector3df normal = point;
			normal -= pos + t * direction;
			normal.normalize();

			return normal;
		}
	}
}