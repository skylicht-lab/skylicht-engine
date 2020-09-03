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

#pragma once

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
		}

		void CParticleRenderer::onQuery(CEntityManager *entityManager, CEntity *entity)
		{
			CParticleBufferData *particleData = entity->getData<CParticleBufferData>();
			if (particleData != NULL)
			{
				CWorldTransformData *transform = entity->getData<CWorldTransformData>();
				if (transform != NULL)
				{
					m_particles.push_back(particleData);
					m_transforms.push_back(transform);
				}
			}
		}

		void CParticleRenderer::init(CEntityManager *entityManager)
		{

		}

		void CParticleRenderer::update(CEntityManager *entityManager)
		{
			IVideoDriver *driver = getVideoDriver();

			irr::core::matrix4 invModelView;
			{
				irr::core::matrix4 modelView(driver->getTransform(video::ETS_VIEW));
				modelView *= driver->getTransform(video::ETS_WORLD);
				modelView.getInversePrimitive(invModelView); // wont work for odd modelview matrices (but should happen in very special cases)
			}

			core::vector3df look(invModelView[8], invModelView[9], invModelView[10]);
			core::vector3df up(invModelView[4], invModelView[5], invModelView[6]);

			look.normalize();
			up.normalize();

			CShaderParticle::setViewUp(up);
			CShaderParticle::setViewLook(look);
		}

		void CParticleRenderer::render(CEntityManager *entityManager)
		{

		}

		void CParticleRenderer::renderTransparent(CEntityManager *entityManager)
		{
			CParticleBufferData** particles = m_particles.pointer();
			CWorldTransformData** transforms = m_transforms.pointer();

			for (u32 i = 0, n = m_particles.size(); i < n; i++)
			{
				renderParticleGroup(transforms[i]->World, particles[i]);
			}
		}

		void CParticleRenderer::renderParticleGroup(const core::matrix4& transform, CParticleBufferData *data)
		{
			IVideoDriver *driver = getVideoDriver();
			driver->setTransform(video::ETS_WORLD, transform);

			CGroup** groups = data->Groups.pointer();
			for (u32 i = 0, n = data->Groups.size(); i < n; i++)
			{
				CGroup *g = groups[i];
				if (g->getCurrentParticleCount() > 0)
				{
					IMeshBuffer *buffer = g->getIntancing()->getMeshBuffer();

					driver->setMaterial(buffer->getMaterial());
					driver->drawMeshBuffer(buffer);
				}
			}
		}
	}
}