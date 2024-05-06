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

	borders(bolds, transforms, numEntity);
	// separation(bolds, transforms, numEntity);
	updateTransform(bolds, transforms, numEntity);
}

void CBoldSystem::separation(CBoldData** bolds, CWorldTransformData** transforms, int numEntity)
{
	// Distance of field of vision for separation between boids
	float desiredSeparation = 20.0f * 20.0f;

	core::vector3df steer;
	int count = 0;

	for (int i = 0; i < numEntity; i++)
	{
		CBoldData* boldI = bolds[i];

		steer.set(0.0f, 0.0f, 0.0f);
		count = 0;

		for (int j = 0; j < numEntity; j++)
		{
			CBoldData* boldJ = bolds[j];

			core::vector3df diff = boldI->Location - boldJ->Location;

			float d = diff.getLength();
			if ((d > 0) && (d < desiredSeparation))
			{
				diff.normalize();
				diff /= d; // Weight by distance
				steer += diff;
				count++;
			}
		}

		if (count > 0)
			steer /= (float)count;

		if (steer.getLengthSQ() > 0.0f) {
			steer.normalize();
			steer *= boldI->MaxSpeed;
			steer -= boldI->Velocity;

			if (steer.getLengthSQ() > boldI->MaxForce * boldI->MaxForce)
			{
				steer.normalize();
				steer *= boldI->MaxForce;
			}

			boldI->Acceleration += steer;
		}
	}
}

void CBoldSystem::borders(CBoldData** bolds, CWorldTransformData** transforms, int numEntity)
{
	float min = -25.0f;
	float max = 25.0f;
	float margin = 2.0f;

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
			steer *= 0.0001f;

			bold->Acceleration += steer;
		}
	}
}

void CBoldSystem::updateTransform(CBoldData** bolds, CWorldTransformData** transforms, int numEntity)
{
	core::vector3df up(0.0f, 1.0f, 0.0f);

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

		core::vector3df right = up.crossProduct(bold->Front);
		right.normalize();

		float speed = bold->MaxSpeed * getTimeStep() * 0.001f;
		if (bold->Velocity.getLengthSQ() > speed * speed)
		{
			bold->Velocity = bold->Front * speed;
		}

		bold->Location += bold->Velocity;

		bold->Acceleration.set(0.0f, 0.0f, 0.0f);

		// apply to transform
		transform->HasChanged = true;
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
	}
}