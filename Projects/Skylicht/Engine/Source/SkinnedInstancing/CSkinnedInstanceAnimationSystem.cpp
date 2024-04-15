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
#include "CSkinnedInstanceAnimationSystem.h"

#include "Entity/CEntityManager.h"
#include "Culling/CVisibleData.h"
#include "SkinnedInstancing/CSkinnedInstanceData.h"

namespace Skylicht
{
	CSkinnedInstanceAnimationSystem::CSkinnedInstanceAnimationSystem() :
		m_group(NULL)
	{

	}

	CSkinnedInstanceAnimationSystem::~CSkinnedInstanceAnimationSystem()
	{

	}

	void CSkinnedInstanceAnimationSystem::beginQuery(CEntityManager* entityManager)
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

		m_skinnedEntities.reset();
	}

	void CSkinnedInstanceAnimationSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		entities = m_group->getEntities();
		numEntity = m_group->getEntityCount();

		for (int i = 0; i < numEntity; i++)
		{
			m_skinnedEntities.push(GET_ENTITY_DATA(entities[i], CSkinnedInstanceData));
		}
	}

	void CSkinnedInstanceAnimationSystem::init(CEntityManager* entityManager)
	{

	}

	void CSkinnedInstanceAnimationSystem::update(CEntityManager* entityManager)
	{
		CSkinnedInstanceData** entities = m_skinnedEntities.pointer();
		int numEntity = m_skinnedEntities.count();

		float t = getTimeStep() / 1000.0f;

		for (int i = 0; i < numEntity; i++)
		{
			CSkinnedInstanceData* entity = entities[i];
			if (!entity->Pause)
			{
				entity->Time = entity->Time + t;
				if (entity->Time >= entity->TimeTo)
				{
					if (entity->Loop)
						entity->Time = entity->TimeFrom;
					else
						entity->Time = entity->TimeTo;
				}
				entity->Frame = (int)(entity->Time * (float)entity->FPS);
			}
		}
	}
}