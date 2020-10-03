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
#include "CSphericEmitter.h"

#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/Zones/CZone.h"
#include "ParticleSystem/Particles/CGroup.h"

namespace Skylicht
{
	namespace Particle
	{
		CSphericEmitter::CSphericEmitter() :
			CEmitter(Spheric),
			m_angleA(0.0f),
			m_angleB(0.0f),
			m_cosAngleMin(0.0f),
			m_cosAngleMax(0.0f)
		{
			memset(m_matrix, 0, sizeof(float) * 9);
		}

		CSphericEmitter::~CSphericEmitter()
		{

		}

		void CSphericEmitter::setAngles(float angleA, float angleB)
		{
			if (angleB < angleA)
				std::swap(angleA, angleB);

			angleA = core::min_(2.0f * core::PI, core::max_(0.0f, angleA));
			angleB = core::min_(2.0f * core::PI, core::max_(0.0f, angleB));

			m_angleA = angleA;
			m_angleB = angleB;

			m_cosAngleMin = cosf(m_angleA * 0.5f);
			m_cosAngleMax = cosf(m_angleB * 0.5f);
		}

		void CSphericEmitter::setDirection(const core::vector3df& d)
		{
			m_direction = d;
			m_direction.normalize();

			if ((m_direction.X == 0.0f) && (m_direction.Y == 0.0f))
			{

				m_matrix[0] = m_direction.Z;
				m_matrix[1] = 0.0f;
				m_matrix[2] = 0.0f;
				m_matrix[3] = 0.0f;
				m_matrix[4] = m_direction.Z;
				m_matrix[5] = 0.0f;
				m_matrix[6] = 0.0f;
				m_matrix[7] = 0.0f;
				m_matrix[8] = m_direction.Z;
			}
			else
			{
				core::vector3df axis;
				axis = m_direction.crossProduct(core::vector3df(0.0f, 0.0f, 1.0f));

				float cosA = m_direction.Z;
				float sinA = -axis.getLength();
				axis /= -sinA;

				float x = axis.X;
				float y = axis.Y;
				float z = axis.Z;

				m_matrix[0] = x * x + cosA * (1.0f - x * x);
				m_matrix[1] = x * y * (1.0f - cosA) - z * sinA;
				m_matrix[2] = m_direction.X;
				m_matrix[3] = x * y * (1.0f - cosA) + z * sinA;
				m_matrix[4] = y * y + cosA * (1.0f - y * y);
				m_matrix[5] = m_direction.Y;
				m_matrix[6] = x * z * (1.0f - cosA) - y * sinA;
				m_matrix[7] = y * z * (1.0f - cosA) + x * sinA;
				m_matrix[8] = m_direction.Z;
			}
		}

		void CSphericEmitter::generateVelocity(CParticle& particle, float speed, CZone* zone, CGroup *group)
		{
			float a = random(m_cosAngleMax, m_cosAngleMin);
			float theta = acosf(a);
			float phi = random(0.0f, 2.0f * core::PI);

			float sinTheta = sinf(theta);
			float x = sinTheta * cosf(phi);
			float y = sinTheta * sinf(phi);
			float z = cosf(theta);

			particle.Velocity.X = (m_matrix[0] * x + m_matrix[1] * y + m_matrix[2] * z);
			particle.Velocity.Y = (m_matrix[3] * x + m_matrix[4] * y + m_matrix[5] * z);
			particle.Velocity.Z = (m_matrix[6] * x + m_matrix[7] * y + m_matrix[8] * z);

			particle.Velocity = group->getTransformVector(particle.Velocity);
			particle.Velocity.normalize();
			particle.Velocity *= speed;
		}
	}
}
