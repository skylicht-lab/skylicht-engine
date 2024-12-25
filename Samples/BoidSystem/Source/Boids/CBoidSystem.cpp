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
		const u32 boidType[] = GET_LIST_ENTITY_DATA(CBoidData);
		m_group = entityManager->findGroup(boidType, 1);
		if (m_group == NULL)
			m_group = entityManager->createGroupFromVisible(boidType, 1);
	}
}

void CBoidSystem::onQuery(CEntityManager* entityManager, CEntity** entities, int count)
{
	entities = m_group->getEntities();
	count = m_group->getEntityCount();

	m_boids.reset();
	m_transforms.reset();

	for (int i = 0; i < count; i++)
	{
		CEntity* entity = entities[i];

		CBoidData* boid = GET_ENTITY_DATA(entity, CBoidData);
		if (boid->Alive)
		{
			m_boids.push(boid);
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
	CBoidData** boids = m_boids.pointer();
	CWorldTransformData** transforms = m_transforms.pointer();
	int numEntity = m_boids.count();

	neighbor(boids, transforms, numEntity);
	separation(boids, transforms, numEntity);
	alignment(boids, transforms, numEntity);
	cohesion(boids, transforms, numEntity);
	borders(boids, transforms, numEntity);
	updateTransform(boids, transforms, numEntity);
}

void CBoidSystem::neighbor(CBoidData** boids, CWorldTransformData** transforms, int numEntity)
{
	m_neighbor.add(boids, numEntity);

	CBoidData* boid;
	for (int i = 0; i < numEntity; i++)
	{
		boid = boids[i];
		boid->Neighbor.reset();

		m_neighbor.queryNeighbor(boid->Location, boid->Neighbor);
	}
}

void CBoidSystem::separation(CBoidData** boids, CWorldTransformData** transforms, int numEntity)
{
	// That will fix the collision each boid
	// Distance of field of vision for separation between boids
	float desiredSeparation = 0.4f;

	core::vector3df target, steer, diff;
	int count = 0;
	float timestepSec = getTimeStep() * 0.001f;
	float weight = 2.5f;

	CBoidData** neighbor;
	CBoidData* boidI;
	CBoidData* boidJ;
	int numNeighbor;

	for (int i = 0; i < numEntity; i++)
	{
		boidI = boids[i];

		target.set(0.0f, 0.0f, 0.0f);
		count = 0;

		neighbor = boidI->Neighbor.pointer();
		numNeighbor = boidI->Neighbor.count();

		for (int j = 0; j < numNeighbor; j++)
		{
			boidJ = neighbor[j];
			if (boidI == boidJ)
				continue;

			diff = boidI->Location - boidJ->Location;

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
			target *= boidI->MaxSpeed;

			// Steering = target - velocity
			steer = target - boidI->Velocity;

			float force = boidI->MaxForce * timestepSec;
			if (steer.getLengthSQ() > force * force)
			{
				steer.normalize();
				steer *= force;
			}

			boidI->Acceleration += steer * weight;
		}
	}
}

void CBoidSystem::alignment(CBoidData** boids, CWorldTransformData** transforms, int numEntity)
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
	CBoidData* boidI;
	CBoidData* boidJ;
	int numNeighbor;

	for (int i = 0; i < numEntity; i++)
	{
		boidI = boids[i];

		target.set(0.0f, 0.0f, 0.0f);
		count = 0;

		neighbor = boidI->Neighbor.pointer();
		numNeighbor = boidI->Neighbor.count();

		for (int j = 0; j < numNeighbor; j++)
		{
			boidJ = neighbor[j];
			if (boidI == boidJ)
				continue;

			diff = boidI->Location - boidJ->Location;

			float d = diff.getLengthSQ();
			if ((d > 0) && (d < desiredSeparationSQ))
			{
				target += boidJ->Velocity;
				count++;
			}
		}

		if (count > 0)
			target /= (float)count;

		if (target.getLengthSQ() > 0.0f) {
			target.normalize();
			target *= boidI->MaxSpeed;

			// Steering = target - velocity
			steer = target - boidI->Velocity;

			float force = boidI->MaxForce * timestepSec * weight;
			if (steer.getLengthSQ() > force * force)
			{
				steer.normalize();
				steer *= force;
			}

			boidI->Acceleration += steer;
		}
	}
}

void CBoidSystem::cohesion(CBoidData** boids, CWorldTransformData** transforms, int numEntity)
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
	CBoidData* boidI;
	CBoidData* boidJ;
	int numNeighbor;

	for (int i = 0; i < numEntity; i++)
	{
		boidI = boids[i];

		target.set(0.0f, 0.0f, 0.0f);
		count = 0;

		neighbor = boidI->Neighbor.pointer();
		numNeighbor = boidI->Neighbor.count();

		for (int j = 0; j < numNeighbor; j++)
		{
			boidJ = neighbor[j];
			if (boidI == boidJ)
				continue;

			diff = boidI->Location - boidJ->Location;

			float d = diff.getLengthSQ();
			if ((d > 0) && (d < desiredSeparationSQ))
			{
				target += boidJ->Location;
				count++;
			}
		}

		if (count > 0)
			target /= (float)count;

		if (target.getLengthSQ() > 0.0f) {
			target *= -1.0f;
			target.normalize();
			target *= boidI->MaxSpeed;

			// Steering = target - velocity
			steer = target - boidI->Velocity;

			float force = boidI->MaxForce * timestepSec * weight;
			if (steer.getLengthSQ() > force * force)
			{
				steer.normalize();
				steer *= force;
			}

			boidI->Acceleration += steer;
		}
	}
}

void CBoidSystem::borders(CBoidData** boids, CWorldTransformData** transforms, int numEntity)
{
	// That will turn when near the border
	core::vector3df center, steer;
	CBoidData* boid;

	for (int i = 0; i < numEntity; i++)
	{
		boid = boids[i];
		steer.set(0.0f, 0.0f, 0.0f);

		if (boid->Location.X < m_minX + m_margin ||
			boid->Location.X > m_maxX - m_margin ||
			boid->Location.Z < m_minZ + m_margin ||
			boid->Location.Z > m_maxZ - m_margin)
		{
			steer = center - boid->Location;
			steer.normalize();
			steer *= 0.0003f;

			boid->Acceleration += steer;
		}
	}
}

void CBoidSystem::updateTransform(CBoidData** boids, CWorldTransformData** transforms, int numEntity)
{
	core::vector3df up(0.0f, 1.0f, 0.0f);
	core::vector3df right;
	core::vector3df lastPostion;

	float timestepSec = getTimeStep() * 0.001f;

	float speed;
	CBoidData* boid;
	CWorldTransformData* transform;
	f32* matData;

	for (int i = 0; i < numEntity; i++)
	{
		boid = boids[i];
		transform = transforms[i];

		// update velocity
		boid->Velocity += boid->Acceleration;

		if (boid->Velocity.getLengthSQ() > 0.0f)
		{
			boid->Front = boid->Velocity;
			boid->Front.normalize();
		}

		right = up.crossProduct(boid->Front);
		right.normalize();

		speed = boid->MaxSpeed * timestepSec;
		if (boid->Velocity.getLengthSQ() > speed * speed)
			boid->Velocity = boid->Front * speed;

		lastPostion = boid->Location;

		boid->Location += boid->Velocity;

		// clamp bound
		boid->Location.X = core::clamp(boid->Location.X, m_minX, m_maxX);
		boid->Location.Z = core::clamp(boid->Location.Z, m_minZ, m_maxZ);

		// update velocity after clamp
		boid->Velocity = boid->Location - lastPostion;

		// reset the acceleration
		boid->Acceleration.set(0.0f, 0.0f, 0.0f);

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

		matData[8] = boid->Front.X;
		matData[9] = boid->Front.Y;
		matData[10] = boid->Front.Z;
		matData[11] = 0.0f;

		matData[12] = boid->Location.X;
		matData[13] = boid->Location.Y;
		matData[14] = boid->Location.Z;
		matData[15] = 1.0f;

		// notify changed for CGroupTransform and CWorldTransformSystem
		transform->HasChanged = true;
	}
}