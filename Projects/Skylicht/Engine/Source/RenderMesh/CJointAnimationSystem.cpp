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
#include "CJointAnimationSystem.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	CJointAnimationSystem::CJointAnimationSystem()
	{

	}

	CJointAnimationSystem::~CJointAnimationSystem()
	{

	}

	void CJointAnimationSystem::beginQuery()
	{
		m_joints.set_used(0);
		m_transforms.set_used(0);
		m_rootInvTransform.set_used(0);
	}

	void CJointAnimationSystem::onQuery(CEntityManager *entityManager, CEntity *entity)
	{
		CJointData *joint = entity->getData<CJointData>();
		if (joint != NULL && joint->RootIndex != 0)
		{
			CWorldTransformData *transform = entity->getData<CWorldTransformData>();
			CWorldInvTransformData *rootInvTransform = entityManager->getEntity(joint->RootIndex)->getData<CWorldInvTransformData>();

			if (transform != NULL && rootInvTransform != NULL)
			{
				m_joints.push_back(joint);
				m_transforms.push_back(transform);
				m_rootInvTransform.push_back(rootInvTransform);
			}
		}
	}

	void CJointAnimationSystem::init(CEntityManager *entityManager)
	{

	}

	void CJointAnimationSystem::update(CEntityManager *entityManager)
	{
		CJointData **joints = m_joints.pointer();
		CWorldTransformData **transforms = m_transforms.pointer();
		CWorldInvTransformData **rootInvTransform = m_rootInvTransform.pointer();

		// calc animation matrix for CSkinnedMeshSystem
		for (u32 i = 0, n = m_joints.size(); i < n; i++)
		{
			joints[i]->AnimationMatrix.setbyproduct_nocheck(rootInvTransform[i]->WorldInverse, transforms[i]->World);
		}
	}
}