/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

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
#include "CSkinnedMeshRendererInstancing.h"
#include "SkinnedInstancing/CSkinnedInstanceData.h"

#include "Culling/CVisibleData.h"
#include "Culling/CCullingData.h"
#include "Entity/CEntityManager.h"

#include "MeshManager/CMeshManager.h"

#include "Material/Shader/ShaderCallback/CShaderSH.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"
#include "Material/Shader/ShaderCallback/CShaderTransformTexture.h"

namespace Skylicht
{
	CSkinnedMeshRendererInstancing::CSkinnedMeshRendererInstancing() :
		m_group(NULL)
	{
		m_pipelineType = IRenderPipeline::Mix;
	}

	CSkinnedMeshRendererInstancing::~CSkinnedMeshRendererInstancing()
	{
		for (auto it : m_instancingGroups)
		{
			it.first->InstancingGroup = NULL;
			delete it.second;
		}
		m_instancingGroups.clear();
	}

	void CSkinnedMeshRendererInstancing::beginQuery(CEntityManager* entityManager)
	{
		if (m_group == NULL)
		{
			const u32 type[] = GET_LIST_ENTITY_DATA(CSkinnedInstanceData);
			m_group = (CGroupSkinnedInstancing*)entityManager->findGroup(type, 1);

			if (m_group == NULL)
			{
				const u32 visibleGroupType[] = GET_LIST_ENTITY_DATA(CVisibleData);
				CEntityGroup* visibleGroup = entityManager->findGroup(visibleGroupType, 1);

				m_group = (CGroupSkinnedInstancing*)entityManager->addCustomGroup(new CGroupSkinnedInstancing(visibleGroup));
			}
		}

		for (auto it : m_instancingGroups)
		{
			SMeshInstancingGroup* group = it.second;
			group->Materials.reset();
			group->Entities.reset();
		}
	}

	void CSkinnedMeshRendererInstancing::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		entities = m_group->getEntities();
		numEntity = m_group->getEntityCount();

		CMeshManager* meshManager = CMeshManager::getInstance();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			// get culling result from CCullingSystem
			CCullingData* cullingData = GET_ENTITY_DATA(entity, CCullingData);
			if (cullingData != NULL)
			{
				if (!cullingData->Visible)
					continue;
			}

			// skip no transform textures
			CSkinnedInstanceData* skinnedIntance = GET_ENTITY_DATA(entity, CSkinnedInstanceData);
			if (skinnedIntance->TransformTextures.size() == 0)
				continue;

			for (u32 j = 0, n = skinnedIntance->RenderData.size(); j < n; j++)
			{
				CRenderMeshData* meshData = skinnedIntance->RenderData[j];

				if (skinnedIntance->TransformTextures[j] == NULL)
					continue;

				// skip if the shader is incompatible 
				CMesh* mesh = meshData->getMesh();
				if (mesh->Materials[0] == NULL)
					continue;

				CShader* shader = mesh->Materials[0]->getShader();
				if (!shader || !rp->canRenderShader(shader))
					continue;

				SMeshInstancing* data = meshData->getMeshInstancing();
				if (data)
				{
					// reset share batch state
					if (data->ShareData)
						*data->ShareData = 0;

					SMeshInstancingGroup* group = data->InstancingGroup;
					if (group == NULL)
					{
						group = new SMeshInstancingGroup();
						group->TransformTexture = skinnedIntance->TransformTextures[j];
						group->RenderMesh = meshData;
						group->RootEntityIndex = meshData->EntityIndex;
						group->MeshIndex = j;

						data->InstancingGroup = group;
						m_instancingGroups[data] = group;
					}

					group->Entities.push(entity);
				}
			}
		}
	}

	void CSkinnedMeshRendererInstancing::init(CEntityManager* entityManager)
	{

	}

	void CSkinnedMeshRendererInstancing::update(CEntityManager* entityManager)
	{
		for (auto it : m_instancingGroups)
		{
			SMeshInstancing* data = it.first;
			SMeshInstancingGroup* group = it.second;

			u32 count = group->Entities.count();
			if (count == 0)
				continue;

			CEntity** entities = group->Entities.pointer();

			CMesh* mesh = group->RenderMesh->getMesh();
			CMaterial* m = NULL;

			CSkinnedMesh* skinnedMesh = dynamic_cast<CSkinnedMesh*>(mesh);
			u32 jointCount = skinnedMesh->Joints.size();
			u32 meshIndex = group->MeshIndex;

			for (u32 i = 0, n = data->RenderMeshBuffers.size(); i < n; i++)
			{
				group->Materials.reset();

				for (u32 j = 0; j < count; j++)
				{
					// update animation to material
					CSkinnedInstanceData* skinnedIntance = GET_ENTITY_DATA(entities[j], CSkinnedInstanceData);

					// clone the material for many instance
					// that will have bugs if the mesh have more than 1 material
					if (meshIndex >= (int)skinnedIntance->Materials.size() || skinnedIntance->Materials[meshIndex] == NULL)
					{
						m = mesh->Materials[i]->clone();

						// init null materials array
						for (u32 t = skinnedIntance->Materials.size(); t <= meshIndex; t++)
							skinnedIntance->Materials.push_back(NULL);

						// save this material and destroy it in destructor of CSkinnedInstanceData
						skinnedIntance->Materials[meshIndex] = m;
					}
					else
					{
						m = skinnedIntance->Materials[meshIndex];
					}

					// set animation to shader params
					SVec4& p = m->getShaderParams().getParam(0);
					p.X = (float)(skinnedIntance->Frame);
					p.Y = (float)(skinnedIntance->ClipId * jointCount);

					group->Materials.push(m);
				}

				// batching transform & material data to buffer
				data->InstancingShader[i]->batchIntancing(
					data->InstancingBuffer[i],
					group->Materials.pointer(),
					group->Entities.pointer(),
					count
				);
			}

			bool batchTransform = true;
			if (data->UseShareVertexBuffer)
			{
				if (*data->ShareData == 0)
				{
					// batch only 1 time in first group, it will reset 0 on onQuery
					*data->ShareData = 1;
				}
				else
				{
					// optimized, that mean we use the share vb that batched
					batchTransform = false;
				}
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

	void CSkinnedMeshRendererInstancing::render(CEntityManager* entityManager)
	{
		renderInstancing(entityManager, false);
	}

	void CSkinnedMeshRendererInstancing::renderTransparent(CEntityManager* entityManager)
	{
		renderInstancing(entityManager, true);
	}

	void CSkinnedMeshRendererInstancing::renderInstancing(CEntityManager* entityManager, bool isRenderTransparent)
	{
		IVideoDriver* driver = getVideoDriver();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		CEntity** allEntities = entityManager->getEntities();

		for (auto& it : m_instancingGroups)
		{
			SMeshInstancing* data = it.first;
			SMeshInstancingGroup* group = it.second;

			int count = group->Entities.count();
			if (count == 0)
				continue;

			CMaterial** materials = group->Materials.pointer();

			CShaderTransformTexture::setTexture(group->TransformTexture);

			CEntity* rootEntity = allEntities[group->RootEntityIndex];
			CIndirectLightingData* indirectLighting = GET_ENTITY_DATA(rootEntity, CIndirectLightingData);
			if (indirectLighting != NULL && indirectLighting->SH != NULL)
			{
				// apply sh
				CShaderSH::setSH9(indirectLighting->SH);
			}

			u32 numMeshBuffer = data->RenderMeshBuffers.size();
			for (u32 i = 0; i < numMeshBuffer; i++)
			{
				if (materials[i] == NULL)
					continue;

				CShader* shader = materials[i]->getShader();

				if (shader->isOpaque() == isRenderTransparent)
				{
					// check this state render transparent is not
					continue;
				}

				if (!rp->canRenderShader(shader))
					continue;

				// apply material
				CShaderMaterial::setMaterial(materials[i]);

				int materialType = shader->getMaterialRenderID();

				rp->drawInstancingMeshBuffer(
					(CMesh*)data->InstancingMesh,
					i,
					materialType,
					entityManager,
					group->RootEntityIndex,
					true
				);
			}
		}
	}
}