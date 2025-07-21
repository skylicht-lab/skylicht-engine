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
	bool g_useCacheCulling = false;

	void CCullingSystem::useCacheCulling(bool b)
	{
		g_useCacheCulling = b;
	}

	bool CCullingSystem::useCacheCulling()
	{
		return g_useCacheCulling;
	}

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
	}

	void CCullingSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		if (g_useCacheCulling)
			return;

		entities = m_group->getEntities();
		numEntity = m_group->getEntityCount();

		m_bboxAndMaterials.reset();

		CEntity* entity;
		CCullingData* culling;
		CVisibleData* visible;

		CRenderMeshData* mesh;
		CMesh* meshObj;
		SBBoxAndMaterial* m;
		CCullingBBoxData* bbox;
		CWorldTransformData* transform;

		for (int i = 0; i < numEntity; i++)
		{
			entity = entities[i];

			culling = GET_ENTITY_DATA(entity, CCullingData);
			visible = GET_ENTITY_DATA(entity, CVisibleData);
			transform = GET_ENTITY_DATA(entity, CWorldTransformData);

			culling->CullingLayer = visible->CullingLayer;
			culling->ShadowCasting = visible->ShadowCasting;

			if (visible->Culled || !visible->Visible)
			{
				culling->Visible = false;
				culling->NeedValidate |= transform->NeedValidate;
			}
			else
			{
				mesh = GET_ENTITY_DATA(entity, CRenderMeshData);
				if (mesh != NULL)
				{
					meshObj = mesh->getMesh();

					m = m_bboxAndMaterials.getPush();
					m->Entity = entity;
					m->Culling = culling;
					m->Transform = transform;
					m->BBox = meshObj->getBoundingBoxPtr();
					m->Materials = &mesh->getMaterials();

					m->Culling->NeedValidate |= transform->NeedValidate;
				}
				else
				{
					bbox = GET_ENTITY_DATA(entity, CCullingBBoxData);
					if (bbox != NULL)
					{
						m = m_bboxAndMaterials.getPush();
						m->Entity = entity;
						m->Culling = culling;
						m->Transform = transform;
						m->BBox = &bbox->BBox;
						m->Materials = &bbox->Materials;

						m->Culling->NeedValidate |= (bbox->NeedValidate || transform->NeedValidate);
						bbox->NeedValidate = false;
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
		const core::aabbox3df& cameraBox = camera->getViewFrustum().getBoundingBox();

		int count = m_bboxAndMaterials.count();

		SBBoxAndMaterial* bbBoxMats = m_bboxAndMaterials.pointer();
		SBBoxAndMaterial* bbBoxMat;
		CEntity* entity;
		CCullingData* culling;
		CMaterial** materials;
		CMaterial* m;
		int materialCount;

		for (int i = 0; i < count; i++)
		{
			bbBoxMat = &bbBoxMats[i];

			entity = bbBoxMat->Entity;
			culling = bbBoxMat->Culling;

			if (g_useCacheCulling)
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

			// check material first
			if (bbBoxMat->Materials != NULL)
			{
				materials = bbBoxMat->Materials->data();
				materialCount = (int)bbBoxMat->Materials->size();

				for (int j = 0; j < materialCount; j++)
				{
					m = materials[j];
					if (m != NULL && rp->canRenderMaterial(m) == false)
					{
						culling->Visible = false;
						break;
					}
				}
			}

			if (culling->Visible == false)
				continue;

			if (rp->getType() == IRenderPipeline::ShadowMap && !culling->ShadowCasting)
			{
				culling->Visible = false;
				continue;
			}

			if (g_useCacheCulling)
				continue;

			// update bbox
			if (culling->NeedValidate)
			{
				culling->BBox = *bbBoxMat->BBox;
				bbBoxMat->Transform->World.transformBoxEx(culling->BBox);
				culling->NeedValidate = false;
			}

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