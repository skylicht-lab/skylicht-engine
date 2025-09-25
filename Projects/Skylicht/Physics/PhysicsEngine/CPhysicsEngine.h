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
#include "CDrawDebug.h"

#ifdef USE_BULLET_PHYSIC_ENGINE
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#endif

namespace Skylicht
{
	namespace Physics
	{
		class CRigidbody;
		class CCharacterController;

		struct SRigidbodyData
		{
			CTransform* Transform;
			CRigidbody* Body;
#ifdef USE_BULLET_PHYSIC_ENGINE
			btRigidBody* BulletBody;
#endif
		};

		struct SCharacterData
		{
			CTransform* Transform;
			CCharacterController* Controller;
#ifdef USE_BULLET_PHYSIC_ENGINE
			btKinematicCharacterController* BulletCharacter;
			btPairCachingGhostObject* GhostObject;
#endif
		};

		class CPhysicsEngine
		{
			friend class CRigidbody;
			friend class CCharacterController;

		public:
			DECLARE_SINGLETON(CPhysicsEngine);

			bool IsInEditor;

		protected:
#ifdef USE_BULLET_PHYSIC_ENGINE
			btOverlappingPairCallback* m_overlapCB;
			btBroadphaseInterface* m_broadphase;
			btCollisionDispatcher* m_dispatcher;
			btConstraintSolver* m_solver;
			btDefaultCollisionConfiguration* m_collisionConfiguration;
			btDiscreteDynamicsWorld* m_dynamicsWorld;

			CDrawDebug* m_drawDebug;
#endif
			float m_gravity;

			core::array<SRigidbodyData*> m_bodies;
			core::array<SCharacterData*> m_characters;

			bool m_enableDrawDebug;

		public:
			CPhysicsEngine();

			virtual ~CPhysicsEngine();

			void initPhysics();

			void exitPhysics();

			bool isInitialized();

			void updatePhysics(float timestepSec, int step = 1);

			void syncTransformToPhysics();

			void debugDrawWorld();

			void enableDrawDebug(bool b, bool alwayDraw = false);

			inline float getGravity()
			{
				return m_gravity;
			}

			void setGravity(float g);

			void updateAABBs();

			bool rayTest(const core::vector3df& from, const core::vector3df& to, SAllRaycastResult& result);

			bool rayTest(const core::vector3df& from, const core::vector3df& to, SClosestRaycastResult& result);

			core::array<SRigidbodyData*>& getBodies()
			{
				return m_bodies;
			}

			core::array<SCharacterData*>& getCharacters()
			{
				return m_characters;
			}

		private:

#ifdef USE_BULLET_PHYSIC_ENGINE
			inline btDiscreteDynamicsWorld* getDynamicsWorld()
			{
				return m_dynamicsWorld;
			}
#endif
			void addBody(CRigidbody* body);

			void removeBody(CRigidbody* body);

			void addCharacter(CCharacterController* character);

			void removeCharacter(CCharacterController* character);

			void syncTransforms();

			void checkCollision();
		};
	}
}