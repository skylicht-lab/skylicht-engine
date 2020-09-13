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
#include "CParticleRenderer.h"
#include "Material/Shader/ShaderCallback/CShaderParticle.h"

namespace Skylicht
{
	namespace Particle
	{
		CParticleRenderer::CParticleRenderer()
		{

		}

		CParticleRenderer::~CParticleRenderer()
		{

		}

		void CParticleRenderer::beginQuery()
		{
			m_particles.set_used(0);
			m_transforms.set_used(0);
			m_cullings.set_used(0);
		}

		void CParticleRenderer::onQuery(CEntityManager *entityManager, CEntity *entity)
		{
			CParticleBufferData *particleData = entity->getData<CParticleBufferData>();
			if (particleData != NULL)
			{
				CWorldTransformData *transform = entity->getData<CWorldTransformData>();
				CVisibleData *visible = entity->getData<CVisibleData>();

				if (transform != NULL && visible->Visible)
				{
					m_particles.push_back(particleData);
					m_transforms.push_back(transform);

					// update bbox for culling
					// use last frame data
					CCullingBBoxData *box = entity->getData<CCullingBBoxData>();
					CWorldInverseTransformData *worldInverse = entity->getData<CWorldInverseTransformData>();

					CGroup** groups = particleData->Groups.pointer();
					for (u32 i = 0, n = particleData->Groups.size(); i < n; i++)
					{
						CGroup *g = groups[i];
						if (i == 0)
							box->BBox = g->getBBox();
						else
							box->BBox.addInternalBox(g->getBBox());
					}

					// convert world bbox to local
					worldInverse->WorldInverse.transformBoxEx(box->BBox);

					m_cullings.push_back(entity->getData<CCullingData>());
				}
			}
		}

		void CParticleRenderer::init(CEntityManager *entityManager)
		{

		}

		void CParticleRenderer::update(CEntityManager *entityManager)
		{
			if (m_particles.size() == 0)
				return;

			IVideoDriver *driver = getVideoDriver();

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

			CParticleBufferData** particles = m_particles.pointer();
			CWorldTransformData** transforms = m_transforms.pointer();
			CCullingData** cullings = m_cullings.pointer();

			for (u32 i = 0, n = m_particles.size(); i < n; i++)
			{
				CParticleBufferData* data = particles[i];

				for (u32 j = 0, m = data->Groups.size(); j < m; j++)
				{
					data->Groups[j]->setWorldMatrix(transforms[i]->World);
					data->Groups[j]->update(cullings[i]->Visible);
				}
			}
		}

		void CParticleRenderer::render(CEntityManager *entityManager)
		{
			if (m_particles.size() == 0)
				return;

		}

		void CParticleRenderer::renderTransparent(CEntityManager *entityManager)
		{
			if (m_particles.size() == 0)
				return;

			getVideoDriver()->setTransform(video::ETS_WORLD, core::IdentityMatrix);

			CParticleBufferData** particles = m_particles.pointer();
			CCullingData** cullings = m_cullings.pointer();

			for (u32 i = 0, n = m_particles.size(); i < n; i++)
			{
				if (cullings[i]->Visible == true)
					renderParticleGroup(particles[i]);
			}
		}

		void CParticleRenderer::renderParticleGroup(CParticleBufferData *data)
		{
			IVideoDriver *driver = getVideoDriver();

			CGroup** groups = data->Groups.pointer();
			for (u32 i = 0, n = data->Groups.size(); i < n; i++)
			{
				CGroup *g = groups[i];
				if (g->getCurrentParticleCount() > 0)
				{
					IMeshBuffer *buffer = g->getIntancing()->getMeshBuffer();

					CShaderParticle::setOrientationUp(g->OrientationUp);
					CShaderParticle::setOrientationNormal(g->OrientationNormal);

					driver->setMaterial(buffer->getMaterial());
					driver->drawMeshBuffer(buffer);
				}
			}
		}
	}
}