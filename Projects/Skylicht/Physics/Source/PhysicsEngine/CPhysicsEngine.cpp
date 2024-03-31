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
#include "CPhysicsEngine.h"
#include "RigidBody/CRigidbody.h"
#include "GameObject/CGameObject.h"

#ifdef USE_BULLET_PHYSIC_ENGINE
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"
#endif

namespace Skylicht
{
	namespace Physics
	{
		IMPLEMENT_SINGLETON(CPhysicsEngine);

		CPhysicsEngine::CPhysicsEngine() :
			m_gravity(-10.0f)
#ifdef USE_BULLET_PHYSIC_ENGINE
			, m_broadphase(NULL),
			m_dispatcher(NULL),
			m_solver(NULL),
			m_collisionConfiguration(NULL),
			m_dynamicsWorld(NULL)
#endif
		{

		}

		CPhysicsEngine::~CPhysicsEngine()
		{
			exitPhysics();
		}

		void CPhysicsEngine::initPhysics()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			// collision configuration contains default setup for memory, collision setup. 
			// Advanced users can create their own configuration.
			m_collisionConfiguration = new btDefaultCollisionConfiguration();

			// use the default collision dispatcher. For parallel processing you can use a diffent dispatcher 
			// (see Extras/BulletMultiThreaded)
			m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

			// btDbvtBroadphase is a good general purpose broadphase. 
			// You can also try out btAxis3Sweep.
			m_broadphase = new btDbvtBroadphase();

			// the default constraint solver. 
			// For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
			m_solver = new btSequentialImpulseConstraintSolver();

			m_dynamicsWorld = new btDiscreteDynamicsWorld(
				m_dispatcher,
				m_broadphase,
				m_solver,
				m_collisionConfiguration);

			m_dynamicsWorld->setGravity(btVector3(0.0f, m_gravity, 0.0f));
#endif
		}

		void CPhysicsEngine::exitPhysics()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_dynamicsWorld)
			{
				delete m_dynamicsWorld;
				delete m_solver;
				delete m_broadphase;
				delete m_dispatcher;
				delete m_collisionConfiguration;

				m_dynamicsWorld = NULL;
				m_solver = NULL;
				m_broadphase = NULL;
				m_dispatcher = NULL;
				m_collisionConfiguration = NULL;
			}
#endif
		}

		void CPhysicsEngine::addBody(CRigidbody* body)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			SRigidbodyData* b = new SRigidbodyData();
			b->Body = body;
			b->BulletBody = body->getBody();
			b->Transform = body->getGameObject()->getTransformMatrix();

			m_bodies.push_back(b);

			// add rigid body
			m_dynamicsWorld->addRigidBody(b->BulletBody);
#endif
		}

		void CPhysicsEngine::removeBody(CRigidbody* body)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			SRigidbodyData** bodies = m_bodies.pointer();
			int used = (int)m_bodies.size();

			int id = -1;
			for (int i = 0; i < used; i++)
			{
				if (m_bodies[i]->Body == body)
				{
					id = i;
					break;
				}
			}

			if (id >= 0)
			{
				// drop rigid body
				m_dynamicsWorld->removeRigidBody(bodies[id]->BulletBody);

				delete bodies[id];

				// move last body to id
				if (used >= 2)
					bodies[id] = bodies[used - 1];

				// remove last
				m_bodies.set_used(used - 1);
			}
#endif
		}

		void CPhysicsEngine::updatePhysics(float timestepSec)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_dynamicsWorld)
			{
				m_dynamicsWorld->stepSimulation(timestepSec);

				syncTransforms();

				checkCollision();
			}
#endif
		}

		void CPhysicsEngine::syncTransforms()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			SRigidbodyData** bodies = m_bodies.pointer();
			int used = (int)m_bodies.size();
			core::matrix4 world;

#ifdef BT_USE_NEON
			float ptr[16] __attribute__((aligned(16)));
#endif

			for (int i = 0; i < used; i++)
			{
				btRigidBody* body = bodies[i]->BulletBody;
				CRigidbody* engineBody = bodies[i]->Body;

				if (body->isStaticOrKinematicObject() && !engineBody->needUpdateTransform())
				{
					// statis kinematic just update when needUpdateTransform == true
					engineBody->notifyUpdateTransform(false);
					continue;
				}

				btTransform& transform = body->getWorldTransform();

#ifdef BT_USE_NEON
				transform.getOpenGLMatrix(ptr);

				float* m = world.pointer();
				for (int i = 0; i < 16; i++)
					m[i] = ptr[i];
#else
				f32* ptr = world.pointer();
				transform.getOpenGLMatrix(ptr);
#endif

				bodies[i]->Transform->setWorldMatrix(world);
			}
#endif
		}

		void CPhysicsEngine::checkCollision()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			SCollisionContactPoint contactData[MANIFOLD_CACHE_SIZE];

			btDispatcher* dispathcher = m_dynamicsWorld->getDispatcher();
			int numManifolds = dispathcher->getNumManifolds();

			for (int i = 0; i < numManifolds; i++)
			{
				btPersistentManifold* contactManifold = dispathcher->getManifoldByIndexInternal(i);
				btCollisionObject* obA = (btCollisionObject*)contactManifold->getBody0();
				btCollisionObject* obB = (btCollisionObject*)contactManifold->getBody1();

				CRigidbody* bodyA = (CRigidbody*)obA->getUserPointer();
				CRigidbody* bodyB = (CRigidbody*)obB->getUserPointer();

				if (bodyA == NULL || bodyB == NULL)
					continue;

				if (bodyA->OnCollision == nullptr && bodyB->OnCollision == nullptr)
					continue;

				int numContacts = contactManifold->getNumContacts();
				if (numContacts == 0)
					continue;

				for (int j = 0; j < numContacts; j++)
				{
					btManifoldPoint& pt = contactManifold->getContactPoint(j);
					if (pt.getDistance() < 0.f)
					{
						const btVector3& ptA = pt.getPositionWorldOnA();
						const btVector3& ptB = pt.getPositionWorldOnB();
						const btVector3& normalOnB = pt.m_normalWorldOnB;

						contactData[j].PositionWorldOnA = Bullet::bulletVectorToIrrVector(ptA);
						contactData[j].PositionWorldOnB = Bullet::bulletVectorToIrrVector(ptB);
						contactData[j].NormalWorldOnB = Bullet::bulletVectorToIrrVector(normalOnB);
					}
				}

				// invoke onCollision
				if (bodyA->OnCollision != nullptr)
					bodyA->OnCollision(bodyA, bodyB, contactData, numContacts);

				if (bodyB->OnCollision != nullptr)
					bodyB->OnCollision(bodyA, bodyB, contactData, numContacts);
			}
#endif
		}

		void CPhysicsEngine::setGravity(float g)
		{
			m_gravity = g;
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_dynamicsWorld)
			{
				m_dynamicsWorld->setGravity(btVector3(0.0f, m_gravity, 0.0f));
			}
#endif
		}

		void CPhysicsEngine::updateAABBs()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_dynamicsWorld)
			{
				m_dynamicsWorld->updateAabbs();
				m_dynamicsWorld->computeOverlappingPairs();
			}
#endif
		}

		bool CPhysicsEngine::rayTest(const core::vector3df& from, const core::vector3df& to, SAllRaycastResult& result)
		{
			bool ret = false;
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_dynamicsWorld)
			{
				btVector3 btfrom(from.X, from.Y, from.Z);
				btVector3 btto(to.X, to.Y, to.Z);

				btCollisionWorld::AllHitsRayResultCallback allResults(btfrom, btto);

				allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;

				m_dynamicsWorld->rayTest(btfrom, btto, allResults);

				ret = allResults.hasHit();

				int numHit = allResults.m_collisionObjects.size();
				for (int i = 0; i < numHit; i++)
				{
					const btCollisionShape* shape = allResults.m_collisionObjects[i]->getCollisionShape();
					if (shape)
					{
						CCollider* collider = (CCollider*)shape->getUserPointer();
						CRigidbody* body = (CRigidbody*)allResults.m_collisionObjects[i]->getUserPointer();

						result.Colliders.push_back(collider);
						result.Bodies.push_back(body);
						result.HitFractions.push_back(allResults.m_hitFractions[i]);
						result.HitNormalWorld.push_back(Bullet::bulletVectorToIrrVector(allResults.m_hitNormalWorld[i]));
						result.HitPointWorld.push_back(Bullet::bulletVectorToIrrVector(allResults.m_hitPointWorld[i]));

						result.RayFromWorld = Bullet::bulletVectorToIrrVector(allResults.m_rayFromWorld);
						result.RayToWorld = Bullet::bulletVectorToIrrVector(allResults.m_rayToWorld);
					}
				}
			}
#endif
			return ret;
		}

		bool CPhysicsEngine::rayTest(const core::vector3df& from, const core::vector3df& to, SClosestRaycastResult& result)
		{
			bool ret = false;
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_dynamicsWorld)
			{
				btVector3 btfrom(from.X, from.Y, from.Z);
				btVector3 btto(to.X, to.Y, to.Z);

				btCollisionWorld::ClosestRayResultCallback closestResults(btfrom, btto);

				m_dynamicsWorld->rayTest(btfrom, btto, closestResults);

				if (closestResults.m_collisionObject)
				{
					ret = true;

					const btCollisionShape* shape = closestResults.m_collisionObject->getCollisionShape();
					if (shape)
					{
						result.Collider = (CCollider*)shape->getUserPointer();
						result.Body = (CRigidbody*)closestResults.m_collisionObject->getUserPointer();
						result.ClosestHitFraction = closestResults.m_closestHitFraction;
						result.HitNormalWorld = Bullet::bulletVectorToIrrVector(closestResults.m_hitNormalWorld);
						result.HitPointWorld = Bullet::bulletVectorToIrrVector(closestResults.m_hitPointWorld);
						result.RayFromWorld = Bullet::bulletVectorToIrrVector(closestResults.m_rayFromWorld);
						result.RayToWorld = Bullet::bulletVectorToIrrVector(closestResults.m_rayToWorld);
					}
				}
			}
#endif
			return ret;
		}
	}
}