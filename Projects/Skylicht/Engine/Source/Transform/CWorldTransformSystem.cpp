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
#include "CWorldTransformSystem.h"
#include "CWorldInverseTransformData.h"
#include "Entity/CEntityManager.h"
#include "Transform/CTransform.h"
#include "Culling/CVisibleData.h"

namespace Skylicht
{
	CWorldTransformSystem::CWorldTransformSystem() :
		m_groupTransform(NULL)
	{
	}

	CWorldTransformSystem::~CWorldTransformSystem()
	{

	}

	void CWorldTransformSystem::registerLateUpdate(ILateUpdate* lateUpdate)
	{
		for (ILateUpdate* l : m_lateUpdates)
		{
			if (l == lateUpdate)
				return;
		}
		m_lateUpdates.push_back(lateUpdate);
	}

	void CWorldTransformSystem::unRegisterLateUpdate(ILateUpdate* lateUpdate)
	{
		std::vector<ILateUpdate*>::iterator i = std::find(m_lateUpdates.begin(), m_lateUpdates.end(), lateUpdate);
		if (i != m_lateUpdates.end())
			m_lateUpdates.erase(i);
	}

	void CWorldTransformSystem::beginQuery(CEntityManager* entityManager)
	{
		if (m_groupTransform == NULL)
		{
			const u32 visibleGroupType[] = GET_LIST_ENTITY_DATA(CVisibleData);
			CEntityGroup* visibleGroup = entityManager->findGroup(visibleGroupType, 1);

			m_groupTransform = (CGroupTransform*)entityManager->addCustomGroup(new CGroupTransform(visibleGroup));
		}
	}

	void CWorldTransformSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
	}

	void CWorldTransformSystem::init(CEntityManager* entityManager)
	{
	}

	void CWorldTransformSystem::update(CEntityManager* entityManager)
	{
		CEntity** allEntities = entityManager->getEntities();

		CWorldTransformData** transforms = m_groupTransform->getRoots();
		int numEntity = m_groupTransform->getNumRoot();

		for (int i = 0; i < numEntity; i++)
		{
			CWorldTransformData* t = transforms[i];
			t->World = t->Relative;
		}

		transforms = m_groupTransform->getChilds();
		numEntity = m_groupTransform->getNumChilds();

		for (int i = 0; i < numEntity; i++)
		{
			CWorldTransformData* t = transforms[i];

			// calc world = parent * relative
			// - relative is copied from CTransformComponentSystem
			// - relative is also defined in CEntityPrefab
			t->World.setbyproduct_nocheck(t->Parent->World, t->Relative);
		}

		// call late update
		for (ILateUpdate* l : m_lateUpdates)
			l->lateUpdate();

		// late update
		transforms = m_groupTransform->getLateUpdate();
		numEntity = m_groupTransform->getLateUpdateCount();
		for (int i = 0; i < numEntity; i++)
		{
			CWorldTransformData* t = transforms[i];
			t->World.setbyproduct_nocheck(t->Parent->World, t->Relative);
		}
	}
}