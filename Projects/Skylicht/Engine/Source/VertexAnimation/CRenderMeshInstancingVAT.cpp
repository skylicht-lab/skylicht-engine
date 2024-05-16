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
#include "CSoftwareSkinningUtils.h"
#include "Entity/CEntityManager.h"
#include "Culling/CCullingData.h"
#include "Culling/CCullingBBoxData.h"
#include "Transform/CWorldInverseTransformData.h"

namespace Skylicht
{
	CRenderMeshInstancingVAT::CRenderMeshInstancingVAT()
	{

	}

	CRenderMeshInstancingVAT::~CRenderMeshInstancingVAT()
	{

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
			if (srcRender != NULL)
			{
				CRenderMeshData* spawnRender = spawnEntity->addData<CRenderMeshData>();
				spawnRender->setMesh(srcRender->getMesh());
				spawnRender->setSkinnedMesh(srcRender->isSkinnedMesh());

				// enable software skinned
				spawnRender->setSoftwareSkinning(true);
				spawnRender->initSoftwareSkinning();

				// add to list renderer
				m_renderers.push_back(spawnRender);

				// also add transform
				m_renderTransforms.push_back(GET_ENTITY_DATA(spawnEntity, CWorldTransformData));

				// add world inv transform for culling system (disable to optimize)
				// spawnEntity->addData<CWorldInverseTransformData>();
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

			// hide this
			spawnEntity->setVisible(false);
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

	void CRenderMeshInstancingVAT::updateSkinnedMesh()
	{
		CEntityManager* entityManager = m_gameObject->getEntityManager();

		// root entity of object
		m_root = m_gameObject->getEntity();
		CWorldInverseTransformData* rootInvTransform = GET_ENTITY_DATA(m_root, CWorldInverseTransformData);

		CEntity** entities = m_baseEntities.pointer();
		u32 numEntity = m_baseEntities.size();

		// step 1
		// like CJointAnimationSystem
		for (u32 i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			CJointData* joint = GET_ENTITY_DATA(entity, CJointData);

			if (joint)
			{
				if (rootInvTransform != NULL)
				{
					// move bone transform to Zero location
					joint->AnimationMatrix.setbyproduct_nocheck(rootInvTransform->WorldInverse, transform->World);
				}
				else
				{
					// if will have bugs if the SkinnedMesh isnot stand at Zero location
					joint->AnimationMatrix = transform->World;
				}
			}
		}

		// step 2
		// like CSkinnedMeshSystem
		for (CRenderMeshData* renderer : m_renderers)
		{
			CSkinnedMesh* skinnedMesh = (CSkinnedMesh*)renderer->getMesh();

			for (u32 j = 0, numJoint = skinnedMesh->Joints.size(); j < numJoint; j++)
			{
				CSkinnedMesh::SJoint& joint = skinnedMesh->Joints[j];

				f32* M = joint.SkinningMatrix;

				const f32* m1 = joint.JointData->AnimationMatrix.pointer();
				const f32* m2 = joint.BindPoseMatrix.pointer();

				// inline mul matrix
				M[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
				M[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
				M[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
				M[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

				M[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
				M[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
				M[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
				M[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

				M[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
				M[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
				M[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
				M[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

				M[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
				M[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
				M[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
				M[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
			}
		}

		// step 3
		// like CSoftwareSkinningSystem
		for (CRenderMeshData* renderer : m_renderers)
		{
			CSkinnedMesh* renderMesh = dynamic_cast<CSkinnedMesh*>(renderer->getMesh());
			CMesh* skinnedMesh = renderer->getSoftwareSkinnedMesh();

			if (renderMesh->getMeshBuffer(0)->getVertexDescriptor()->getID() == video::EVT_SKIN_TANGENTS)
				CSoftwareSkinningUtils::softwareSkinningTangent(skinnedMesh, renderMesh, NULL);
			else
				CSoftwareSkinningUtils::softwareSkinning(skinnedMesh, renderMesh, NULL);
		}
	}

	CEntity* CRenderMeshInstancingVAT::spawn()
	{
		CEntity* entity = createEntity();

		CIndirectLightingData* indirect = entity->addData<CIndirectLightingData>();
		indirect->initSH();

		// entity->addData<CWorldInverseTransformData>();
		entity->addData<CCullingData>();

		CCullingBBoxData* cullingBBox = entity->addData<CCullingBBoxData>();

		CEntityManager* entityMgr = m_gameObject->getEntityManager();

		bool firstBox = true;

		for (CRenderMeshData*& renderer : m_renderers)
		{
			// set bbox
			CMesh* mesh = renderer->getSoftwareSkinnedMesh();
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
			CRenderMeshData* renderMesh = entity->addData<CRenderMeshData>();
			renderMesh->setMesh(mesh);
			break;
		}

		return entity;
	}
}