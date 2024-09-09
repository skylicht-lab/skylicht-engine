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
#include "Material/Shader/ShaderCallback/CShaderLighting.h"

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

	void CSkinnedMeshRenderer::init(CEntityManager* entityManager)
	{

	}

	void CSkinnedMeshRenderer::beginQuery(CEntityManager* entityManager)
	{
		m_meshs.set_used(0);
		m_transparents.set_used(0);

		CMeshRenderSystem::beginQuery(entityManager);
	}

	void CSkinnedMeshRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		numEntity = m_groupMesh->getNumHardwareSkinnedMesh();
		entities = m_groupMesh->getHardwareSkinnedMeshes();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];
			CRenderMeshData* meshData = GET_ENTITY_DATA(entity, CRenderMeshData);

			bool cullingVisible = true;

			// get culling result from CCullingSystem
			CCullingData* cullingData = GET_ENTITY_DATA(entity, CCullingData);
			if (cullingData != NULL)
				cullingVisible = cullingData->Visible;

			// only render visible culling mesh
			if (cullingVisible == true)
				m_meshs.push_back(meshData);
		}
	}

	int cmpRenderSkinMeshFunc(const void* a, const void* b)
	{
		CRenderMeshData* pa = *((CRenderMeshData**)a);
		CRenderMeshData* pb = *((CRenderMeshData**)b);

		CMesh* meshA = pa->getMesh();
		CMesh* meshB = pb->getMesh();

		std::vector<CMaterial*>& materialsA = meshA->Materials;
		std::vector<CMaterial*>& materialsB = meshB->Materials;

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
			IMeshBuffer* mbA = meshA->getMeshBuffer(0);
			IMeshBuffer* mbB = meshB->getMeshBuffer(0);

			if (mbA == mbB)
				return 0;

			return mbA < mbB ? -1 : 1;
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
		{
			IMeshBuffer* mbA = meshA->getMeshBuffer(0);
			IMeshBuffer* mbB = meshB->getMeshBuffer(0);

			if (mbA == mbB)
				return 0;

			return mbA < mbB ? -1 : 1;
		}

		return textureA < textureB ? -1 : 1;
	}

	void CSkinnedMeshRenderer::update(CEntityManager* entityManager)
	{
		// need sort render by material, texture, mesh		
		u32 count = m_meshs.size();
		if (count > 0)
		{
			// need sort by material
			qsort(m_meshs.pointer(), count, sizeof(CRenderMeshData*), cmpRenderSkinMeshFunc);
		}
	}

	void CSkinnedMeshRenderer::render(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();

		CRenderMeshData** meshs = m_meshs.pointer();
		CShaderManager* shaderManager = CShaderManager::getInstance();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		for (u32 i = 0, n = m_meshs.size(); i < n; i++)
		{
			CRenderMeshData* renderMeshData = m_meshs[i];
			CEntity* entity = renderMeshData->Entity;

			CIndirectLightingData* lightingData = GET_ENTITY_DATA(entity, CIndirectLightingData);
			if (lightingData != NULL)
			{
				if (lightingData->Type == CIndirectLightingData::SH9)
					CShaderSH::setSH9(lightingData->SH);
				else if (lightingData->Type == CIndirectLightingData::AmbientColor)
					CShaderLighting::setLightAmbient(lightingData->Color);
			}

			// set bone matrix to shader callback
			CSkinnedMesh* mesh = (CSkinnedMesh*)renderMeshData->getMesh();
			shaderManager->BoneMatrix = mesh->SkinningMatrix;
			shaderManager->BoneCount = mesh->Joints.size();

			// software blendshape
			if (renderMeshData->isSoftwareBlendShape())
				mesh = (CSkinnedMesh*)renderMeshData->getSoftwareBlendShapeMesh();

			// set transform
			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			driver->setTransform(video::ETS_WORLD, transform->World);

			bool haveTransparent = false;

			// render mesh
			for (u32 j = 0, m = mesh->getMeshBufferCount(); j < m; j++)
			{
				CMaterial* material = mesh->Materials[j];
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
		CShaderManager* shaderManager = CShaderManager::getInstance();
		IRenderPipeline* rp = entityManager->getRenderPipeline();
		CRenderMeshData** meshs = m_meshs.pointer();
		CEntity** allEntities = entityManager->getEntities();

		for (u32 i = 0; i < numTransparent; i++)
		{
			u32 meshID = m_transparents[i];

			CRenderMeshData* renderMeshData = m_meshs[meshID];
			CEntity* entity = renderMeshData->Entity;

			// set bone matrix to shader callback
			CSkinnedMesh* mesh = (CSkinnedMesh*)renderMeshData->getMesh();
			shaderManager->BoneMatrix = mesh->SkinningMatrix;
			shaderManager->BoneCount = mesh->Joints.size();

			// set transform
			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			driver->setTransform(video::ETS_WORLD, transform->World);

			// software blendshape
			if (renderMeshData->isSoftwareBlendShape())
				mesh = (CSkinnedMesh*)renderMeshData->getSoftwareBlendShapeMesh();

			// render mesh
			for (u32 j = 0, m = mesh->getMeshBufferCount(); j < m; j++)
			{
				CMaterial* material = mesh->Materials[j];

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