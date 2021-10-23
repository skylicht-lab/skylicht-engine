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
	CCullingSystem::CCullingSystem()
	{
		m_pipelineType = IRenderPipeline::Mix;
	}

	CCullingSystem::~CCullingSystem()
	{

	}

	void CCullingSystem::beginQuery(CEntityManager* entityManager)
	{
		m_cullings.set_used(0);
		m_transforms.set_used(0);
		m_invTransforms.set_used(0);
		m_bboxAndMaterials.set_used(0);
	}

	void CCullingSystem::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CCullingData* culling = entity->getData<CCullingData>();
		if (culling == NULL)
			return;

		CVisibleData* visible = entity->getData<CVisibleData>();
		if (visible != NULL)
			culling->CullingLayer = visible->CullingLayer;
		else
			culling->CullingLayer = 1;

		if (culling != NULL && visible != NULL && visible->Visible == false)
		{
			culling->Visible = false;
		}
		else if (culling != NULL)
		{
			CRenderMeshData* mesh = entity->getData<CRenderMeshData>();
			if (mesh != NULL)
			{
				CWorldTransformData* transform = entity->getData<CWorldTransformData>();
				CWorldInverseTransformData* invTransform = entity->getData<CWorldInverseTransformData>();

				if (transform != NULL)
				{
					m_cullings.push_back(culling);

					CMesh* meshObj = mesh->getMesh();

					m_bboxAndMaterials.push_back(
						SBBoxAndMaterial(
							meshObj->getBoundingBox(),
							&meshObj->Material
						)
					);

					m_transforms.push_back(transform);
					m_invTransforms.push_back(invTransform);
				}
			}
			else
			{
				CCullingBBoxData* bbox = entity->getData<CCullingBBoxData>();
				if (bbox != NULL)
				{
					CWorldTransformData* transform = entity->getData<CWorldTransformData>();
					CWorldInverseTransformData* invTransform = entity->getData<CWorldInverseTransformData>();

					m_cullings.push_back(culling);

					m_bboxAndMaterials.push_back(
						SBBoxAndMaterial(
							bbox->BBox,
							&bbox->Materials
						)
					);

					m_transforms.push_back(transform);
					m_invTransforms.push_back(invTransform);
				}
			}
		}
	}

	void CCullingSystem::init(CEntityManager* entityManager)
	{

	}

	void CCullingSystem::update(CEntityManager* entityManager)
	{
		CCullingData** cullings = m_cullings.pointer();
		SBBoxAndMaterial* bboxAndMaterials = m_bboxAndMaterials.pointer();
		CWorldTransformData** transforms = m_transforms.pointer();
		CWorldInverseTransformData** invTransforms = m_invTransforms.pointer();

		IRenderPipeline* rp = entityManager->getRenderPipeline();
		if (rp == NULL)
			return;

		core::matrix4 invTrans;

		// camera
		CCamera* camera = entityManager->getCamera();

		u32 numEntity = m_cullings.size();
		for (u32 i = 0; i < numEntity; i++)
		{
			// get mesh bbox
			CCullingData* culling = cullings[i];
			CWorldTransformData* transform = transforms[i];
			CWorldInverseTransformData* invTransform = invTransforms[i];
			SBBoxAndMaterial& bbBoxMat = bboxAndMaterials[i];

			culling->Visible = true;

			// check camera mask culling
			u32 test = camera->getCullingMask() & culling->CullingLayer;
			if (test == 0)
			{
				culling->Visible = false;
				continue;
			}

			// check material first
			if (bbBoxMat.Materials != NULL)
			{
				for (CMaterial* material : *bbBoxMat.Materials)
				{
					if (material != NULL && rp->canRenderMaterial(material) == false)
					{
						culling->Visible = false;
						break;
					}
				}
			}

			if (culling->Visible == false)
				continue;

			// transform world bbox
			culling->BBox = bbBoxMat.BBox;

			transform->World.transformBoxEx(culling->BBox);

			// 1. Detect by bounding box			
			SViewFrustum frust;

			if (rp->getType() == IRenderPipeline::ShadowMap)
			{
				CShadowMapRP* shadowMapRP = (CShadowMapRP*)rp;

				const core::aabbox3df& box = shadowMapRP->getFrustumBox();
				culling->Visible = culling->BBox.intersectsWithBox(box);
				continue;
			}
			else
			{
				frust = camera->getViewFrustum();
				culling->Visible = culling->BBox.intersectsWithBox(frust.getBoundingBox());
			}

			// 2. Detect algorithm
			if (culling->Visible == true)
			{
				if (culling->Type == CCullingData::FrustumBox && invTransform != NULL)
				{
					// transform the frustum to the node's current absolute transformation
					invTrans = invTransform->WorldInverse;
					frust.transform(invTrans);

					core::vector3df edges[8];
					bbBoxMat.BBox.getEdges(edges);

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
				else if (culling->Type == CCullingData::Occlusion)
				{
					// todo
					// later
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