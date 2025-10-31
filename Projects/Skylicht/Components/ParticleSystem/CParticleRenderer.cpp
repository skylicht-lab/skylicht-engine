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
#include "Material/Shader/ShaderCallback/CShaderSH.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"

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
				m_group = entityManager->findGroup(particle, 1);
				if (!m_group)
					m_group = entityManager->createGroupFromVisible(particle, 1);
			}
		}

		void CParticleRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
		{

		}

		void CParticleRenderer::init(CEntityManager* entityManager)
		{

		}

		void CParticleRenderer::update(CEntityManager* entityManager)
		{

		}

		void CParticleRenderer::render(CEntityManager* entityManager)
		{

		}

		const core::matrix4& CParticleRenderer::getTransformNoRotate(const core::matrix4& world)
		{
			m_transform.makeIdentity();
			m_transform.setTranslation(world.getTranslation());
			m_transform.setScale(world.getScale());
			return m_transform;
		}

		void CParticleRenderer::renderTransparent(CEntityManager* entityManager)
		{
			if (m_group->getEntityCount() == 0)
				return;

			IVideoDriver* driver = getVideoDriver();
			irr::core::matrix4 invModelView;
			{
				irr::core::matrix4 modelView(driver->getTransform(video::ETS_VIEW));
				modelView.getInversePrimitive(invModelView); // wont work for odd modelview matrices (but should happen in very special cases)
			}

			m_billboardLook.set(invModelView[8], invModelView[9], invModelView[10]);
			m_billboardUp.set(invModelView[4], invModelView[5], invModelView[6]);

			m_billboardLook.normalize();
			m_billboardUp.normalize();

			CEntity** entities = m_group->getEntities();
			int numEntity = m_group->getEntityCount();

			for (int i = 0; i < numEntity; i++)
			{
				CEntity* entity = entities[i];

				CParticleBufferData* data = GET_ENTITY_DATA(entity, CParticleBufferData);
				CCullingData* culling = GET_ENTITY_DATA(entity, CCullingData);

				if (culling->Visible == true)
				{
					for (u32 j = 0, m = data->AllGroups.size(); j < m; j++)
						data->AllGroups[j]->updateForRenderer();

					CIndirectLightingData* lightingData = GET_ENTITY_DATA(entity, CIndirectLightingData);
					if (lightingData != NULL)
						lightingData->applyShader();

					CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
					renderParticleGroup(data, getTransformNoRotate(transform->World));
				}
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

				if (culling->Visible == true)
				{
					CIndirectLightingData* lightingData = GET_ENTITY_DATA(entity, CIndirectLightingData);
					if (lightingData != NULL)
						lightingData->applyShader();

					CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
					renderParticleGroupEmission(data, getTransformNoRotate(transform->World));
				}
			}
		}

		void CParticleRenderer::renderParticleGroup(CParticleBufferData* data, const core::matrix4& world)
		{
			IVideoDriver* driver = getVideoDriver();
			driver->setTransform(video::ETS_WORLD, world);

			CGroup** groups = data->AllGroups.pointer();
			for (u32 i = 0, n = data->AllGroups.size(); i < n; i++)
			{
				CGroup* g = groups[i];

				if (g->getCurrentParticleCount() > 0 && g->Visible)
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

			CGroup** groups = data->AllGroups.pointer();
			for (u32 i = 0, n = data->AllGroups.size(); i < n; i++)
			{
				CGroup* g = groups[i];
				if (g->getCurrentParticleCount() > 0 && g->Visible)
				{
					IRenderer* renderer = g->getRenderer();
					if (renderer != NULL && renderer->isEmission())
					{
						SColorf color = CShaderMaterial::getColorIntensity();

						float intensity = renderer->getEmissionIntensity();
						CShaderMaterial::setColorIntensity(SColorf(intensity, intensity, intensity));

						renderGroup(driver, g);

						CShaderMaterial::setColorIntensity(color);
					}
				}
			}
		}

		void CParticleRenderer::renderGroup(IVideoDriver* driver, Particle::CGroup* group)
		{
			IMeshBuffer* buffer = NULL;
			IRenderer* renderer = group->getRenderer();

			if (group->UseOrientationAsBillboard)
			{
				CShaderParticle::setViewUp(group->OrientationUp);
				CShaderParticle::setViewLook(group->OrientationNormal);
			}
			else
			{
				CShaderParticle::setViewUp(m_billboardUp);
				CShaderParticle::setViewLook(m_billboardLook);
			}

			if (renderer->useInstancing() == true)
			{
				CParticleInstancing* instancing = group->getIntancing();
				if (instancing->getInstanceBuffer() &&
					instancing->getInstanceBuffer()->getVertexCount() > 0)
				{
					buffer = group->getIntancing()->getMeshBuffer();
				}
			}
			else
			{
				buffer = group->getParticleBuffer()->getMeshBuffer();
			}

			if (buffer && buffer->getIndexBuffer()->getIndexCount() > 0)
			{
				CShaderParticle::setOrientationUp(group->OrientationUp);
				CShaderParticle::setOrientationNormal(group->OrientationNormal);

				video::SMaterial& mat = buffer->getMaterial();

				if (renderer != NULL)
				{
					CMaterial* material = renderer->getMaterial();
					material->updateTexture(mat);

					CShaderMaterial::setMaterial(material);
				}

				driver->setMaterial(mat);
				driver->drawMeshBuffer(buffer);
			}
		}
	}
}
