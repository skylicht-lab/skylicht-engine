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
#include "CCullingSystem.h"
#include "CCullingBBoxData.h"
#include "Entity/CEntityManager.h"
#include "RenderPipeline/IRenderPipeline.h"
#include "Camera/CCamera.h"

#include "RenderPipeline/CShadowMapRP.h"

namespace Skylicht
{
	bool CCullingSystem::s_useCacheCulling = false;

	CCullingSystem::CCullingSystem() :
		m_group(NULL)
	{
		m_pipelineType = IRenderPipeline::Mix;
	}

	CCullingSystem::~CCullingSystem()
	{

	}

	void CCullingSystem::beginQuery(CEntityManager* entityManager)
	{
		if (m_group == NULL)
		{
			const u32 type[] = { DATA_TYPE_INDEX(CCullingData) };
			m_group = entityManager->createGroup(type, 1);
		}

		if (s_useCacheCulling)
			return;
	}

	void CCullingSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		if (s_useCacheCulling)
			return;

		entities = m_group->getEntities();
		numEntity = m_group->getEntityCount();

		m_bboxAndMaterials.reset();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CCullingData* culling = GET_ENTITY_DATA(entity, CCullingData);
			CVisibleData* visible = GET_ENTITY_DATA(entity, CVisibleData);

			culling->CullingLayer = visible->CullingLayer;

			if (visible->Culled || !visible->Visible)
			{
				culling->Visible = false;
			}
			else
			{
				CRenderMeshData* mesh = GET_ENTITY_DATA(entity, CRenderMeshData);
				if (mesh != NULL)
				{
					CMesh* meshObj = mesh->getMesh();

					SBBoxAndMaterial* m = m_bboxAndMaterials.getPush();
					m->Entity = entity;
					m->Culling = culling;
					m->BBox = meshObj->getBoundingBoxPtr();
					m->Materials = &meshObj->Materials;
				}
				else
				{
					CCullingBBoxData* bbox = GET_ENTITY_DATA(entity, CCullingBBoxData);
					if (bbox != NULL)
					{
						SBBoxAndMaterial* m = m_bboxAndMaterials.getPush();
						m->Entity = entity;
						m->Culling = culling;
						m->BBox = &bbox->BBox;
						m->Materials = &bbox->Materials;
					}
				}
			}
		}
	}

	void CCullingSystem::init(CEntityManager* entityManager)
	{

	}

	void CCullingSystem::update(CEntityManager* entityManager)
	{
		IRenderPipeline* rp = entityManager->getRenderPipeline();
		if (rp == NULL)
			return;

		core::matrix4 invTrans;

		// camera
		CCamera* camera = entityManager->getCamera();
		u32 cameraCullingMask = camera->getCullingMask();
		const core::aabbox3df& cameraBox = camera->getViewFrustum().getBoundingBox();

		int count = m_bboxAndMaterials.count();
		SBBoxAndMaterial* bboxMats = m_bboxAndMaterials.pointer();

		for (int i = 0; i < count; i++)
		{
			SBBoxAndMaterial* bbBoxMat = &bboxMats[i];

			CEntity* entity = bbBoxMat->Entity;
			CCullingData* culling = bbBoxMat->Culling;

			if (s_useCacheCulling)
			{
				// if we have the last test result
				if (culling->CameraCulled == true)
					continue;
			}
			else
			{
				culling->CameraCulled = false;
			}

			culling->Visible = true;

			// check camera mask culling
			u32 test = cameraCullingMask & culling->CullingLayer;
			if (test == 0)
			{
				culling->Visible = false;
				continue;
			}

			// check material first
			if (bbBoxMat->Materials != NULL)
			{
				CMaterial** materials = bbBoxMat->Materials->data();
				int materialCount = (int)bbBoxMat->Materials->size();

				for (int j = 0; j < materialCount; j++)
				{
					CMaterial* m = materials[j];
					if (m != NULL && rp->canRenderMaterial(m) == false)
					{
						culling->Visible = false;
						break;
					}
				}
			}

			if (culling->Visible == false)
				continue;

			if (s_useCacheCulling)
				continue;

			// transform world bbox
			culling->BBox = *bbBoxMat->BBox;

			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			transform->World.transformBoxEx(culling->BBox);

			// 1. Detect by bounding box
			if (rp->getType() == IRenderPipeline::ShadowMap)
			{
				CShadowMapRP* shadowMapRP = (CShadowMapRP*)rp;

				const core::aabbox3df& box = shadowMapRP->getFrustumBox();
				culling->CameraCulled = !culling->BBox.intersectsWithBox(box);
				culling->Visible = !culling->CameraCulled;
				continue;
			}
			else
			{
				culling->CameraCulled = !culling->BBox.intersectsWithBox(cameraBox);
				culling->Visible = !culling->CameraCulled;
			}

			// 2. Detect algorithm
			if (culling->Visible == true)
			{
				if (culling->Type == CCullingData::FrustumBox)
				{
					CWorldInverseTransformData* invTransform = GET_ENTITY_DATA(entity, CWorldInverseTransformData);
					if (invTransform)
					{
						// transform the frustum to the node's current absolute transformation
						invTrans = invTransform->WorldInverse;

						SViewFrustum frust = camera->getViewFrustum();
						frust.transform(invTrans);

						core::vector3df edges[8];
						bbBoxMat->BBox->getEdges(edges);

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
								culling->CameraCulled = true;
								culling->Visible = false;
								break;
							}
						}
					} // if invTransform
				}
			}
		}
	}

	void CCullingSystem::render(CEntityManager* entityManager)
	{

	}

	void CCullingSystem::postRender(CEntityManager* entityManager)
	{

	}
}