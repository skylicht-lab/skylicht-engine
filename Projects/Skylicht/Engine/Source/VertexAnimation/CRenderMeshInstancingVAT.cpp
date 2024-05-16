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
			// spawnEntity->setVisible(false);
		}

		bool addInvData = true;
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
}