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
#include "CSkinnedMeshSystem.h"

namespace Skylicht
{
	CSkinnedMeshSystem::CSkinnedMeshSystem()
	{

	}

	CSkinnedMeshSystem::~CSkinnedMeshSystem()
	{

	}

	void CSkinnedMeshSystem::beginQuery()
	{
		m_meshs.set_used(0);
	}

	void CSkinnedMeshSystem::onQuery(CEntityManager *entityManager, CEntity *entity)
	{
		CRenderMeshData *renderer = entity->getData<CRenderMeshData>();
		if (renderer != NULL && renderer->isSkinnedMesh())
		{
			CSkinnedMesh *mesh = NULL;

			if (renderer->isSoftwareSkinning() == true)
				mesh = dynamic_cast<CSkinnedMesh*>(renderer->getOriginalMesh());
			else
				mesh = dynamic_cast<CSkinnedMesh*>(renderer->getMesh());

			if (mesh != NULL)
				m_meshs.push_back(mesh);
		}
	}

	void CSkinnedMeshSystem::init(CEntityManager *entityManager)
	{

	}

	void CSkinnedMeshSystem::update(CEntityManager *entityManager)
	{
		CSkinnedMesh** meshs = m_meshs.pointer();
		for (u32 i = 0, n = m_meshs.size(); i < n; i++)
		{
			CSkinnedMesh *skinnedMesh = meshs[i];

			for (u32 j = 0, numJoint = skinnedMesh->Joints.size(); j < numJoint; j++)
			{
				CSkinnedMesh::SJoint& joint = skinnedMesh->Joints[j];

				// skinMat = animMat * invMat * bindShapMat
				// note:
				// AnimationMatrix is a transform of bone at pos (0,0,0)
				core::matrix4 mat;
				mat.setbyproduct(joint.GlobalInversedMatrix, skinnedMesh->BindShapeMatrix);
				joint.SkinningMatrix.setbyproduct(joint.JointData->AnimationMatrix, mat);
			}
		}
	}
}