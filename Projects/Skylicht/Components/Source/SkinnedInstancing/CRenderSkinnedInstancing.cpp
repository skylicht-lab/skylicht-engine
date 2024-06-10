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

#include "CRenderSkinnedInstancing.h"
#include "CTransformTextureData.h"
#include "CSkinnedInstanceData.h"

#include "MeshManager/CMeshManager.h"
#include "Entity/CEntityManager.h"
#include "Culling/CCullingData.h"
#include "TextureManager/CTextureManager.h"

#include "Transform/CWorldInverseTransformData.h"
#include "Culling/CCullingBBoxData.h"
#include "Culling/CVisibleData.h"

#include "Material/Shader/Instancing/IShaderInstancing.h"

#include "CSkinnedInstanceAnimationSystem.h"
#include "CSkinnedMeshRendererInstancing.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CRenderSkinnedInstancing);

	CATEGORY_COMPONENT(CRenderSkinnedInstancing, "Skinned Mesh", "Renderer/Instancing");

	CRenderSkinnedInstancing::CRenderSkinnedInstancing()
	{

	}

	CRenderSkinnedInstancing::~CRenderSkinnedInstancing()
	{

	}

	void CRenderSkinnedInstancing::initComponent()
	{
		CEntityManager* entityMgr = m_gameObject->getEntityManager();
		entityMgr->addSystem<CSkinnedInstanceAnimationSystem>();
		entityMgr->addRenderSystem<CSkinnedMeshRendererInstancing>();

		CRenderMeshInstancing::initComponent();
	}

	void CRenderSkinnedInstancing::releaseEntities()
	{
		m_textures.clear();
		CRenderMeshInstancing::releaseEntities();
	}

	void CRenderSkinnedInstancing::initFromPrefab(CEntityPrefab* prefab)
	{
		// clear old entity
		releaseEntities();

		CEntityManager* entityManager = m_gameObject->getEntityManager();

		// root entity of object
		m_root = m_gameObject->getEntity();
		CWorldTransformData* rootTransform = GET_ENTITY_DATA(m_root, CWorldTransformData);

		// spawn childs entity
		int numEntities = prefab->getNumEntities();

		core::array<CEntity*> renderEntities;

		// we just add the renderer prefab
		for (int i = 0; i < numEntities; i++)
		{
			CEntity* srcEntity = prefab->getEntity(i);
			CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
			if (srcRender != NULL)
			{
				if (srcRender->isSkinnedMesh())
				{
					// just add skinned mesh
					renderEntities.push_back(srcEntity);
				}
			}
		}

		numEntities = renderEntities.size();

		CEntity** entities = entityManager->createEntity(numEntities, m_baseEntities);

		// copy entity data
		for (int i = 0; i < numEntities; i++)
		{
			CEntity* spawnEntity = entities[i];
			CEntity* srcEntity = renderEntities[i];

			// copy transform data
			CWorldTransformData* srcTransform = srcEntity->getData<CWorldTransformData>();
			if (srcTransform != NULL)
			{
				CWorldTransformData* spawnTransform = spawnEntity->addData<CWorldTransformData>();
				spawnTransform->Name = srcTransform->Name;

				// get the world matrix
				core::matrix4 m = srcTransform->Relative;
				int parentID = srcTransform->ParentIndex;
				while (parentID != -1)
				{
					CEntity* parent = prefab->getEntity(parentID);
					CWorldTransformData* parentTransform = parent->getData<CWorldTransformData>();

					m = parentTransform->Relative * m;
					parentID = parentTransform->ParentIndex;
				}

				spawnTransform->Relative = m;
				spawnTransform->HasChanged = true;
				spawnTransform->Depth = rootTransform->Depth + 1;
				spawnTransform->ParentIndex = m_root->getIndex();

				m_transforms.push_back(spawnTransform);
			}

			// copy render data
			CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
			if (srcRender != NULL)
			{
				CWorldTransformData* transform = GET_ENTITY_DATA(spawnEntity, CWorldTransformData);

				CRenderMeshData* spawnRender = spawnEntity->addData<CRenderMeshData>();
				spawnRender->setMesh(srcRender->getMesh());
				spawnRender->setSkinnedInstancing(true);
				spawnRender->setVisible(false);

				// add to list renderer
				m_renderers.push_back(spawnRender);

				// also add transform
				m_renderTransforms.push_back(transform);

				// also check add indirect lighting to hold ReflectionTexture for intacing
				CIndirectLightingData* indirectLighting = spawnEntity->addData<CIndirectLightingData>();
				indirectLighting->initSH();
			}
		}
	}

	void CRenderSkinnedInstancing::initTextureTransform(core::matrix4* transforms, u32 w, u32 h, std::map<std::string, int>& bones)
	{
		// w: num frames
		// h: num bones (all)
		CTextureManager* textureMgr = CTextureManager::getInstance();
		CEntityManager* entityMgr = m_gameObject->getEntityManager();

		m_textures.clear();

		int clipJointCount = (int)bones.size();
		int numClip = h / clipJointCount;

		// fix: OpenGL texture size
		u32 tw = 0;
		while (tw < w)
		{
			tw = tw + 64;
		}

		for (CRenderMeshData* renderer : m_renderers)
		{
			// get skinned mesh & number of bones
			CSkinnedMesh* skinnedMesh = dynamic_cast<CSkinnedMesh*>(renderer->getMesh());
			u32 jointCount = skinnedMesh->Joints.size();

			core::matrix4* boneMatrix = new core::matrix4[tw * jointCount * numClip];

			for (int clipId = 0; clipId < numClip; clipId++)
			{
				for (u32 i = 0, n = jointCount; i < n; i++)
				{
					CSkinnedMesh::SJoint& joint = skinnedMesh->Joints[i];

					// convert to transform index
					u32 id = bones[joint.Name];

					if (id >= h)
					{
						char log[1024];
						sprintf(log, "[CRenderSkinnedInstancing] initTextureTransform %s out of range: %d", joint.Name.c_str(), id);
						os::Printer::log(log);
					}
					else
					{
						int clipOffset1 = clipId * jointCount * tw;
						int clipOffset2 = clipId * clipJointCount * w;

						for (u32 j = 0; j < w; j++)
						{
							// calc skinned matrix, that like CSkinnedMeshSystem::update
							boneMatrix[clipOffset1 + i * tw + j].setbyproduct_nocheck(transforms[clipOffset2 + id * w + j], joint.BindPoseMatrix);
						}
					}
				}
			}

			CEntity* entity = renderer->Entity;

			// save the texture
			CTransformTextureData* data = entity->addData<CTransformTextureData>();
			data->TransformTexture = textureMgr->createTransformTexture2D("BoneTransformTexture", boneMatrix, tw, jointCount * numClip);
			data->JointCount = jointCount;

			// add textures
			m_textures.push_back(data);

			delete[]boneMatrix;
		}
	}

	CEntity* CRenderSkinnedInstancing::spawn()
	{
		CEntity* entity = createEntity();

		CIndirectLightingData* indirect = entity->addData<CIndirectLightingData>();
		indirect->initSH();

		// entity->addData<CWorldInverseTransformData>();
		entity->addData<CCullingData>();

		CCullingBBoxData* cullingBBox = entity->addData<CCullingBBoxData>();

		CSkinnedInstanceData* skinnedInstance = entity->addData<CSkinnedInstanceData>();

		CEntityManager* entityMgr = m_gameObject->getEntityManager();

		bool firstBox = true;
		for (CRenderMeshData*& renderer : m_renderers)
		{
			// set bbox
			CMesh* mesh = renderer->getMesh();
			if (firstBox)
			{
				firstBox = false;
				cullingBBox->BBox = mesh->getBoundingBox();
			}
			else
			{
				cullingBBox->BBox.addInternalBox(mesh->getBoundingBox());
			}

			// add renderer
			skinnedInstance->RenderData.push_back(renderer);
		}

		// add textures
		for (CTransformTextureData*& t : m_textures)
		{
			skinnedInstance->TransformTextures.push_back(t->TransformTexture);
		}

		return entity;
	}

	bool CRenderSkinnedInstancing::setAnimation(CEntity* entity, int animTextureIndex, CAnimationClip* clipInfo, float currentTime, int bakeFps, int skeletonId, bool loop, bool pause)
	{
		CSkinnedInstanceData* skinnedData = GET_ENTITY_DATA(entity, CSkinnedInstanceData);
		if (skinnedData == NULL)
			return false;

		skinnedData->setAnimation(animTextureIndex, clipInfo, currentTime, bakeFps, skeletonId, loop, pause);
		return true;
	}

	bool CRenderSkinnedInstancing::setAnimation(CEntity* entity, int animTextureIndex, CAnimationClip* clipInfo, float clipBegin, float clipDuration, float currentTime, int bakeFps, int skeletonId, bool loop, bool pause)
	{
		CSkinnedInstanceData* skinnedData = GET_ENTITY_DATA(entity, CSkinnedInstanceData);
		if (skinnedData == NULL)
			return false;

		skinnedData->setAnimation(animTextureIndex, clipInfo, clipBegin, clipDuration, currentTime, bakeFps, skeletonId, loop, pause);
		return true;
	}

	void CRenderSkinnedInstancing::setAnimationWeight(CEntity* entity, int skeletonId, float weight)
	{
		CSkinnedInstanceData* skinnedData = GET_ENTITY_DATA(entity, CSkinnedInstanceData);
		if (skinnedData == NULL)
			return;

		skinnedData->setAnimationWeight(skeletonId, weight);
	}
}