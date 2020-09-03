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
#include "CEmitter.h"

#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/Zones/CZone.h"

namespace Skylicht
{
	namespace Particle
	{
		CEmitter::CEmitter(EEmitter type) :
			m_tank(0),
			m_flow(0.0f),
			m_forceMin(8.0f),
			m_forceMax(10.0f),
			m_active(true),
			m_emitFullZone(true),
			m_type(type)
		{
			m_fraction = os::Randomizer::frand();
		}

		CEmitter::~CEmitter()
		{

		}

		u32 CEmitter::updateNumber(float deltaTime)
		{
			int nbBorn = 0;

			if (m_flow <= 0.0f)
			{
				nbBorn = core::max_(0, m_tank);
				m_tank = 0;
			}
			else if (m_tank >= 0)
			{
				m_fraction += m_flow * deltaTime * 0.001f;
				nbBorn = static_cast<int>(m_fraction);
				if (m_tank > 0)
				{
					nbBorn = core::min_(m_tank, nbBorn);
					m_tank -= nbBorn;
				}
				m_fraction -= nbBorn;
			}
			else
			{
				nbBorn = 0;
			}

			return (u32)nbBorn;
		}

		void CEmitter::generateVelocity(CParticle& particle, CZone* zone)
		{
			float force = m_forceMin + (m_forceMax - m_forceMin) * os::Randomizer::frand();
			generateVelocity(particle, force / particle.Params[Mass], zone);
		}

		void CEmitter::emitParticle(CParticle &particle, CZone* zone)
		{
			zone->generatePosition(particle, m_emitFullZone);
			generateVelocity(particle, zone);
		}
	}
}