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
#include "CParticleTrailRenderer.h"


namespace Skylicht
{
	namespace Particle
	{
		CParticleTrailRenderer::CParticleTrailRenderer()
		{

		}

		CParticleTrailRenderer::~CParticleTrailRenderer()
		{

		}

		void CParticleTrailRenderer::beginQuery()
		{
			m_trails.set_used(0);
			m_cullings.set_used(0);
		}

		void CParticleTrailRenderer::onQuery(CEntityManager *entityManager, CEntity *entity)
		{
			CParticleTrailData *trail = entity->getData<CParticleTrailData>();
			if (trail != NULL)
			{
				CVisibleData *visible = entity->getData<CVisibleData>();

				if (visible->Visible == true)
					m_trails.push_back(trail);

				CCullingData *culling = entity->getData<CCullingData>();
				m_cullings.push_back(culling);
			}
		}

		void CParticleTrailRenderer::init(CEntityManager *entityManager)
		{

		}

		void CParticleTrailRenderer::update(CEntityManager *entityManager)
		{
			CParticleTrailData** trails = m_trails.pointer();

			for (u32 i = 0, n = m_trails.size(); i < n; i++)
			{
				CParticleTrailData *trailData = trails[i];

				// update particle trail
				u32 m = trailData->Trails.size();
				for (u32 j = 0; j < m; j++)
				{
					trailData->Trails[j]->update();
				}
			}
		}

		void CParticleTrailRenderer::render(CEntityManager *entityManager)
		{

		}

		void CParticleTrailRenderer::renderTransparent(CEntityManager *entityManager)
		{

		}
	}
}