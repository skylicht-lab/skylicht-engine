/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "CLightCullingSystem.h"
#include "Entity/CEntityManager.h"
#include "Material/Shader/CShaderManager.h"

namespace Skylicht
{
	CLightCullingSystem::CLightCullingSystem()
	{
		m_pipelineType = IRenderPipeline::Mix;
	}

	CLightCullingSystem::~CLightCullingSystem()
	{

	}

	void CLightCullingSystem::beginQuery(CEntityManager* entityManager)
	{
		m_cullings.set_used(0);
		m_visible.set_used(0);
		m_transforms.set_used(0);
		m_invTransforms.set_used(0);
	}

	void CLightCullingSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CLightCullingData* culling = GET_ENTITY_DATA(entity, CLightCullingData);

			if (culling != NULL)
			{
				CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				CWorldInverseTransformData* invTransform = GET_ENTITY_DATA(entity, CWorldInverseTransformData);

				m_cullings.push_back(culling);
				m_transforms.push_back(transform);
				m_invTransforms.push_back(invTransform);
			}
		}
	}

	void CLightCullingSystem::init(CEntityManager* entityManager)
	{

	}

	void CLightCullingSystem::update(CEntityManager* entityManager)
	{
		CLightCullingData** cullings = m_cullings.pointer();
		CWorldTransformData** transforms = m_transforms.pointer();
		CWorldInverseTransformData** invTransforms = m_invTransforms.pointer();

		core::matrix4 invTrans;

		CCamera* camera = entityManager->getCamera();
		core::aabbox3df camBox = camera->getViewFrustum().getBoundingBox();
		core::vector3df camPos = camera->getGameObject()->getPosition();

		u32 numEntity = m_cullings.size();
		for (u32 i = 0; i < numEntity; i++)
		{
			// get mesh bbox
			CLightCullingData* culling = cullings[i];
			CWorldTransformData* transform = transforms[i];
			CWorldInverseTransformData* invTransform = invTransforms[i];

			culling->Visible = true;

			// transform world bbox
			core::aabbox3df lightBox = culling->BBox;
			transform->World.transformBoxEx(lightBox);

			// 1. Detect by bounding box
			culling->Visible = lightBox.intersectsWithBox(camBox);
			culling->CameraDistance = transform->World.getTranslation().getDistanceFromSQ(camPos);

			// 2. Detect algorithm
			if (culling->Visible == true)
			{
				SViewFrustum frust = camera->getViewFrustum();

				// transform the frustum to the node's current absolute transformation
				invTrans = invTransform->WorldInverse;
				frust.transform(invTrans);

				core::vector3df edges[8];
				culling->BBox.getEdges(edges);

				for (s32 i = 0; i < scene::SViewFrustum::VF_PLANE_COUNT; ++i)
				{
					bool boxInFrustum = false;
					for (u32 j = 0; j < 8; ++j)
					{
						if (frust.planes[i].classifyPointRelation(edges[j]) != core::ISREL3D_FRONT)
						{
							boxInFrustum = true;
							break;
						}
					}

					if (!boxInFrustum)
					{
						culling->Visible = false;
						break;
					}
				}
			}

			// add list visible light
			if (culling->Visible == true)
			{
				int n = (int)m_visible.size();
				if (n == 0 || culling->CameraDistance > m_visible[n - 1]->CameraDistance)
				{
					// add end
					m_visible.push_back(culling);
				}
				else
				{
					// insert sort
					for (int i = 0; i < n; i++)
					{
						if (culling->CameraDistance < m_visible[i]->CameraDistance)
						{
							m_visible.insert(culling, i);
							break;
						}
					}
				}
			}
		}
	}

	void CLightCullingSystem::render(CEntityManager* entityManager)
	{

	}

	void CLightCullingSystem::postRender(CEntityManager* entityManager)
	{

	}
}