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
#include "CParticleTrail.h"

namespace Skylicht
{
	namespace Particle
	{
		CParticleTrail::CParticleTrail(CGroup *group) :
			m_group(group),
			m_segmentLength(0.3f),
			m_segmentCount(150),
			m_width(0.3f),
			m_alpha(1.0f),
			m_trailCount(0)
		{
			group->setCallback(this);
		}

		CParticleTrail::~CParticleTrail()
		{
			if (m_group != NULL)
				m_group->setCallback(NULL);

			for (u32 i = 0, n = m_trails.size(); i < n; i++)
			{
				m_trails[i].DeleteData();
				// printf("Delete %d\n", i);
			}

			m_trails.clear();
		}

		void CParticleTrail::update()
		{
			if (m_group == NULL)
				return;
		}

		void CParticleTrail::OnParticleUpdate(CParticle *particles, int num, CGroup *group, float dt)
		{
			float seg2 = m_segmentLength * m_segmentLength;

			// delete dead particle data
			if (m_trails.size() >= (u32)num)
			{
				for (u32 i = (u32)num, n = m_trails.size(); i < n; i++)
				{
					// printf("Delete %d\n", i);
					m_trails[i].DeleteData();
				}

				m_trailCount = num;
			}

			m_trails.set_used(num);

			for (int i = 0; i < num; i++)
			{
				CParticle& p = particles[i];

				STrailInfo &particlePos = m_trails[p.Index];

				if (m_trailCount <= i)
				{
					// init new particle data
					particlePos.InitData();
					m_trailCount = i + 1;

					// printf("New %d\n", i);
				}

				particlePos.CurrentPosition = p.Position;

				if (particlePos.Position->size() == 0)
				{
					particlePos.Position->push_back(SParticlePosition());

					SParticlePosition& pos = particlePos.Position->getLast();
					pos.Alpha = m_alpha;
					pos.Width = m_width;
					pos.Position = p.Position;

					particlePos.LastPosition = p.Position;
				}
				else
				{
					if (p.Position.getDistanceFromSQ(particlePos.LastPosition) >= seg2)
					{
						particlePos.Position->push_back(SParticlePosition());

						SParticlePosition& pos = particlePos.Position->getLast();
						pos.Alpha = m_alpha;
						pos.Width = m_width;
						pos.Position = p.Position;

						particlePos.LastPosition = p.Position;
					}
				}
			}
		}

		void CParticleTrail::OnParticleBorn(CParticle &p)
		{

		}

		void CParticleTrail::OnParticleDead(CParticle &p)
		{

		}

		void CParticleTrail::OnSwapParticleData(CParticle &p1, CParticle &p2)
		{
			int index1 = p1.Index;
			int index2 = p2.Index;

			STrailInfo t = m_trails[index1];

			m_trails[index1] = m_trails[index2];
			m_trails[index2] = t;
		}

		void CParticleTrail::OnGroupDestroy()
		{
			m_group = NULL;
		}
	}
}