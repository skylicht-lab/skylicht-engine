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
#include "Entity/CEntityManager.h"
#include "CParticleRenderer.h"
#include "Material/Shader/ShaderCallback/CShaderParticle.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

namespace Skylicht
{
	namespace Particle
	{
		CParticleRenderer::CParticleRenderer() :
			m_group(NULL)
		{
			m_renderPass = Transparent;
		}

		CParticleRenderer::~CParticleRenderer()
		{

		}

		void CParticleRenderer::beginQuery(CEntityManager* entityManager)
		{
			if (m_group == NULL)
			{
				const u32 particle[] = GET_LIST_ENTITY_DATA(CParticleBufferData);
				m_group = entityManager->createGroupFromVisible(particle, 1);
			}
		}

		void CParticleRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
		{
			entities = m_group->getEntities();
			numEntity = m_group->getEntityCount();

			for (int i = 0; i < numEntity; i++)
			{
				CEntity* entity = entities[i];

				CParticleBufferData* particleData = GET_ENTITY_DATA(entity, CParticleBufferData);

				// update bbox for culling
				// use last frame data
				CCullingBBoxData* box = GET_ENTITY_DATA(entity, CCullingBBoxData);

				CGroup** groups = particleData->Groups.pointer();
				for (u32 i = 0, n = particleData->Groups.size(); i < n; i++)
				{
					CGroup* g = groups[i];
					if (i == 0)
						box->BBox = g->getBBox();
					else
						box->BBox.addInternalBox(g->getBBox());
				}
			}
		}

		void CParticleRenderer::init(CEntityManager* entityManager)
		{

		}

		void CParticleRenderer::update(CEntityManager* entityManager)
		{
			if (m_group->getEntityCount() == 0)
				return;

			IVideoDriver* driver = getVideoDriver();

			irr::core::matrix4 invModelView;
			{
				irr::core::matrix4 modelView(driver->getTransform(video::ETS_VIEW));
				modelView.getInversePrimitive(invModelView); // wont work for odd modelview matrices (but should happen in very special cases)
			}

			core::vector3df look(invModelView[8], invModelView[9], invModelView[10]);
			core::vector3df up(invModelView[4], invModelView[5], invModelView[6]);

			look.normalize();
			up.normalize();

			CShaderParticle::setViewUp(up);
			CShaderParticle::setViewLook(look);
		}

		void CParticleRenderer::render(CEntityManager* entityManager)
		{

		}

		void CParticleRenderer::renderTransparent(CEntityManager* entityManager)
		{
			if (m_group->getEntityCount() == 0)
				return;

			CEntity** entities = m_group->getEntities();
			int numEntity = m_group->getEntityCount();

			for (int i = 0; i < numEntity; i++)
			{
				CEntity* entity = entities[i];

				CParticleBufferData* data = GET_ENTITY_DATA(entity, CParticleBufferData);
				CCullingData* culling = GET_ENTITY_DATA(entity, CCullingData);
				CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);

				// update group before render
				for (u32 j = 0, m = data->Groups.size(); j < m; j++)
				{
					data->Groups[j]->update(culling->Visible);
				}

				// render
				if (culling->Visible == true)
					renderParticleGroup(data, transform->World);
			}
		}

		void CParticleRenderer::renderEmission(CEntityManager* entityManager)
		{
			if (m_group->getEntityCount() == 0)
				return;

			CEntity** entities = m_group->getEntities();
			int numEntity = m_group->getEntityCount();

			for (int i = 0; i < numEntity; i++)
			{
				CEntity* entity = entities[i];

				CParticleBufferData* data = GET_ENTITY_DATA(entity, CParticleBufferData);
				CCullingData* culling = GET_ENTITY_DATA(entity, CCullingData);
				CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);

				// render
				if (culling->Visible == true)
					renderParticleGroupEmission(data, transform->World);
			}
		}

		void CParticleRenderer::renderParticleGroup(CParticleBufferData* data, const core::matrix4& world)
		{
			IVideoDriver* driver = getVideoDriver();
			driver->setTransform(video::ETS_WORLD, world);

			CGroup** groups = data->Groups.pointer();
			for (u32 i = 0, n = data->Groups.size(); i < n; i++)
			{
				CGroup* g = groups[i];
				if (g->getCurrentParticleCount() > 0)
				{
					IRenderer* renderer = g->getRenderer();
					if (renderer != NULL)
					{
						renderGroup(driver, g);
					}
				}
			}
		}

		void CParticleRenderer::renderParticleGroupEmission(CParticleBufferData* data, const core::matrix4& world)
		{
			IVideoDriver* driver = getVideoDriver();
			driver->setTransform(video::ETS_WORLD, world);

			CGroup** groups = data->Groups.pointer();
			for (u32 i = 0, n = data->Groups.size(); i < n; i++)
			{
				CGroup* g = groups[i];
				if (g->getCurrentParticleCount() > 0)
				{
					IRenderer* renderer = g->getRenderer();
					if (renderer != NULL && renderer->isEmission())
					{
						renderGroup(driver, g);
					}
				}
			}
		}

		void CParticleRenderer::renderGroup(IVideoDriver* driver, Particle::CGroup* group)
		{
			IMeshBuffer* buffer = NULL;
			IRenderer* renderer = group->getRenderer();

			if (renderer->useInstancing() == true)
				buffer = group->getIntancing()->getMeshBuffer();
			else
				buffer = group->getParticleBuffer()->getMeshBuffer();

			CShaderParticle::setOrientationUp(group->OrientationUp);
			CShaderParticle::setOrientationNormal(group->OrientationNormal);

			if (renderer != NULL)
				CShaderMaterial::setMaterial(renderer->getMaterial());

			driver->setMaterial(buffer->getMaterial());
			driver->drawMeshBuffer(buffer);
		}
	}
}