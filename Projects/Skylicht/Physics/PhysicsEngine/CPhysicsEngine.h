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

		/**
		 * @brief Data structure for managing rigid body within the physics engine.
		 * @ingroup Physics
		 */
		struct SRigidbodyData
		{
			CTransform* Transform;
			CRigidbody* Body;
#ifdef USE_BULLET_PHYSIC_ENGINE
			btRigidBody* BulletBody;
#endif
		};

		/**
		 * @brief Data structure for managing character controller within the physics engine.
		 * @ingroup Physics
		 */
		struct SCharacterData
		{
			CTransform* Transform;
			CCharacterController* Controller;
#ifdef USE_BULLET_PHYSIC_ENGINE
			btKinematicCharacterController* BulletCharacter;
			btPairCachingGhostObject* GhostObject;
#endif
		};

		/**
		 * @brief The main physics engine manager.
		 * @ingroup Physics
		 * 
		 * This singleton class orchestrates the physics simulation, manages collision objects,
		 * and provides raycasting utilities. It wraps the Bullet Physics engine.
		 * 
		 * Example: Initializing the physics engine
		 * @code
		 * // Initialize the physics engine singleton
		 * Physics::CPhysicsEngine::createGetInstance();
		 * 
		 * // Initialize the physics world (usually in your ViewInit or similar)
		 * Physics::CPhysicsEngine::getInstance()->initPhysics();
		 * 
		 * // Set gravity
		 * Physics::CPhysicsEngine::getInstance()->setGravity(-9.81f);
		 * 
		 * // In your update loop
		 * float timestepSec = getTimeStep() / 1000.0f;
		 * Physics::CPhysicsEngine::getInstance()->updatePhysics(timestepSec);
		 * @endcode
		 * 
		 * Example: Performing a raycast
		 * @code
		 * Physics::CPhysicsEngine* physicsEngine = Physics::CPhysicsEngine::getInstance();
		 * Physics::SClosestRaycastResult result;
		 * 
		 * core::vector3df from = core::vector3df(0, 10, 0);
		 * core::vector3df to = core::vector3df(0, -10, 0);
		 * 
		 * if (physicsEngine->rayTest(from, to, result))
		 * {
		 *     if (result.Body)
		 *     {
		 *         // Hit a rigid body
		 *         result.Body->applyCenterImpulse(core::vector3df(0, 5, 0));
		 *     }
		 * }
		 * @endcode
		 */
		class CPhysicsEngine
		{
			friend class CRigidbody;
			friend class CCharacterController;

		public:
			DECLARE_SINGLETON(CPhysicsEngine);

			/**
			 * @brief Indicates if the engine is running within the editor.
			 */
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

			/**
			 * @brief Initializes the physics world and configuration.
			 */
			void initPhysics();

			/**
			 * @brief Cleans up and releases physics world resources.
			 */
			void exitPhysics();

			/**
			 * @brief Checks if the physics engine has been initialized.
			 * @return True if initialized, false otherwise.
			 */
			bool isInitialized();

			/**
			 * @brief Steps the physics simulation.
			 * @param timestepSec Time elapsed since the last frame in seconds.
			 * @param step Maximum number of sub-steps (default: 1).
			 */
			void updatePhysics(float timestepSec, int step = 1);

			/**
			 * @brief Synchronizes Skylicht transforms to the physics engine.
			 */
			void syncTransformToPhysics();

			/**
			 * @brief Draws the physics world for debugging.
			 */
			void debugDrawWorld();

			/**
			 * @brief Enables or disables physics debug visualization.
			 * @param b True to enable, false to disable.
			 * @param alwayDraw If true, debug lines are drawn even if objects are inactive.
			 */
			void enableDrawDebug(bool b, bool alwayDraw = false);

			/**
			 * @brief Gets the current gravity value.
			 * @return The gravity on the Y-axis.
			 */
			inline float getGravity()
			{
				return m_gravity;
			}

			/**
			 * @brief Sets the gravity on the Y-axis.
			 * @param g Gravity value.
			 */
			void setGravity(float g);

			/**
			 * @brief Updates the Axis-Aligned Bounding Boxes (AABBs) for all objects.
			 */
			void updateAABBs();

			/**
			 * @brief Performs a raycast test returning all hits.
			 * @param from Ray start position.
			 * @param to Ray end position.
			 * @param result Structure to store all hit results.
			 * @return True if any hits were found.
			 */
			bool rayTest(const core::vector3df& from, const core::vector3df& to, SAllRaycastResult& result);

			/**
			 * @brief Performs a raycast test returning only the closest hit.
			 * @param from Ray start position.
			 * @param to Ray end position.
			 * @param result Structure to store the closest hit result.
			 * @return True if a hit was found.
			 */
			bool rayTest(const core::vector3df& from, const core::vector3df& to, SClosestRaycastResult& result);

			/**
			 * @brief Gets all registered rigid bodies.
			 * @return Array of rigid body data.
			 */
			core::array<SRigidbodyData*>& getBodies()
			{
				return m_bodies;
			}

			/**
			 * @brief Gets all registered character controllers.
			 * @return Array of character controller data.
			 */
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