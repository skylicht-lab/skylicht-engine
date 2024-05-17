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
#include "Culling/CVisibleData.h"
#include "Entity/CEntityManager.h"
#include "CSkinnedMeshSystem.h"

namespace Skylicht
{
	CSkinnedMeshSystem::CSkinnedMeshSystem()
	{

	}

	CSkinnedMeshSystem::~CSkinnedMeshSystem()
	{

	}

	void CSkinnedMeshSystem::beginQuery(CEntityManager* entityManager)
	{
		CMeshSystem::beginQuery(entityManager);
	}

	void CSkinnedMeshSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{

	}

	void CSkinnedMeshSystem::init(CEntityManager* entityManager)
	{

	}

	void CSkinnedMeshSystem::update(CEntityManager* entityManager)
	{
		int numEntity = m_groupMesh->getNumSkinnedMesh();
		CEntity** entities = m_groupMesh->getSkinnedMeshes();

		updateSkinnedMesh(entityManager, entities, numEntity);
	}

	void CSkinnedMeshSystem::updateSkinnedMesh(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];
			CRenderMeshData* renderer = GET_ENTITY_DATA(entity, CRenderMeshData);
			CSkinnedMesh* skinnedMesh = (CSkinnedMesh*)renderer->getMesh();

			for (u32 j = 0, numJoint = skinnedMesh->Joints.size(); j < numJoint; j++)
			{
				CSkinnedMesh::SJoint& joint = skinnedMesh->Joints[j];

				// gpuSkinMat = animMat * bindPoseMatrix
				// bindPoseMatrix = invMat * bindShapMat (see collada loader)
				// animMat = transform of joint at pos (0,0,0)
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
	}
}