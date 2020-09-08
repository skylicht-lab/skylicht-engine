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
#include "CRing.h"
#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/CGroup.h"

namespace Skylicht
{
	namespace Particle
	{
		CRing::CRing(const core::vector3df& position, const core::vector3df& normal, float minRadius, float maxRadius) :
			CZone(Ring),
			m_position(position)
		{
			setNormal(normal);
			setRadius(minRadius, maxRadius);
		}

		CRing::~CRing()
		{

		}

		void CRing::setRadius(float minRadius, float maxRadius)
		{
			if (minRadius < 0.0f) minRadius = -minRadius;
			if (maxRadius < 0.0f) maxRadius = -maxRadius;

			if (minRadius > maxRadius)
				std::swap(minRadius, maxRadius);

			m_minRadius = minRadius;
			m_maxRadius = maxRadius;
		}

		void CRing::generatePosition(CParticle& particle, bool full, CGroup* group)
		{
			float sqrMinRadius = m_minRadius * m_minRadius;
			float sqrMaxRadius = m_maxRadius * m_maxRadius;

			core::vector3df pos = group->getTransformPosition(m_position);
			core::vector3df normal = group->getTransformVector(m_normal);

			core::vector3df tmp;
			do
			{
				tmp.set(random(-1.0f, 1.0f), random(-1.0f, 1.0f), random(-1.0f, 1.0f));
			} while (tmp.getLengthSQ() > 1.0f);

			core::vector3df v = normal.crossProduct(tmp);
			v.normalize();
			v *= sqrtf(random(sqrMinRadius, sqrMaxRadius)); // to have a uniform distribution

			particle.Position = pos + v;
		}

		core::vector3df CRing::computeNormal(const core::vector3df& point, CGroup* group)
		{
			return group->getTransformVector(m_normal);
		}
	}
}