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
#include "CParentRelativeSystem.h"

#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/CSubGroup.h"

namespace Skylicht
{
	namespace Particle
	{
		CParentRelativeSystem::CParentRelativeSystem()
		{

		}

		CParentRelativeSystem::~CParentRelativeSystem()
		{

		}

		void CParentRelativeSystem::update(CParticle *particles, int num, CGroup *group, float dt)
		{
			CSubGroup *subGroup = dynamic_cast<CSubGroup*>(group);
			if (subGroup == NULL)
				return;

			CGroup *parentGroup = subGroup->getParentGroup();

			CParticle *baseParticles = parentGroup->getParticlePointer();
			CParticle *p;

#pragma omp parallel for private(p)
			for (int i = 0; i < num; i++)
			{
				p = particles + i;

				if (p->ParentIndex >= 0)
				{
					p->Position = (p->Position - p->LastPosition) + baseParticles[p->ParentIndex].Position;
				}
			}
		}
	}
}