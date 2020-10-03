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
#include "CSubGroup.h"
#include "Systems/CParentRelativeSystem.h"

namespace Skylicht
{
	namespace Particle
	{
		CSubGroup::CSubGroup(CGroup *group) :
			m_parentGroup(group),
			m_followParentTransform(false)
		{
			m_parentGroup->addCallback(this);

			m_parentSystem = new CParentRelativeSystem();
			m_parentSystem->setEnable(false);

			addSystem(m_parentSystem);
		}

		CSubGroup::~CSubGroup()
		{
			if (m_parentGroup != NULL)
				m_parentGroup->removeCallback(this);

			removeSystem(m_parentSystem);
			delete m_parentSystem;
		}

		void CSubGroup::OnParticleDead(CParticle &p)
		{
			CParticle* particles = getParticlePointer();
			u32 n = getNumParticles();

			for (u32 i = 0; i < n; i++)
			{
				if (particles[i].ParentIndex == p.Index)
					particles[i].ParentIndex = -1;
			}
		}

		void CSubGroup::OnSwapParticleData(CParticle &p1, CParticle &p2)
		{
			CParticle* particles = getParticlePointer();
			u32 n = getNumParticles();

			for (u32 i = 0; i < n; i++)
			{
				if (particles[i].ParentIndex == p1.Index)
					particles[i].ParentIndex = p2.Index;
				else if (particles[i].ParentIndex == p2.Index)
					particles[i].ParentIndex = p1.Index;
			}
		}

		void CSubGroup::OnGroupDestroy()
		{
			m_parentGroup = NULL;
		}

		void CSubGroup::bornParticle()
		{
			u32 emiterId = 0;
			u32 emiterLaunch = m_launch.size();

			// parent particle
			CParticle* baseParticles = m_parentGroup->getParticlePointer();
			u32 numberParticles = m_parentGroup->getNumParticles();

			for (u32 i = emiterId; i < emiterLaunch; i++)
			{
				SLaunchParticle &launch = m_launch[i];
				if (launch.Number > 0)
				{
					// emit new particles from base particle
					for (u32 k = 0; k < numberParticles; k++)
					{
						CParticle& base = baseParticles[k];

						// base orientation
						m_position = base.Position;
						m_direction = base.Velocity;
						m_direction.normalize();
						m_rotate.rotationFromTo(CTransform::s_oy, m_direction);

						// init new particle
						CParticle* newParticles = create(launch.Number);

						for (u32 j = 0, n = launch.Number; j < n; j++)
						{
							CParticle &p = newParticles[j];
							p.ParentIndex = (s32)base.Index;
							launchParticle(p, launch);
							launch.Number = n;
						}
					}

					launch.Number = 0;
				}
			}
		}

		core::vector3df CSubGroup::getTransformPosition(const core::vector3df& pos)
		{
			if (m_followParentTransform == true)
				return pos;

			core::vector3df ret = m_position + pos;
			return ret;
		}

		core::vector3df CSubGroup::getTransformVector(const core::vector3df& vec)
		{
			core::vector3df ret = m_rotate * vec;
			return ret;
		}

		void CSubGroup::setFollowParentTransform(bool b)
		{
			m_followParentTransform = b;

			m_parentSystem->setEnable(b);
		}
	}
}