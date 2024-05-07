#include "pch.h"
#include "CBoldAnimationSystem.h"
#include "SkinnedInstancing/CRenderSkinnedInstancing.h"

CBoldAnimationSystem::CBoldAnimationSystem() :
	m_group(NULL)
{

}

CBoldAnimationSystem::~CBoldAnimationSystem()
{

}

void CBoldAnimationSystem::beginQuery(CEntityManager* entityManager)
{
	if (m_group == NULL)
	{
		const u32 boldType[] = GET_LIST_ENTITY_DATA(CBoldData);
		m_group = entityManager->findGroup(boldType, 1);
		if (m_group == NULL)
			m_group = entityManager->createGroupFromVisible(boldType, 1);
	}
}

void CBoldAnimationSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int count)
{
	entities = m_group->getEntities();
	count = m_group->getEntityCount();

	m_entities.reset();
	m_bolds.reset();
	m_skinnedInstances.reset();

	for (int i = 0; i < count; i++)
	{
		CEntity* entity = entities[i];

		CBoldData* bold = GET_ENTITY_DATA(entity, CBoldData);
		if (bold->Alive)
		{
			CSkinnedInstanceData* skinnedInstance = GET_ENTITY_DATA(entity, CSkinnedInstanceData);
			if (skinnedInstance)
			{
				m_entities.push(entity);
				m_bolds.push(bold);
				m_skinnedInstances.push(skinnedInstance);
			}
		}
	}
}

void CBoldAnimationSystem::init(CEntityManager* entityManager)
{

}

void CBoldAnimationSystem::addClip(CAnimationClip* clip, int clipId, int fps, float runSpeed)
{
	m_clips.push_back(SMovingAnimation());

	SMovingAnimation& a = m_clips.getLast();
	a.ClipId = clipId;
	a.Clip = clip;
	a.FPS = fps;
	a.Speed = runSpeed;
}

void CBoldAnimationSystem::update(CEntityManager* entityManager)
{
	SMovingAnimation* clips = m_clips.pointer();
	int numClip = (int)m_clips.size();

	CEntity** entities = m_entities.pointer();
	CBoldData** bolds = m_bolds.pointer();
	CSkinnedInstanceData** skinnedIntances = m_skinnedInstances.pointer();

	int numEntity = m_bolds.count();

	float speed = 0.0f;
	float maxWeight = 0.0f;

	float frameRatio = 0.0f;
	int clipId = 0;

	SSkeletonAnimation* skeleton;

	float minAnimSpeed = m_clips[0].Speed;
	float maxAnimSpeed = m_clips[numClip - 1].Speed;

	for (int i = 0; i < numEntity; i++)
	{
		CBoldData* bold = bolds[i];
		CSkinnedInstanceData* instance = skinnedIntances[i];

		speed = bold->Velocity.getLength();
		speed = core::clamp(speed, minAnimSpeed, maxAnimSpeed);

		maxWeight = 0.0f;
		frameRatio = 0.0f;

		for (int j = 0; j < 2; j++)
		{
			skeleton = &instance->Skeletons[j];

			if (maxWeight < skeleton->Weight)
			{
				maxWeight = skeleton->Weight;
				float duration = skeleton->TimeTo - skeleton->TimeFrom;
				float currentTime = skeleton->Time - skeleton->TimeFrom;
				if (duration > 0.0f)
					frameRatio = currentTime / duration;
			}
		}

		for (int j = 0; j < numClip - 1; j++)
		{
			SMovingAnimation& clip1 = clips[j];
			SMovingAnimation& clip2 = clips[j + 1];

			if (clip1.Speed <= speed && speed <= clip2.Speed)
			{
				// Update animation blending
				if (clip1.ClipId != instance->Skeletons[0].ClipId)
				{
					CRenderSkinnedInstancing::setAnimation(
						entities[i],
						clip1.ClipId,
						clip1.Clip,
						frameRatio * clip1.Clip->Duration,
						clip1.FPS,
						0);

					CRenderSkinnedInstancing::setAnimation(
						entities[i],
						clip2.ClipId,
						clip2.Clip,
						frameRatio * clip2.Clip->Duration,
						clip2.FPS,
						1);
				}

				// Calc animation weight by entity Speed
				float s = speed - clip1.Speed;
				float total = clip2.Speed - clip1.Speed;
				float weight1 = 1.0f;
				float weight2 = 0.0f;

				if (total > 0)
				{
					weight2 = s / total;
					weight1 = 1.0f - weight2;
				}

				CRenderSkinnedInstancing::setAnimationWeight(entities[i], 0, weight1);
				CRenderSkinnedInstancing::setAnimationWeight(entities[i], 1, weight2);

				break;
			}
		}
	}
}