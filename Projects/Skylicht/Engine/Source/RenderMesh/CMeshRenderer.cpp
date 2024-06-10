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
#include "CMeshRenderer.h"

#include "Culling/CCullingData.h"
#include "Entity/CEntityManager.h"

#include "Material/Shader/ShaderCallback/CShaderSH.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"

namespace Skylicht
{
	CMeshRenderer::CMeshRenderer()
	{
		m_pipelineType = IRenderPipeline::Mix;
	}

	CMeshRenderer::~CMeshRenderer()
	{

	}

	void CMeshRenderer::beginQuery(CEntityManager* entityManager)
	{
		m_meshs.set_used(0);

		CMeshRenderSystem::beginQuery(entityManager);
	}

	void CMeshRenderer::addToListMesh(CEntity** entities, int numEntity)
	{
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

	void CMeshRenderer::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		// do not render gpu skinning, pass for CSkinMeshRenderer
		// do not render instancing mesh, pass for CInstancingMeshRenderer
		entities = m_groupMesh->getStaticMeshes();
		numEntity = m_groupMesh->getNumStaticMesh();

		addToListMesh(entities, numEntity);

		// render software skinned mesh
		entities = m_groupMesh->getSoftwareSkinnedMeshes();
		numEntity = m_groupMesh->getNumSoftwareSkinnedMesh();

		addToListMesh(entities, numEntity);
	}

	void CMeshRenderer::init(CEntityManager* entityManager)
	{

	}

	int cmpRenderMeshFunc(const void* a, const void* b)
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

		// comprate by texture
		ITexture* textureA = materialA->getTexture(0);
		ITexture* textureB = materialB->getTexture(0);

		// if no texture
		if (textureA == NULL || textureB == NULL)
		{
			if (materialA == materialB)
			{
				// compare mesh
				IMeshBuffer* mbA = meshA->getMeshBuffer(0);
				IMeshBuffer* mbB = meshB->getMeshBuffer(0);

				if (mbA == mbB)
					return 0;
			}

			return materialA < materialB ? -1 : 1;
		}

		// sort by texture 0
		if (textureA == textureB)
		{
			// compare mesh
			IMeshBuffer* mbA = meshA->getMeshBuffer(0);
			IMeshBuffer* mbB = meshB->getMeshBuffer(0);

			if (mbA == mbB)
				return 0;

			return mbA < mbB ? -1 : 1;
		}

		return textureA < textureB ? -1 : 1;
	}

	void CMeshRenderer::update(CEntityManager* entityManager)
	{
		// need sort render by material, texture, mesh		
		u32 count = m_meshs.size();
		if (count > 0)
		{
			// need sort by material
			qsort(m_meshs.pointer(), count, sizeof(CRenderMeshData*), cmpRenderMeshFunc);
		}
	}

	void CMeshRenderer::render(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();
		IRenderPipeline* rp = entityManager->getRenderPipeline();
		CRenderMeshData** meshs = m_meshs.pointer();

		for (u32 i = 0, n = m_meshs.size(); i < n; i++)
		{
			CRenderMeshData* meshData = m_meshs[i];
			CEntity* entity = meshData->Entity;

			CMesh* mesh = meshData->getMesh();
			if (meshData->isSoftwareBlendShape())
				mesh = meshData->getSoftwareBlendShapeMesh();
			if (meshData->isSoftwareSkinning())
				mesh = meshData->getSoftwareSkinnedMesh();

			CIndirectLightingData* lightingData = GET_ENTITY_DATA(entity, CIndirectLightingData);
			if (lightingData != NULL)
			{
				if (lightingData->Type == CIndirectLightingData::SH9)
					CShaderSH::setSH9(lightingData->SH);
				else if (lightingData->Type == CIndirectLightingData::AmbientColor)
					CShaderLighting::setLightAmbient(lightingData->Color);
			}

			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			driver->setTransform(video::ETS_WORLD, transform->World);

			for (u32 j = 0, m = mesh->getMeshBufferCount(); j < m; j++)
				rp->drawMeshBuffer(mesh, j, entityManager, meshData->EntityIndex, false);
		}
	}
}