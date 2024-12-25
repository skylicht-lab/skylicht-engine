#include "pch.h"
#include "CBoidAnimationSystem.h"

CBoidAnimationSystem::CBoidAnimationSystem() :
	m_group(NULL)
{

}

CBoidAnimationSystem::~CBoidAnimationSystem()
{

}

void CBoidAnimationSystem::beginQuery(CEntityManager* entityManager)
{
	if (m_group == NULL)
	{
		const u32 boidType[] = GET_LIST_ENTITY_DATA(CBoidData);
		m_group = entityManager->findGroup(boidType, 1);
		if (m_group == NULL)
			m_group = entityManager->createGroupFromVisible(boidType, 1);
	}
}

void CBoidAnimationSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int count)
{
	entities = m_group->getEntities();
	count = m_group->getEntityCount();

	m_boids.reset();
	m_skinnedInstances.reset();

	for (int i = 0; i < count; i++)
	{
		CEntity* entity = entities[i];

		CBoidData* boid = GET_ENTITY_DATA(entity, CBoidData);
		if (boid->Alive)
		{
			CSkinnedInstanceData* skinnedInstance = GET_ENTITY_DATA(entity, CSkinnedInstanceData);
			if (skinnedInstance)
			{
				m_boids.push(boid);
				m_skinnedInstances.push(skinnedInstance);
			}
		}
	}
}

void CBoidAnimationSystem::init(CEntityManager* entityManager)
{

}

void CBoidAnimationSystem::addClip(CAnimationClip* clip, int clipId, int fps, float runSpeed)
{
	m_clips.push_back(SMovingAnimation());

	SMovingAnimation& a = m_clips.getLast();
	a.ClipId = clipId;
	a.Clip = clip;
	a.FPS = fps;
	a.Speed = runSpeed;
}

void CBoidAnimationSystem::update(CEntityManager* entityManager)
{
	SMovingAnimation* clips = m_clips.pointer();
	int numClip = (int)m_clips.size();

	CBoidData** boids = m_boids.pointer();
	CSkinnedInstanceData** skinnedIntances = m_skinnedInstances.pointer();

	int numEntity = m_boids.count();

	float speed = 0.0f;
	float maxWeight = 0.0f;

	float frameRatio = 0.0f;
	int clipId = 0;

	SSkeletonAnimation* skeleton;

	float minAnimSpeed = m_clips[0].Speed;
	float maxAnimSpeed = m_clips[numClip - 1].Speed;

	for (int i = 0; i < numEntity; i++)
	{
		CBoidData* boid = boids[i];
		CSkinnedInstanceData* instance = skinnedIntances[i];

		speed = boid->Velocity.getLength();
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
					instance->setAnimation(
						clip1.ClipId,
						clip1.Clip,
						frameRatio * clip1.Clip->Duration,
						clip1.FPS,
						0);

					instance->setAnimation(
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

				instance->setAnimationWeight(0, weight1);
				instance->setAnimationWeight(1, weight2);

				break;
			}
		}
	}
}