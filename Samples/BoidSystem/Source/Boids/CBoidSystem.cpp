#include "pch.h"
#include "CBoidSystem.h"

CBoidSystem::CBoidSystem() :
	m_group(NULL),
	m_minX(-25.0f),
	m_maxX(25.0f),
	m_minZ(-25.0f),
	m_maxZ(25.0f)
{

}

CBoidSystem::~CBoidSystem()
{

}

void CBoidSystem::beginQuery(CEntityManager* entityManager)
{
	if (m_group == NULL)
	{
		const u32 boldType[] = GET_LIST_ENTITY_DATA(CBoidData);
		m_group = entityManager->findGroup(boldType, 1);
		if (m_group == NULL)
			m_group = entityManager->createGroupFromVisible(boldType, 1);
	}
}

void CBoidSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int count)
{
	entities = m_group->getEntities();
	count = m_group->getEntityCount();

	m_bolds.reset();
	m_transforms.reset();

	for (int i = 0; i < count; i++)
	{
		CEntity* entity = entities[i];

		CBoidData* bold = GET_ENTITY_DATA(entity, CBoidData);
		if (bold->Alive)
		{
			m_bolds.push(bold);
			m_transforms.push(GET_ENTITY_DATA(entity, CWorldTransformData));
		}
	}

	m_neighbor.clear();
}

void CBoidSystem::init(CEntityManager* entityManager)
{

}

void CBoidSystem::update(CEntityManager* entityManager)
{
	CBoidData** bolds = m_bolds.pointer();
	CWorldTransformData** transforms = m_transforms.pointer();
	int numEntity = m_bolds.count();

	neighbor(bolds, transforms, numEntity);
	separation(bolds, transforms, numEntity);
	alignment(bolds, transforms, numEntity);
	cohesion(bolds, transforms, numEntity);
	borders(bolds, transforms, numEntity);
	updateTransform(bolds, transforms, numEntity);
}

void CBoidSystem::neighbor(CBoidData** bolds, CWorldTransformData** transforms, int numEntity)
{
	m_neighbor.add(bolds, numEntity);

	CBoidData* bold;
	for (int i = 0; i < numEntity; i++)
	{
		bold = bolds[i];
		bold->Neighbor.reset();

		m_neighbor.queryNeighbor(bold->Location, bold->Neighbor);
	}
}

void CBoidSystem::separation(CBoidData** bolds, CWorldTransformData** transforms, int numEntity)
{
	// That will fix the collision each bold
	// Distance of field of vision for separation between boids
	float desiredSeparation = 0.4f;

	core::vector3df target, steer, diff;
	int count = 0;
	float timestepSec = getTimeStep() * 0.001f;
	float weight = 2.5f;

	CBoidData** neighbor;
	CBoidData* boldI;
	CBoidData* boldJ;
	int numNeighbor;

	for (int i = 0; i < numEntity; i++)
	{
		boldI = bolds[i];

		target.set(0.0f, 0.0f, 0.0f);
		count = 0;

		neighbor = boldI->Neighbor.pointer();
		numNeighbor = boldI->Neighbor.count();

		for (int j = 0; j < numNeighbor; j++)
		{
			boldJ = neighbor[j];
			if (boldI == boldJ)
				continue;

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

void CBoidSystem::alignment(CBoidData** bolds, CWorldTransformData** transforms, int numEntity)
{
	// That will follow the moving at 5m
	// Distance of field of vision for separation between boids
	float desiredSeparation = 5.0f;
	float desiredSeparationSQ = desiredSeparation * desiredSeparation;

	core::vector3df target, steer, diff;
	int count = 0;
	float timestepSec = getTimeStep() * 0.001f;
	float weight = 0.3f;

	CBoidData** neighbor;
	CBoidData* boldI;
	CBoidData* boldJ;
	int numNeighbor;

	for (int i = 0; i < numEntity; i++)
	{
		boldI = bolds[i];

		target.set(0.0f, 0.0f, 0.0f);
		count = 0;

		neighbor = boldI->Neighbor.pointer();
		numNeighbor = boldI->Neighbor.count();

		for (int j = 0; j < numNeighbor; j++)
		{
			boldJ = neighbor[j];
			if (boldI == boldJ)
				continue;

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

void CBoidSystem::cohesion(CBoidData** bolds, CWorldTransformData** transforms, int numEntity)
{
	// That will group the small team 0.5m
	// Distance of field of vision for separation between boids
	float desiredSeparation = 0.5f;
	float desiredSeparationSQ = desiredSeparation * desiredSeparation;

	core::vector3df target, steer, diff;
	int count = 0;
	float timestepSec = getTimeStep() * 0.001f;
	float weight = 0.2f;

	CBoidData** neighbor;
	CBoidData* boldI;
	CBoidData* boldJ;
	int numNeighbor;

	for (int i = 0; i < numEntity; i++)
	{
		boldI = bolds[i];

		target.set(0.0f, 0.0f, 0.0f);
		count = 0;

		neighbor = boldI->Neighbor.pointer();
		numNeighbor = boldI->Neighbor.count();

		for (int j = 0; j < numNeighbor; j++)
		{
			boldJ = neighbor[j];
			if (boldI == boldJ)
				continue;

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

void CBoidSystem::borders(CBoidData** bolds, CWorldTransformData** transforms, int numEntity)
{
	// That will turn when near the border
	core::vector3df center, steer;
	CBoidData* bold;

	for (int i = 0; i < numEntity; i++)
	{
		bold = bolds[i];
		steer.set(0.0f, 0.0f, 0.0f);

		if (bold->Location.X < m_minX + m_margin ||
			bold->Location.X > m_maxX - m_margin ||
			bold->Location.Z < m_minZ + m_margin ||
			bold->Location.Z > m_maxZ - m_margin)
		{
			steer = center - bold->Location;
			steer.normalize();
			steer *= 0.0003f;

			bold->Acceleration += steer;
		}
	}
}

void CBoidSystem::updateTransform(CBoidData** bolds, CWorldTransformData** transforms, int numEntity)
{
	core::vector3df up(0.0f, 1.0f, 0.0f);
	core::vector3df right;
	core::vector3df lastPostion;

	float timestepSec = getTimeStep() * 0.001f;

	float speed;
	CBoidData* bold;
	CWorldTransformData* transform;
	f32* matData;

	for (int i = 0; i < numEntity; i++)
	{
		bold = bolds[i];
		transform = transforms[i];

		// update velocity
		bold->Velocity += bold->Acceleration;

		if (bold->Velocity.getLengthSQ() > 0.0f)
		{
			bold->Front = bold->Velocity;
			bold->Front.normalize();
		}

		right = up.crossProduct(bold->Front);
		right.normalize();

		speed = bold->MaxSpeed * timestepSec;
		if (bold->Velocity.getLengthSQ() > speed * speed)
			bold->Velocity = bold->Front * speed;

		lastPostion = bold->Location;

		bold->Location += bold->Velocity;

		// clamp bound
		bold->Location.X = core::clamp(bold->Location.X, m_minX, m_maxX);
		bold->Location.Z = core::clamp(bold->Location.Z, m_minZ, m_maxZ);

		// update velocity after clamp
		bold->Velocity = bold->Location - lastPostion;

		// reset the acceleration
		bold->Acceleration.set(0.0f, 0.0f, 0.0f);

		// apply to transform
		matData = transform->Relative.pointer();
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