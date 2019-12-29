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
#include "CRenderMesh.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"

#include "Transform/CWorldTransformData.h"
#include "RenderMesh/CRenderMeshData.h"
#include "RenderMesh/CJointData.h"
#include "Culling/CCullingData.h"

namespace Skylicht
{
	CRenderMesh::CRenderMesh()
	{

	}

	CRenderMesh::~CRenderMesh()
	{
		CEntityManager *entityManager = m_gameObject->getEntityManager();
		for (u32 i = 0, n = m_childs.size(); i < n; i++)
		{
			entityManager->removeEntity(m_childs[i]);
		}
		m_childs.clear();
	}

	void CRenderMesh::initComponent()
	{

	}

	void CRenderMesh::updateComponent()
	{

	}

	void CRenderMesh::initFromPrefab(CEntityPrefab *prefab)
	{
		CEntityManager *entityManager = m_gameObject->getEntityManager();

		// root entity of object
		CEntity *rootEntity = m_gameObject->getEntity();
		CWorldTransformData *rootTransform = rootEntity->getData<CWorldTransformData>();

		// spawn childs entity
		int numEntities = prefab->getNumEntities();
		CEntity** entities = entityManager->createEntity(numEntities, m_childs);

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
				CWorldTransformData *spawnTransform = spawnEntity->addData<CWorldTransformData>();
				spawnTransform->Name = srcTransform->Name;
				spawnTransform->Relative = srcTransform->Relative;
				spawnTransform->HasChanged = true;
				spawnTransform->Depth = rootTransform->Depth + srcTransform->Depth;

				if (srcTransform->ParentIndex == -1)
				{
					spawnTransform->ParentIndex = rootEntity->getIndex();
				}
				else
				{
					spawnTransform->ParentIndex = entityIndex[srcTransform->ParentIndex];
				}
			}

			// copy render data
			CRenderMeshData *srcRender = srcEntity->getData<CRenderMeshData>();
			if (srcRender != NULL)
			{
				CRenderMeshData *spawnRender = spawnEntity->addData<CRenderMeshData>();
				spawnRender->setMesh(srcRender->getMesh());
			}

			// copy culling data
			CCullingData *srcCulling = srcEntity->getData<CCullingData>();
			if (srcCulling != NULL)
			{
				CCullingData *spawnCulling = spawnEntity->addData<CCullingData>();
				spawnCulling->Type = srcCulling->Type;
				spawnCulling->Visible = srcCulling->Visible;
			}

			// copy joint data
			CJointData *srcJointData = srcEntity->getData<CJointData>();
			if (srcJointData != NULL)
			{
				CJointData *spawnJoint = spawnEntity->addData<CJointData>();
				spawnJoint->BoneRoot = srcJointData->BoneRoot;
				spawnJoint->SID = srcJointData->SID;
				spawnJoint->BoneName = srcJointData->BoneName;
				spawnJoint->AnimationMatrix = srcJointData->AnimationMatrix;
				spawnJoint->DefaultAnimationMatrix = srcJointData->DefaultAnimationMatrix;
				spawnJoint->DefaultRelativeMatrix = srcJointData->DefaultRelativeMatrix;
			}
		}
	}
}