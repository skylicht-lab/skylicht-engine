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
			it.first->InstancingGroup = NULL;
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

		CEntity* entity;
		CRenderMeshData* meshData;
		SMeshInstancing* data;
		bool cullingVisible;
		CCullingData* cullingData;

		for (int i = 0; i < numEntity; i++)
		{
			entity = entities[i];

			meshData = GET_ENTITY_DATA(entity, CRenderMeshData);
			data = meshData->getMeshInstancing();
			if (data == NULL)
				continue;

			cullingVisible = true;

			// get culling result from CCullingSystem
			cullingData = GET_ENTITY_DATA(entity, CCullingData);
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

			CEntity** entities = group->Entities.pointer();

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
					{
						CInstancingMaterialData* m = GET_ENTITY_DATA(entities[j], CInstancingMaterialData);
						if (m && m->Enable)
							group->Materials.push(m->Materials[i]);
						else
							group->Materials.push(data->Materials[i]);
					}

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

		core::array<SInstancingGroup> instancings;
		sortBeforeRender(instancings);

		m_transparents.set_used(0);

		for (u32 i = 0, n = instancings.size(); i < n; i++)
		{
			SMeshInstancing* data = instancings[i].Instancing;
			SMeshInstancingGroup* group = instancings[i].Group;

			int count = group->Entities.count();
			if (count == 0)
				continue;

			bool haveTransparent = false;
			u32 numMeshBuffer = data->RenderMeshBuffers.size();
			for (u32 i = 0; i < numMeshBuffer; i++)
			{
				if (data->Materials[i] == NULL)
					continue;

				CShader* shader = data->Materials[i]->getShader();

				if (!rp->canRenderShader(shader))
					continue;

				if (shader->isOpaque())
				{
					CShaderMaterial::setMaterial(data->Materials[i]);

					rp->drawInstancingMeshBuffer(
						(CMesh*)data->InstancingMesh,
						i,
						shader->getInstancingShader(data->BaseVertexType),
						entityManager,
						group->RootEntityIndex,
						false
					);
				}
				else
				{
					haveTransparent = true;
				}
			}

			if (haveTransparent)
				m_transparents.push_back(instancings[i]);
		}
	}

	void CMeshRendererInstancing::renderTransparent(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		for (u32 i = 0, n = m_transparents.size(); i < n; i++)
		{
			SMeshInstancing* data = m_transparents[i].Instancing;
			SMeshInstancingGroup* group = m_transparents[i].Group;

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

				if (!shader->isOpaque())
				{
					CShaderMaterial::setMaterial(data->Materials[i]);

					rp->drawInstancingMeshBuffer(
						(CMesh*)data->InstancingMesh,
						i,
						shader->getInstancingShader(data->BaseVertexType),
						entityManager,
						group->RootEntityIndex,
						false
					);
				}
			}
		}
	}

	int cmpMaterialsFunc(SMeshInstancing* meshA, SMeshInstancing* meshB)
	{
		core::array<CMaterial*>& materialsA = meshA->Materials;
		core::array<CMaterial*>& materialsB = meshB->Materials;

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
			IMeshBuffer* mbA = meshA->MeshBuffers[0];
			IMeshBuffer* mbB = meshB->MeshBuffers[0];

			if (mbA == mbB)
				return 0;

			return mbA < mbB ? -1 : 1;
		}

		// comprate by texture
		ITexture* textureA = materialA->getTexture(0);
		ITexture* textureB = materialB->getTexture(0);

		if (textureA == NULL || textureB == NULL || textureA == textureB)
		{
			CShader* shaderA = materialA->getShader();
			CShader* shaderB = materialB->getShader();

			if (shaderA == shaderB || materialA == materialB)
			{
				// compare mesh
				IMeshBuffer* mbA = meshA->MeshBuffers[0];
				IMeshBuffer* mbB = meshB->MeshBuffers[0];

				if (mbA == mbB)
					return 0;

				return mbA < mbB ? -1 : 1;
			}

			// compare by shader
			return shaderA < shaderB ? -1 : 1;
		}

		return textureA < textureB ? -1 : 1;
	}

	void CMeshRendererInstancing::sortBeforeRender(core::array<SInstancingGroup>& instancing)
	{
		instancing.set_used(0);

		for (auto& it : m_groups)
		{
			SMeshInstancing* data = it.first;
			SMeshInstancingGroup* group = it.second;

			int count = group->Entities.count();
			if (count == 0)
				continue;

			// insert sort
			bool insert = false;

			for (u32 i = 0, n = instancing.size(); i < n; i++)
			{
				if (cmpMaterialsFunc(instancing[i].Instancing, data) > 0)
				{
					SInstancingGroup g{ data, group };
					instancing.insert(g, i);
					insert = true;
					break;
				}
			}

			if (!insert)
			{
				SInstancingGroup g{ data, group };
				instancing.push_back(g);
			}
		}
	}
}