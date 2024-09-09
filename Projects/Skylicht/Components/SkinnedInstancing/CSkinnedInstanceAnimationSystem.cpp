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

			int mainSkeleton = 0;
			float maxWeight = 0.0f;
			float frameRatio = 0.0f;

			for (int skeletonId = 0; skeletonId < 2; skeletonId++)
			{
				SSkeletonAnimation* skeleton = &entity->Skeletons[skeletonId];

				// find the main skeleton
				if (maxWeight < skeleton->Weight)
				{
					maxWeight = skeleton->Weight;
					mainSkeleton = skeletonId;
				}

				// update time/frame
				if (!skeleton->Pause && skeleton->Weight > 0.0f)
				{
					skeleton->Time = skeleton->Time + t;
					if (skeleton->Time >= skeleton->TimeTo)
					{
						if (skeleton->Loop)
							skeleton->Time = skeleton->TimeFrom;
						else
							skeleton->Time = skeleton->TimeTo;
					}

					skeleton->Frame = (int)(skeleton->Time * (float)skeleton->FPS);

					if (skeletonId == mainSkeleton)
					{
						float duration = skeleton->TimeTo - skeleton->TimeFrom;
						float currentTime = skeleton->Time - skeleton->TimeFrom;
						if (duration > 0.0f)
							frameRatio = currentTime / duration;
						else
							frameRatio = 0.0f;
					}
				}
			}

			// sync by time scale
			// like the function CSkeleton::syncAnimationByTimeScale
			for (int skeletonId = 0; skeletonId < 2; skeletonId++)
			{
				SSkeletonAnimation* skeleton = &entity->Skeletons[skeletonId];

				if (skeletonId != mainSkeleton && !skeleton->Pause && skeleton->Weight > 0.0f)
				{
					float duration = skeleton->TimeTo - skeleton->TimeFrom;

					// calc time/frame by main ratio
					skeleton->Time = skeleton->TimeFrom + frameRatio * duration;
					skeleton->Frame = (int)(skeleton->Time * (float)skeleton->FPS);
				}
			}
		}
	}
}