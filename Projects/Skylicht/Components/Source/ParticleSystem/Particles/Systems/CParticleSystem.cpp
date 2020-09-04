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
#include "CParticleSystem.h"

#include "ParticleSystem/Particles/CParticle.h"
#include "ParticleSystem/Particles/CGroup.h"

namespace Skylicht
{
	namespace Particle
	{
		CParticleSystem::CParticleSystem()
		{

		}

		CParticleSystem::~CParticleSystem()
		{

		}

		void CParticleSystem::update(CParticle *particles, int num, CGroup *group, float dt)
		{
			dt = dt * 0.001f;

			core::vector3df gravity = group->Gravity * dt;

			float friction = group->Friction * dt;

			CParticle *p;
			float *params;

			std::vector<CModel*>& listModel = group->getModels();
			std::vector<EParticleParams> listParams;
			std::vector<CInterpolator*> listInterpolator;

			for (CModel *m : listModel)
			{
				listInterpolator.push_back(m->getInterpolator());
				listParams.push_back(m->getType());
			}

			CModel** models = listModel.data();
			CInterpolator** interpolators = listInterpolator.data();
			EParticleParams* paramTypes = listParams.data();

			u32 numModels = listModel.size();

#pragma omp parallel for private(p, params)
			for (int i = 0; i < num; i++)
			{
				p = particles + i;
				params = p->Params;

				// update life time
				p->Age = p->Age + dt;

				if (!p->Immortal)
					p->Life -= dt;

				// update position
				p->LastPosition = p->Position;

				p->Position = p->Position + p->Velocity * dt;

				// update gravity
				p->Velocity = p->Velocity + gravity;

				// update friction
				if (group->Friction > 0.0f)
				{
					float f = 1.0f - core::min_(1.0f, friction / params[Mass]);
					p->Velocity *= f;
				}

				// update interpolate parameters
				float x = p->Age / p->LifeTime;

				for (u32 j = 0; j < numModels; j++)
				{
					// linear
					float y = x;

					if (interpolators[j] != NULL)
					{
						// interpolate
						y = interpolators[j]->interpolate(x);
					}

					EParticleParams t = paramTypes[j];

					// update param value
					params[t] = p->StartValue[t] + (p->EndValue[t] - p->StartValue[t]) * y;
				}
			}
		}
	}
}