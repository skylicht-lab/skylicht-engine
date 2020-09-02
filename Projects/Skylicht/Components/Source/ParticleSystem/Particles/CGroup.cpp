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

#include "CGroup.h"

#include "Systems/CParticleSystem.h"

namespace Skylicht
{
	namespace Particle
	{
		CGroup::CGroup() :
			m_zone(NULL),
			m_renderer(NULL),
			Gravity(0.0f, 0.0f, 0.0f),
			Friction(0.1f),
			LifeMin(3.0f),
			LifeMax(5.0f)
		{
			m_particleSystem = new CParticleSystem();
			m_bufferSystem = new CParticleBufferSystem();

			m_instancing = new CParticleInstancing();
		}

		CGroup::~CGroup()
		{
			delete m_particleSystem;
			delete m_bufferSystem;

			delete m_instancing;
		}

		IRenderer* CGroup::setRenderer(IRenderer *r)
		{
			m_renderer = r;
			m_renderer->getParticleBuffer(m_instancing->getMeshBuffer());
			return r;
		}

		void CGroup::update()
		{
			if (m_zone == NULL)
				return;

			float dt = getTimeStep();

			u32 autoBorn = 0;

			// update emitter
			m_launch.set_used(0);
			for (CEmitter *e : m_emitters)
			{
				u32 nb = e->updateNumber(dt);
				if (nb > 0)
				{
					SLaunchParticle data = { e, nb };
					m_launch.push_back(data);
					autoBorn += nb;
				}
			}

			CParticle *particles = m_particles.pointer();
			u32 numParticles = m_particles.size();

			// update particle system
			m_particleSystem->update(particles, numParticles, this, dt);

			for (ISystem *s : m_systems)
				s->update(particles, numParticles, this, dt);

			m_bufferSystem->update(particles, numParticles, this, dt);

			u32 emiterId = 0;
			u32 emiterLaunch = m_launch.size();

			// launch new and remove die particle
			for (u32 i = 0; i < numParticles; i++)
			{
				CParticle& p = particles[i];
				if (p.Life < 0)
				{
					if (autoBorn > 0)
					{
						// try born id dead particle						
						if (launchParticle(p, m_launch[emiterId]) == true)
							emiterId++;

						--autoBorn;
					}
					else
					{
						// remove dead particle
						remove(i);
						--i;
						--numParticles;
					}
				}
			}

			// born new particle
			if (autoBorn > 0)
			{
				for (u32 i = emiterId; i < emiterLaunch; i++)
				{
					SLaunchParticle &launch = m_launch[i];
					if (launch.Number > 0)
					{
						CParticle* newParticles = create(launch.Number);
						for (u32 j = 0, n = launch.Number; j < n; j++)
						{
							CParticle &p = newParticles[j];
							launchParticle(p, launch);
						}
					}
				}
			}

			// printf("Particle update: %d\n", m_particles.size());
		}

		bool CGroup::launchParticle(CParticle& p, SLaunchParticle& launch)
		{
			p.Age = 0.0f;
			p.Life = LifeMin + (LifeMax - LifeMin) * os::Randomizer::frand();

			launch.Emitter->emitParticle(p, m_zone);
			launch.Number--;

			return launch.Number == 0;
		}

		CParticle* CGroup::create(u32 num)
		{
			u32 total = m_particles.size();
			for (u32 i = 0; i < num; i++)
				m_particles.push_back(CParticle());
			return m_particles.pointer() + total;
		}

		void CGroup::remove(u32 index)
		{
			u32 total = m_particles.size();
			if (index >= total)
				return;

			m_particles[index].swap(m_particles.getLast());
			m_particles.set_used(total - 1);
		}
	}
}