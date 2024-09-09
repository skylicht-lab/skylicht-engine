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
#include "Entity/CEntityManager.h"
#include "Material/Shader/CShaderManager.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

#include "TextureManager/CTextureManager.h"

namespace Skylicht
{
	namespace Particle
	{
		CParticleTrailRenderer::CParticleTrailRenderer() :
			m_group(NULL)
		{

		}

		CParticleTrailRenderer::~CParticleTrailRenderer()
		{

		}

		void CParticleTrailRenderer::beginQuery(CEntityManager* entityManager)
		{
			if (m_group == NULL)
			{
				const u32 particle[] = GET_LIST_ENTITY_DATA(CParticleTrailData);
				m_group = entityManager->createGroupFromVisible(particle, 1);
			}
		}

		void CParticleTrailRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
		{

		}

		void CParticleTrailRenderer::init(CEntityManager* entityManager)
		{

		}

		void CParticleTrailRenderer::update(CEntityManager* entityManager)
		{
			CCamera* camera = entityManager->getCamera();

			CEntity** entities = m_group->getEntities();
			int numEntity = m_group->getEntityCount();

			for (int i = 0; i < numEntity; i++)
			{
				CParticleTrailData* trailData = GET_ENTITY_DATA(entities[i], CParticleTrailData);

				// update particle trail
				u32 m = trailData->Trails.size();
				for (u32 j = 0; j < m; j++)
				{
					trailData->Trails[j]->update(camera);
				}
			}
		}

		void CParticleTrailRenderer::render(CEntityManager* entityManager)
		{

		}

		void CParticleTrailRenderer::renderTransparent(CEntityManager* entityManager)
		{
			CEntity** entities = m_group->getEntities();
			int numEntity = m_group->getEntityCount();

			IVideoDriver* driver = getVideoDriver();
			driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

			for (int i = 0; i < numEntity; i++)
			{
				CParticleTrailData* trailData = GET_ENTITY_DATA(entities[i], CParticleTrailData);

				u32 m = trailData->Trails.size();
				for (u32 j = 0; j < m; j++)
				{
					CParticleTrail* p = trailData->Trails[j];

					IMeshBuffer* mb = p->getMeshBuffer();

					CShaderMaterial::setMaterial(p->getMaterial());

					driver->setMaterial(mb->getMaterial());
					driver->drawMeshBuffer(mb);
				}
			}
		}

		void CParticleTrailRenderer::renderEmission(CEntityManager* entityManager)
		{
			CEntity** entities = m_group->getEntities();
			int numEntity = m_group->getEntityCount();

			IVideoDriver* driver = getVideoDriver();
			driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

			for (int i = 0; i < numEntity; i++)
			{
				CParticleTrailData* trailData = GET_ENTITY_DATA(entities[i], CParticleTrailData);

				u32 m = trailData->Trails.size();
				for (u32 j = 0; j < m; j++)
				{
					CParticleTrail* p = trailData->Trails[j];

					IMeshBuffer* mb = p->getMeshBuffer();

					CShaderMaterial::setMaterial(p->getMaterial());

					driver->setMaterial(mb->getMaterial());
					driver->drawMeshBuffer(mb);
				}
			}
		}
	}
}