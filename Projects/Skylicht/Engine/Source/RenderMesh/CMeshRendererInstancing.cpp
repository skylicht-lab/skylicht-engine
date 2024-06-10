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
#include "CMeshRendererInstancing.h"

#include "Culling/CCullingData.h"
#include "Entity/CEntityManager.h"

#include "Material/Shader/ShaderCallback/CShaderSH.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

namespace Skylicht
{
	CMeshRendererInstancing::CMeshRendererInstancing()
	{
		m_pipelineType = IRenderPipeline::Mix;
	}

	CMeshRendererInstancing::~CMeshRendererInstancing()
	{
		for (auto it : m_groups)
		{
			delete it.second;
		}
		m_groups.clear();
	}

	void CMeshRendererInstancing::beginQuery(CEntityManager* entityManager)
	{
		m_meshs.set_used(0);

		for (auto it : m_groups)
		{
			SMeshInstancingGroup* group = it.second;
			group->Materials.reset();
			group->Entities.reset();
		}

		CMeshRenderSystem::beginQuery(entityManager);
	}

	void CMeshRendererInstancing::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		numEntity = m_groupMesh->getNumInstancingMesh();
		entities = m_groupMesh->getInstancingMeshes();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CRenderMeshData* meshData = GET_ENTITY_DATA(entity, CRenderMeshData);

			SMeshInstancing* data = meshData->getMeshInstancing();
			if (data == NULL)
				continue;

			bool cullingVisible = true;

			// get culling result from CCullingSystem
			CCullingData* cullingData = GET_ENTITY_DATA(entity, CCullingData);
			if (cullingData != NULL)
				cullingVisible = cullingData->Visible;

			// only render visible culling mesh
			if (cullingVisible == true)
			{
				m_meshs.push_back(meshData);

				// reset share batch state
				if (data->ShareDataTransform)
					*data->ShareDataTransform = 0;

				if (data->ShareDataMaterials)
					*data->ShareDataMaterials = 0;
			}
		}
	}

	void CMeshRendererInstancing::init(CEntityManager* entityManager)
	{

	}

	void CMeshRendererInstancing::update(CEntityManager* entityManager)
	{
		u32 numEntity = m_meshs.size();
		CRenderMeshData** renderData = m_meshs.pointer();

		CEntity** allEntities = entityManager->getEntities();

		// update instancing
		for (u32 i = 0; i < numEntity; i++)
		{
			SMeshInstancing* data = renderData[i]->getMeshInstancing();

			SMeshInstancingGroup* group = data->InstancingGroup;
			if (group == NULL)
			{
				group = new SMeshInstancingGroup();
				group->RootEntityIndex = renderData[i]->EntityIndex;

				m_groups[data] = group;

				data->InstancingGroup = group;
			}

			group->Entities.push(renderData[i]->Entity);
		}

		// bake instancing in group
		for (auto it : m_groups)
		{
			SMeshInstancing* data = it.first;
			SMeshInstancingGroup* group = it.second;

			u32 count = group->Entities.count();
			if (count == 0)
				continue;

			for (u32 i = 0, n = data->RenderMeshBuffers.size(); i < n; i++)
			{
				group->Materials.reset();

				bool batchMaterial = true;
				if (data->UseShareMaterialsBuffer)
				{
					// see function CRenderMeshInstancing::applyShareMaterialBuffer
					if (*data->ShareDataMaterials == 0)
						*data->ShareDataMaterials = 1;
					else
						batchMaterial = false;
				}

				if (batchMaterial)
				{
					for (u32 j = 0; j < count; j++)
						group->Materials.push(data->Materials[i]);

					// batching transform & material data to buffer
					data->InstancingShader[i]->batchIntancing(
						data->MaterialBuffer[i],
						group->Materials.pointer(),
						group->Entities.pointer(),
						count
					);
				}
			}

			bool batchTransform = true;
			if (data->UseShareTransformBuffer)
			{
				// see function CRenderMeshInstancing::applyShareTransformBuffer
				// batch only 1 time in first group, it will reset 0 on onQuery
				if (*data->ShareDataTransform == 0)
					*data->ShareDataTransform = 1;
				else
					batchTransform = false;
			}

			if (batchTransform)
			{
				IShaderInstancing::batchTransformAndLighting(
					data->TransformBuffer,
					data->IndirectLightingBuffer,
					group->Entities.pointer(),
					count
				);
			}
		}
	}

	void CMeshRendererInstancing::render(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		for (auto& it : m_groups)
		{
			SMeshInstancing* data = it.first;
			SMeshInstancingGroup* group = it.second;

			int count = group->Entities.count();
			if (count == 0)
				continue;

			u32 numMeshBuffer = data->RenderMeshBuffers.size();
			for (u32 i = 0; i < numMeshBuffer; i++)
			{
				if (data->Materials[i] == NULL)
					continue;

				CShader* shader = data->Materials[i]->getShader();

				if (!rp->canRenderShader(shader))
					continue;

				CShaderMaterial::setMaterial(data->Materials[i]);

				rp->drawInstancingMeshBuffer(
					(CMesh*)data->InstancingMesh,
					i,
					shader->getInstancingShader(),
					entityManager,
					group->RootEntityIndex,
					false
				);
			}
		}
	}
}