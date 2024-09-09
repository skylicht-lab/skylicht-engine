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

#pragma once

#include "Utils/CSingleton.h"
#include "Transform/CTransformMatrix.h"
#include "CPhysicsRaycast.h"

#ifdef USE_BULLET_PHYSIC_ENGINE
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#endif

namespace Skylicht
{
	namespace Physics
	{
		class CRigidbody;

		struct SRigidbodyData
		{
			CRigidbody* Body;
			CTransformMatrix* Transform;
#ifdef USE_BULLET_PHYSIC_ENGINE
			btRigidBody* BulletBody;
#endif
		};

		class CPhysicsEngine
		{
			friend class CRigidbody;
		public:
			DECLARE_SINGLETON(CPhysicsEngine);

		protected:
#ifdef USE_BULLET_PHYSIC_ENGINE
			btBroadphaseInterface* m_broadphase;
			btCollisionDispatcher* m_dispatcher;
			btConstraintSolver* m_solver;
			btDefaultCollisionConfiguration* m_collisionConfiguration;
			btDiscreteDynamicsWorld* m_dynamicsWorld;
#endif
			float m_gravity;

			core::array<SRigidbodyData*> m_bodies;

		public:
			CPhysicsEngine();

			virtual ~CPhysicsEngine();

			void initPhysics();

			void exitPhysics();

			void updatePhysics(float timestepSec);

			inline float getGravity()
			{
				return m_gravity;
			}

			void setGravity(float g);

			void updateAABBs();

			bool rayTest(const core::vector3df& from, const core::vector3df& to, SAllRaycastResult& result);

			bool rayTest(const core::vector3df& from, const core::vector3df& to, SClosestRaycastResult& result);

		private:

#ifdef USE_BULLET_PHYSIC_ENGINE
			inline btDiscreteDynamicsWorld* getDynamicsWorld()
			{
				return m_dynamicsWorld;
			}
#endif
			void addBody(CRigidbody* body);

			void removeBody(CRigidbody* body);

			void syncTransforms();

			void checkCollision();
		};
	}
}