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

#include "CRenderMeshInstancingVAT.h"
#include "CVertexAnimTextureData.h"

#include "VertexAnimation/CSoftwareSkinningUtils.h"
#include "Entity/CEntityManager.h"
#include "Culling/CCullingData.h"
#include "Culling/CCullingBBoxData.h"
#include "Transform/CWorldInverseTransformData.h"
#include "SkinnedInstancing/CSkinnedInstanceData.h"
#include "SkinnedInstancing/CRenderSkinnedInstancing.h"
#include "SkinnedInstancing/CSkinnedInstanceAnimationSystem.h"
#include "SkinnedInstancing/CSkinnedMeshRendererInstancing.h"

#include "RenderMesh/CJointAnimationSystem.h"
#include "RenderMesh/CSkinnedMeshSystem.h"


namespace Skylicht
{
	ACTIVATOR_REGISTER(CRenderMeshInstancingVAT);

	CATEGORY_COMPONENT(CRenderMeshInstancingVAT, "Skinned Mesh VAT", "Renderer/Instancing");

	CRenderMeshInstancingVAT::CRenderMeshInstancingVAT()
	{
		for (int i = 0; i < 10; i++)
			m_clipOffset[i] = 0;
	}

	CRenderMeshInstancingVAT::~CRenderMeshInstancingVAT()
	{

	}

	void CRenderMeshInstancingVAT::initComponent()
	{
		CEntityManager* entityMgr = m_gameObject->getEntityManager();
		entityMgr->addSystem<CSkinnedInstanceAnimationSystem>();
		entityMgr->addRenderSystem<CSkinnedMeshRendererInstancing>();

		CRenderMeshInstancing::initComponent();
	}

	void CRenderMeshInstancingVAT::initFromPrefab(CEntityPrefab* prefab)
	{
		// clear old entity
		releaseEntities();

		CEntityManager* entityManager = m_gameObject->getEntityManager();

		// root entity of object
		m_root = m_gameObject->getEntity();
		CWorldTransformData* rootTransform = GET_ENTITY_DATA(m_root, CWorldTransformData);

		// spawn childs entity
		int numEntities = prefab->getNumEntities();
		CEntity** entities = entityManager->createEntity(numEntities, m_baseEntities);

		// map new entity index from src prefab
		std::map<int, int> entityIndex;

		// copy entity data
		for (int i = 0; i < numEntities; i++)
		{
			CEntity* spawnEntity = entities[i];
			CEntity* srcEntity = prefab->getEntity(i);

			// map index
			entityIndex[srcEntity->getIndex()] = spawnEntity->getIndex();

			// copy transform data
			CWorldTransformData* srcTransform = srcEntity->getData<CWorldTransformData>();
			if (srcTransform != NULL)
			{
				CWorldTransformData* spawnTransform = spawnEntity->addData<CWorldTransformData>();
				spawnTransform->Name = srcTransform->Name;
				spawnTransform->Relative = srcTransform->Relative;
				spawnTransform->HasChanged = true;
				spawnTransform->Depth = rootTransform->Depth + 1 + srcTransform->Depth;

				if (srcTransform->ParentIndex == -1 || entityIndex.find(srcTransform->ParentIndex) == entityIndex.end())
					spawnTransform->ParentIndex = m_root->getIndex();
				else
					spawnTransform->ParentIndex = entityIndex[srcTransform->ParentIndex];

				m_transforms.push_back(spawnTransform);
			}

			// copy render data
			CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
			if (srcRender != NULL &&
				srcRender->getMesh() &&
				srcRender->isSkinnedMesh())
			{
				CRenderMeshData* spawnRender = spawnEntity->addData<CRenderMeshData>();

				// set vertex index
				CMesh* mesh = srcRender->getMesh();
				CSoftwareSkinningUtils::resetVertexID(mesh);

				// the mesh, that will render instancing to screen
				spawnRender->setMesh(mesh);
				spawnRender->setSkinnedMesh(true);
				spawnRender->setSkinnedInstancing(true);
				spawnRender->setVisible(false);

				// add to list renderer
				m_renderers.push_back(spawnRender);

				// also add transform
				m_renderTransforms.push_back(GET_ENTITY_DATA(spawnEntity, CWorldTransformData));

				// also check add indirect lighting to hold ReflectionTexture for intacing
				CIndirectLightingData* indirectLighting = spawnEntity->addData<CIndirectLightingData>();
				indirectLighting->initSH();

				// add world inv transform for culling system (disable to optimize)
				// spawnEntity->addData<CWorldInverseTransformData>();

				spawnEntity->addData<CVertexAnimTextureData>();
			}

			// copy culling data
			CCullingData* srcCulling = srcEntity->getData<CCullingData>();
			if (srcCulling != NULL)
			{
				CCullingData* spawnCulling = spawnEntity->addData<CCullingData>();
				spawnCulling->Type = srcCulling->Type;
				spawnCulling->Visible = srcCulling->Visible;
			}

			// copy joint data
			CJointData* srcJointData = (CJointData*)srcEntity->getData<CJointData>();
			if (srcJointData != NULL)
			{
				CJointData* spawnJoint = spawnEntity->addData<CJointData>();
				spawnJoint->BoneRoot = srcJointData->BoneRoot;
				spawnJoint->SID = srcJointData->SID;
				spawnJoint->BoneName = srcJointData->BoneName;
				spawnJoint->AnimationMatrix = srcJointData->AnimationMatrix;
				spawnJoint->RootIndex = m_root->getIndex();
			}
		}

		bool addInvData = false;
		int boneId = 0;

		// re-map joint with new entity in CEntityManager
		for (CRenderMeshData*& r : m_renderers)
		{
			if (r->isSkinnedMesh() == true)
			{
				CSkinnedMesh* skinMesh = dynamic_cast<CSkinnedMesh*>(r->getMesh());
				if (skinMesh != NULL)
				{
					u32 numJoints = (u32)skinMesh->Joints.size();

					// alloc animation matrix (Max: 64 bone)
					skinMesh->SkinningMatrix = new f32[16 * GPU_BONES_COUNT];

					for (u32 i = 0; i < numJoints; i++)
					{
						// map entity data to joint
						CSkinnedMesh::SJoint& joint = skinMesh->Joints[i];
						joint.EntityIndex = entityIndex[joint.EntityIndex];
						joint.JointData = GET_ENTITY_DATA(entityManager->getEntity(joint.EntityIndex), CJointData);

						// setup bone index for Texture Transform animations
						if (joint.JointData->BoneID == -1)
							joint.JointData->BoneID = boneId++;

						// pointer to skin mesh animation matrix
						joint.SkinningMatrix = skinMesh->SkinningMatrix + i * 16;
					}
				}

				if (addInvData == false)
				{
					if (GET_ENTITY_DATA(m_root, CWorldInverseTransformData) == NULL)
						m_root->addData<CWorldInverseTransformData>();
					addInvData = true;
				}
			}
		}
	}

	void CRenderMeshInstancingVAT::allocFrames(u32 numFrames)
	{
		CEntityManager* entityManager = m_gameObject->getEntityManager();
		CEntity** entities = entityManager->getEntities();

		for (CRenderMeshData*& renderer : m_renderers)
		{
			// get vertex count
			IMeshBuffer* mb = renderer->getMesh()->getMeshBuffer(0);
			u32 vtxCount = mb->getVertexBuffer()->getVertexCount();

			// alloc frames data
			CVertexAnimTextureData* vertexAnimData = GET_ENTITY_DATA(renderer->Entity, CVertexAnimTextureData);
			vertexAnimData->allocFrames(vtxCount, numFrames);
		}
	}

	void CRenderMeshInstancingVAT::bakeSkinnedMesh(u32 frame)
	{
		CEntityManager* entityManager = m_gameObject->getEntityManager();

		// root entity of object
		CEntity** entities = m_baseEntities.pointer();
		u32 numEntity = m_baseEntities.size();

		core::array<CEntity*> joints;
		core::array<CEntity*> renderers;

		for (u32 i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];
			if (GET_ENTITY_DATA(entity, CJointData))
				joints.push_back(entity);
			if (GET_ENTITY_DATA(entity, CRenderMeshData))
				renderers.push_back(entity);
		}

		// step 1
		// like CJointAnimationSystem
		CJointAnimationSystem::updateAnimationMatrix(entityManager, joints.pointer(), (int)joints.size());

		// step 2
		// like CSkinnedMeshSystem
		CSkinnedMeshSystem::updateSkinnedMesh(entityManager, renderers.pointer(), (int)renderers.size());

		// all entities
		entities = entityManager->getEntities();

		// step 3
		// like CSoftwareSkinningSystem
		for (CRenderMeshData* renderer : m_renderers)
		{
			// use software skinning
			CSkinnedMesh* renderMesh = dynamic_cast<CSkinnedMesh*>(renderer->getMesh());
			CMesh* skinnedMesh = renderer->getSoftwareSkinnedMesh();

			if (renderMesh->getMeshBuffer(0)->getVertexType() == video::EVT_SKIN_TANGENTS)
				CSoftwareSkinningUtils::softwareSkinningTangent(skinnedMesh, renderMesh, NULL);
			else
				CSoftwareSkinningUtils::softwareSkinning(skinnedMesh, renderMesh, NULL);

			// get vertex animation data and bake the vertex infomation
			CVertexAnimTextureData* vertexAnimData = GET_ENTITY_DATA(renderer->Entity, CVertexAnimTextureData);
			vertexAnimData->addFrame(frame, skinnedMesh);
		}
	}

	void CRenderMeshInstancingVAT::beginBake()
	{
		for (int i = 0; i < 10; i++)
			m_clipOffset[i] = 0;

		for (CRenderMeshData* renderer : m_renderers)
		{
			// the mesh, that will bake to VAT
			renderer->setSoftwareSkinning(true);
			renderer->initSoftwareSkinning();
		}
	}

	void CRenderMeshInstancingVAT::setClipFrameOffset(u32 id, u32 frames)
	{
		m_clipOffset[id] = frames;
	}

	void CRenderMeshInstancingVAT::endBake()
	{
		CEntityManager* entityManager = m_gameObject->getEntityManager();

		for (CRenderMeshData*& renderer : m_renderers)
		{
			CVertexAnimTextureData* vertexAnimData = GET_ENTITY_DATA(renderer->Entity, CVertexAnimTextureData);
			vertexAnimData->buildTexture();

			renderer->releaseSoftwareSkinning();
		}
	}

	CEntity* CRenderMeshInstancingVAT::spawn()
	{
		CEntityManager* entityManager = m_gameObject->getEntityManager();

		for (CRenderMeshData*& renderer : m_renderers)
		{
			CVertexAnimTextureData* vertexAnimData = GET_ENTITY_DATA(renderer->Entity, CVertexAnimTextureData);
			if (vertexAnimData->PositionTexture == NULL)
			{
				char log[512];
				sprintf(log, "[CRenderMeshInstancingVAT] spawn fail, you forgot endBake()");
				os::Printer::log(log);
				return NULL;
			}
		}

		CEntity* entity = createEntity();

		CIndirectLightingData* indirect = entity->addData<CIndirectLightingData>();
		indirect->initSH();

		CSkinnedInstanceData* skinnedInstance = entity->addData<CSkinnedInstanceData>();
		skinnedInstance->IsVertexAnimationTexture = true;
		skinnedInstance->ClipOffset = m_clipOffset;

		// entity->addData<CWorldInverseTransformData>();
		entity->addData<CCullingData>();

		CCullingBBoxData* cullingBBox = entity->addData<CCullingBBoxData>();

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

			// add vertex texture
			CVertexAnimTextureData* vertexAnimData = GET_ENTITY_DATA(renderer->Entity, CVertexAnimTextureData);
			skinnedInstance->PositionTextures.push_back(vertexAnimData->PositionTexture);
			skinnedInstance->NormalTextures.push_back(vertexAnimData->NormalTexture);
		}

		return entity;
	}

	bool CRenderMeshInstancingVAT::setAnimation(CEntity* entity, int clipId, CAnimationClip* clipInfo, float currentTime, int bakeFps, int skeletonId, bool loop, bool pause)
	{
		return CRenderSkinnedInstancing::setAnimation(entity, clipId, clipInfo, currentTime, bakeFps, skeletonId, loop, pause);
	}

	bool CRenderMeshInstancingVAT::setAnimation(CEntity* entity, int clipId, CAnimationClip* clipInfo, float clipBegin, float clipDuration, float currentTime, int bakeFps, int skeletonId, bool loop, bool pause)
	{
		return CRenderSkinnedInstancing::setAnimation(entity, clipId, clipInfo, clipBegin, clipDuration, currentTime, bakeFps, skeletonId, loop, pause);
	}

	void CRenderMeshInstancingVAT::setAnimationWeight(CEntity* entity, int skeletonId, float weight)
	{
		CRenderSkinnedInstancing::setAnimationWeight(entity, skeletonId, weight);
	}
}