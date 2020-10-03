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
#include "CVortexSystem.h"

#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/CGroup.h"

namespace Skylicht
{
	namespace Particle
	{
		CVortexSystem::CVortexSystem(const core::vector3df& position, const core::vector3df& direction, float rotationSpeed, float attractionSpeed) :
			m_position(position),
			m_direction(direction),
			m_rotationSpeed(rotationSpeed),
			m_attractionSpeed(attractionSpeed),
			m_eyeAttractionSpeed(1.0f),
			m_eyeRadius(0.0f),
			m_killingParticleEnabled(false)
		{

		}

		CVortexSystem::~CVortexSystem()
		{

		}

		void CVortexSystem::update(CParticle *particles, int num, CGroup *group, float dt)
		{
			core::vector3df position = group->getTransformPosition(m_position);

			core::vector3df direction = group->getTransformVector(m_direction);
			direction.normalize();

			float deltaTime = dt * 0.001f;

			CParticle *p;
			float dist, angle, endRadius;
			core::vector3df rotationCenter, normal, tangent, attraction;

#pragma omp parallel for private(p, dist, angle, endRadius, rotationCenter, normal, tangent, attraction)
			for (int i = 0; i < num; i++)
			{
				p = particles + i;

				// Distance of the projection point from the position of the vortex
				dist = direction.dotProduct(p->Position - position);

				// Position of the rotation center (orthogonal projection of the particle)
				rotationCenter = direction;
				rotationCenter *= dist;

				attraction = -rotationCenter;

				rotationCenter += position;

				// Distance of the particle from the eye of the vortex
				dist = rotationCenter.getDistanceFrom(p->Position);

				if (dist <= m_eyeRadius)
				{
					if (m_killingParticleEnabled)
						p->Life = -1.0f;
					continue;
				}

				angle = m_rotationSpeed * deltaTime / dist;

				// Distance attraction
				attraction.normalize();
				attraction *= m_eyeAttractionSpeed * deltaTime / dist;

				// Computes ortho base
				normal = (p->Position - rotationCenter) / dist;
				tangent = direction.crossProduct(normal);

				endRadius = dist - m_attractionSpeed * deltaTime;
				if (endRadius <= m_eyeRadius)
				{
					endRadius = m_eyeRadius;
					if (m_killingParticleEnabled)
						p->Life = -1.0f;
				}

				p->Position = rotationCenter + normal * endRadius * cosf(angle) + tangent * endRadius * sinf(angle);

				p->Position += attraction;
			}
		}
	}
}