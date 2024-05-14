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
					if (data->ShareDataTransform)
						*data->ShareDataTransform = 0;

					if (data->ShareDataMaterials)
						*data->ShareDataMaterials = 0;

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
			u32 meshIndex = group->MeshIndex;

			CMaterial* m = NULL;

			for (u32 i = 0, n = data->RenderMeshBuffers.size(); i < n; i++)
			{
				group->Materials.reset();

				bool batchMaterial = true;
				if (data->UseShareMaterialsBuffer)
				{
					if (*data->ShareDataMaterials == 0)
						*data->ShareDataMaterials = 1;
					else
					{
						batchMaterial = false;

						// just get 1 material for group
						// no need full batch material
						count = 1;
					}
				}

				for (u32 j = 0; j < count; j++)
				{
					// update animation to material
					CSkinnedInstanceData* skinnedIntance = GET_ENTITY_DATA(entities[j], CSkinnedInstanceData);

					// clone the material for many instance
					// that will have bugs if the mesh have more than 1 material
					if (skinnedIntance->Materials.size() <= meshIndex ||
						skinnedIntance->Materials[meshIndex] == NULL)
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

					SSkeletonAnimation* skeletons = skinnedIntance->Skeletons;

					// set animation to shader params
					SVec4& p = m->getShaderParams().getParam(0);
					p.X = (float)(skeletons[0].Frame);
					p.Y = (float)(skeletons[0].ClipId);
					p.Z = (float)(skeletons[1].Frame);
					p.W = (float)(skeletons[1].ClipId);

					// set blending to shader params
					SVec4& b = m->getShaderParams().getParam(5);
					b.X = skeletons[1].Weight;

					group->Materials.push(m);
				}

				if (batchMaterial)
				{
					// see function CRenderMeshInstancing::applyShareMaterialBuffer
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
		CShaderManager* shaderMgr = CShaderManager::getInstance();

		for (auto& it : m_instancingGroups)
		{
			SMeshInstancing* data = it.first;
			SMeshInstancingGroup* group = it.second;

			int count = group->Entities.count();
			if (count == 0)
				continue;

			// apply bone count
			CMesh* mesh = group->RenderMesh->getMesh();
			CSkinnedMesh* skinnedMesh = dynamic_cast<CSkinnedMesh*>(mesh);
			shaderMgr->BoneCount = skinnedMesh->Joints.size();

			// apply bone transform texture
			CShaderTransformTexture::setTexture(group->TransformTexture);

			CEntity* rootEntity = allEntities[group->RootEntityIndex];
			CIndirectLightingData* indirectLighting = GET_ENTITY_DATA(rootEntity, CIndirectLightingData);
			if (indirectLighting != NULL && indirectLighting->SH != NULL)
			{
				// apply sh
				CShaderSH::setSH9(indirectLighting->SH);
			}

			int materialCount = group->Materials.count();
			CMaterial** materials = group->Materials.pointer();

			u32 numMeshBuffer = data->RenderMeshBuffers.size();
			for (u32 i = 0; i < numMeshBuffer; i++)
			{
				if (materialCount == 0 || materials[0] == NULL)
					continue;

				CShader* shader = materials[0]->getShader();

				if (shader->isOpaque() == isRenderTransparent)
				{
					// check this state render transparent is not
					continue;
				}

				if (!rp->canRenderShader(shader))
					continue;

				// apply material
				CShaderMaterial::setMaterial(materials[0]);

				rp->drawInstancingMeshBuffer(
					(CMesh*)data->InstancingMesh,
					i,
					shader,
					entityManager,
					group->RootEntityIndex,
					true
				);
			}
		}
	}
}