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
#include "CSkinnedMeshRenderer.h"

#include "Culling/CCullingData.h"
#include "Material/Shader/CShaderManager.h"
#include "Material/Shader/ShaderCallback/CShaderSH.h"

#include "Entity/CEntityManager.h"

namespace Skylicht
{
	CSkinnedMeshRenderer::CSkinnedMeshRenderer()
	{
		m_pipelineType = IRenderPipeline::Mix;
	}

	CSkinnedMeshRenderer::~CSkinnedMeshRenderer()
	{

	}

	void CSkinnedMeshRenderer::beginQuery(CEntityManager* entityManager)
	{
		m_meshs.set_used(0);
		m_transforms.set_used(0);
		m_indirectLightings.set_used(0);

		m_transparents.set_used(0);
	}

	void CSkinnedMeshRenderer::onQuery(CEntityManager* entityManager, CEntity* entity)
	{
		CRenderMeshData* meshData = (CRenderMeshData*)entity->getDataByIndex(CRenderMeshData::DataTypeIndex);
		if (meshData != NULL)
		{
			if (meshData->isSkinnedMesh() == true && meshData->isSoftwareSkinning() == false)
			{
				bool cullingVisible = true;

				// get culling result from CCullingSystem
				CCullingData* cullingData = (CCullingData*)entity->getDataByIndex(CCullingData::DataTypeIndex);
				if (cullingData != NULL)
					cullingVisible = cullingData->Visible;

				// only render visible culling mesh
				if (cullingVisible == true)
				{
					m_meshs.push_back(meshData);
				}
			}
		}
	}

	void CSkinnedMeshRenderer::init(CEntityManager* entityManager)
	{

	}

	int cmpRenderSkinMeshFunc(const void* a, const void* b)
	{
		CRenderMeshData* pa = *((CRenderMeshData**)a);
		CRenderMeshData* pb = *((CRenderMeshData**)b);

		CMesh* meshA = pa->getMesh();
		CMesh* meshB = pb->getMesh();

		std::vector<CMaterial*>& materialsA = meshA->Material;
		std::vector<CMaterial*>& materialsB = meshB->Material;

		// no material, compare by mesh
		if (materialsA.size() == 0 || materialsA.size() == 0)
		{
			if (meshA == meshB)
				return 0;

			return meshA < meshB ? -1 : 1;
		}

		CMaterial* materialA = materialsA[0];
		CMaterial* materialB = materialsB[0];

		// compare by mesh
		if (materialA == NULL || materialB == NULL)
		{
			if (meshA == meshB)
				return 0;

			return meshA < meshB ? -1 : 1;
		}

		// compare by texture
		ITexture* textureA = materialA->getTexture(0);
		ITexture* textureB = materialB->getTexture(0);

		// if no texture
		if (textureA == NULL || textureB == NULL)
		{
			if (materialA == materialB)
				return 0;
			return materialA < materialB ? -1 : 1;
		}

		// sort by texture 0
		if (textureA == textureB)
			return 0;

		return textureA < textureB ? -1 : 1;
	}

	void CSkinnedMeshRenderer::update(CEntityManager* entityManager)
	{
		// need sort render by material, texture, mesh		
		u32 count = m_meshs.size();

		// need sort by material
		qsort(m_meshs.pointer(), count, sizeof(CRenderMeshData*), cmpRenderSkinMeshFunc);

		// get world transform			
		for (u32 i = 0; i < count; i++)
		{
			CEntity* entity = entityManager->getEntity(m_meshs[i]->EntityIndex);

			CWorldTransformData* transform = (CWorldTransformData*)entity->getDataByIndex(CWorldTransformData::DataTypeIndex);
			CIndirectLightingData* indirect = (CIndirectLightingData*)entity->getDataByIndex(CIndirectLightingData::DataTypeIndex);

			m_transforms.push_back(transform);
			m_indirectLightings.push_back(indirect);
		}
	}

	void CSkinnedMeshRenderer::render(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();

		CRenderMeshData** meshs = m_meshs.pointer();
		CWorldTransformData** transforms = m_transforms.pointer();
		CIndirectLightingData** indirectLighting = m_indirectLightings.pointer();

		CShaderManager* shaderManager = CShaderManager::getInstance();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		for (u32 i = 0, n = m_meshs.size(); i < n; i++)
		{
			CRenderMeshData* renderMeshData = m_meshs[i];

			CIndirectLightingData* lightingData = indirectLighting[i];
			if (lightingData != NULL)
			{
				if (lightingData->Type == CIndirectLightingData::SH9)
					CShaderSH::setSH9(lightingData->SH);
			}

			CSkinnedMesh* mesh = (CSkinnedMesh*)renderMeshData->getMesh();

			// set bone matrix to shader callback
			shaderManager->BoneMatrix = mesh->SkinningMatrix;

			// set transform
			driver->setTransform(video::ETS_WORLD, transforms[i]->World);

			bool haveTransparent = false;

			// render mesh
			for (u32 j = 0, m = mesh->getMeshBufferCount(); j < m; j++)
			{
				CMaterial* material = mesh->Material[j];
				if (material == NULL)
				{
					// draw opaque mesh because unknown material
					rp->drawMeshBuffer(mesh, j, entityManager, renderMeshData->EntityIndex, true);
				}
				else if (material->getShader() != NULL && material->getShader()->isOpaque() == false)
				{
					// draw transparent material later
					haveTransparent = true;
				}
				else
				{
					// draw opaque mesh
					rp->drawMeshBuffer(mesh, j, entityManager, renderMeshData->EntityIndex, true);
				}
			}

			if (haveTransparent == true)
			{
				// this will render in transparent pass
				m_transparents.push_back(i);
			}
		}
	}

	void CSkinnedMeshRenderer::renderTransparent(CEntityManager* entityManager)
	{
		u32 numTransparent = m_transparents.size();
		if (numTransparent == 0)
			return;

		IVideoDriver* driver = getVideoDriver();

		CRenderMeshData** meshs = m_meshs.pointer();
		CWorldTransformData** transforms = m_transforms.pointer();

		CShaderManager* shaderManager = CShaderManager::getInstance();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		for (u32 i = 0; i < numTransparent; i++)
		{
			u32 meshID = m_transparents[i];

			CRenderMeshData* renderMeshData = m_meshs[meshID];
			CSkinnedMesh* mesh = (CSkinnedMesh*)renderMeshData->getMesh();

			// set bone matrix to shader callback
			shaderManager->BoneMatrix = mesh->SkinningMatrix;

			// set transform
			driver->setTransform(video::ETS_WORLD, transforms[meshID]->World);

			// render mesh
			for (u32 j = 0, m = mesh->getMeshBufferCount(); j < m; j++)
			{
				CMaterial* material = mesh->Material[j];

				if (material != NULL &&
					material->getShader() != NULL &&
					material->getShader()->isOpaque() == false)
				{
					rp->drawMeshBuffer(mesh, j, entityManager, renderMeshData->EntityIndex, true);
				}
			}
		}
	}
}