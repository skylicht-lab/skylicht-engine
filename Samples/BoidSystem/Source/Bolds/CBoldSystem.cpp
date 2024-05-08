#include "pch.h"
#include "CBoldSystem.h"

CBoldSystem::CBoldSystem() :
	m_group(NULL)
{

}

CBoldSystem::~CBoldSystem()
{

}

void CBoldSystem::beginQuery(CEntityManager* entityManager)
{
	if (m_group == NULL)
	{
		const u32 boldType[] = GET_LIST_ENTITY_DATA(CBoldData);
		m_group = entityManager->findGroup(boldType, 1);
		if (m_group == NULL)
			m_group = entityManager->createGroupFromVisible(boldType, 1);
	}
}

void CBoldSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int count)
{
	entities = m_group->getEntities();
	count = m_group->getEntityCount();

	m_bolds.reset();
	m_transforms.reset();

	for (int i = 0; i < count; i++)
	{
		CEntity* entity = entities[i];

		CBoldData* bold = GET_ENTITY_DATA(entity, CBoldData);
		if (bold->Alive)
		{
			m_bolds.push(bold);
			m_transforms.push(GET_ENTITY_DATA(entity, CWorldTransformData));
		}
	}
}

void CBoldSystem::init(CEntityManager* entityManager)
{

}

void CBoldSystem::update(CEntityManager* entityManager)
{
	CBoldData** bolds = m_bolds.pointer();
	CWorldTransformData** transforms = m_transforms.pointer();
	int numEntity = m_bolds.count();

	separation(bolds, transforms, numEntity);
	alignment(bolds, transforms, numEntity);
	cohesion(bolds, transforms, numEntity);
	borders(bolds, transforms, numEntity);
	updateTransform(bolds, transforms, numEntity);
}

void CBoldSystem::separation(CBoldData** bolds, CWorldTransformData** transforms, int numEntity)
{
	// That will fix the collision each bold
	// Distance of field of vision for separation between boids
	float desiredSeparation = 0.4f;

	core::vector3df target, steer, diff;
	int count = 0;
	float timestepSec = getTimeStep() * 0.001f;
	float weight = 2.5f;

	for (int i = 0; i < numEntity; i++)
	{
		CBoldData* boldI = bolds[i];

		target.set(0.0f, 0.0f, 0.0f);
		count = 0;

		for (int j = 0; j < numEntity; j++)
		{
			if (i == j)
				continue;

			CBoldData* boldJ = bolds[j];

			diff = boldI->Location - boldJ->Location;

			float d = diff.getLength();
			if ((d > 0) && (d < desiredSeparation))
			{
				diff.normalize();
				diff /= d; // Weight by distance
				target += diff;
				count++;
			}
		}

		if (count > 0)
			target /= (float)count;

		if (target.getLengthSQ() > 0.0f) {
			target.normalize();
			target *= boldI->MaxSpeed;

			// Steering = target - velocity
			steer = target - boldI->Velocity;

			float force = boldI->MaxForce * timestepSec;
			if (steer.getLengthSQ() > force * force)
			{
				steer.normalize();
				steer *= force;
			}

			boldI->Acceleration += steer * weight;
		}
	}
}

void CBoldSystem::alignment(CBoldData** bolds, CWorldTransformData** transforms, int numEntity)
{
	// That will follow the moving at 5m
	// Distance of field of vision for separation between boids
	float desiredSeparation = 5.0f;
	float desiredSeparationSQ = desiredSeparation * desiredSeparation;

	core::vector3df target, steer, diff;
	int count = 0;
	float timestepSec = getTimeStep() * 0.001f;
	float weight = 0.2f;

	for (int i = 0; i < numEntity; i++)
	{
		CBoldData* boldI = bolds[i];

		target.set(0.0f, 0.0f, 0.0f);
		count = 0;

		for (int j = 0; j < numEntity; j++)
		{
			if (i == j)
				continue;

			CBoldData* boldJ = bolds[j];

			diff = boldI->Location - boldJ->Location;

			float d = diff.getLengthSQ();
			if ((d > 0) && (d < desiredSeparationSQ))
			{
				target += boldJ->Velocity;
				count++;
			}
		}

		if (count > 0)
			target /= (float)count;

		if (target.getLengthSQ() > 0.0f) {
			target.normalize();
			target *= boldI->MaxSpeed;

			// Steering = target - velocity
			steer = target - boldI->Velocity;

			float force = boldI->MaxForce * timestepSec * weight;
			if (steer.getLengthSQ() > force * force)
			{
				steer.normalize();
				steer *= force;
			}

			boldI->Acceleration += steer;
		}
	}
}

void CBoldSystem::cohesion(CBoldData** bolds, CWorldTransformData** transforms, int numEntity)
{
	// That will group the small team 0.5m
	// Distance of field of vision for separation between boids
	float desiredSeparation = 0.5f;
	float desiredSeparationSQ = desiredSeparation * desiredSeparation;

	core::vector3df target, steer, diff;
	int count = 0;
	float timestepSec = getTimeStep() * 0.001f;
	float weight = 0.2f;

	for (int i = 0; i < numEntity; i++)
	{
		CBoldData* boldI = bolds[i];

		target.set(0.0f, 0.0f, 0.0f);
		count = 0;

		for (int j = 0; j < numEntity; j++)
		{
			if (i == j)
				continue;

			CBoldData* boldJ = bolds[j];

			diff = boldI->Location - boldJ->Location;

			float d = diff.getLengthSQ();
			if ((d > 0) && (d < desiredSeparationSQ))
			{
				target += boldJ->Location;
				count++;
			}
		}

		if (count > 0)
			target /= (float)count;

		if (target.getLengthSQ() > 0.0f) {
			target *= -1.0f;
			target.normalize();
			target *= boldI->MaxSpeed;

			// Steering = target - velocity
			steer = target - boldI->Velocity;

			float force = boldI->MaxForce * timestepSec * weight;
			if (steer.getLengthSQ() > force * force)
			{
				steer.normalize();
				steer *= force;
			}

			boldI->Acceleration += steer;
		}
	}
}

void CBoldSystem::borders(CBoldData** bolds, CWorldTransformData** transforms, int numEntity)
{
	// That will turn when near the border
	float min = -25.0f;
	float max = 25.0f;
	float margin = 3.0f;

	core::vector3df center, steer;

	for (int i = 0; i < numEntity; i++)
	{
		CBoldData* bold = bolds[i];
		steer.set(0.0f, 0.0f, 0.0f);

		if (bold->Location.X < min + margin ||
			bold->Location.X > max - margin ||
			bold->Location.Z < min + margin ||
			bold->Location.Z > max - margin)
		{
			steer = center - bold->Location;
			steer.normalize();
			steer *= 0.0003f;

			bold->Acceleration += steer;
		}
	}
}

void CBoldSystem::updateTransform(CBoldData** bolds, CWorldTransformData** transforms, int numEntity)
{
	core::vector3df up(0.0f, 1.0f, 0.0f);
	core::vector3df right;

	float timestepSec = getTimeStep() * 0.001f;

	for (int i = 0; i < numEntity; i++)
	{
		CBoldData* bold = bolds[i];
		CWorldTransformData* transform = transforms[i];

		bold->Velocity += bold->Acceleration;

		if (bold->Velocity.getLengthSQ() > 0.0f)
		{
			bold->Front = bold->Velocity;
			bold->Front.normalize();
		}

		right = up.crossProduct(bold->Front);
		right.normalize();

		float speed = bold->MaxSpeed * timestepSec;
		if (bold->Velocity.getLengthSQ() > speed * speed)
			bold->Velocity = bold->Front * speed;

		bold->Location += bold->Velocity;

		bold->Acceleration.set(0.0f, 0.0f, 0.0f);

		// apply to transform
		f32* matData = transform->Relative.pointer();
		matData[0] = right.X;
		matData[1] = right.Y;
		matData[2] = right.Z;
		matData[3] = 0.0f;

		matData[4] = up.X;
		matData[5] = up.Y;
		matData[6] = up.Z;
		matData[7] = 0.0f;

		matData[8] = bold->Front.X;
		matData[9] = bold->Front.Y;
		matData[10] = bold->Front.Z;
		matData[11] = 0.0f;

		matData[12] = bold->Location.X;
		matData[13] = bold->Location.Y;
		matData[14] = bold->Location.Z;
		matData[15] = 1.0f;

		// notify changed for CGroupTransform and CWorldTransformSystem
		transform->HasChanged = true;
	}
}