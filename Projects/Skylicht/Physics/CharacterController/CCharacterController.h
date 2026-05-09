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

#include "PhysicsEngine/ICollisionObject.h"

#ifdef USE_BULLET_PHYSIC_ENGINE
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#endif

namespace Skylicht
{
	namespace Physics
	{
		/**
		 * @brief A kinematic character controller for handling player movement.
		 * @ingroup Physics
		 * 
		 * This component provides functionality for walking, jumping, and interacting with the physics world
		 * through a ghost object and a kinematic controller (btKinematicCharacterController).
		 * 
		 * Example: Setting up a player character
		 * @code
		 * CGameObject* playerObj = zone->createEmptyObject();
		 * 
		 * float radius = 0.5f;
		 * float height = 1.8f;
		 * 
		 * // 1. Add capsule collider
		 * Physics::CCapsuleCollider* collider = playerObj->addComponent<Physics::CCapsuleCollider>();
		 * collider->setCapsule(radius, height);
		 * 
		 * // 2. Add and init CharacterController
		 * Physics::CCharacterController* controller = playerObj->addComponent<Physics::CCharacterController>();
		 * controller->initCharacter(0.3f); // stepHeight
		 * 
		 * // 3. Move character (in update loop)
		 * core::vector3df moveDir(1.0f, 0.0f, 0.0f);
		 * float speed = 5.0f;
		 * float deltaTime = getTimeStep() / 1000.0f;
		 * controller->setWalkDirection(moveDir * speed * deltaTime);
		 * @endcode
		 */
		class CCharacterController : public ICollisionObject
		{
			friend class CPhysicsEngine;
		protected:
#ifdef USE_BULLET_PHYSIC_ENGINE
			btPairCachingGhostObject* m_ghostObject;
			btKinematicCharacterController* m_controller;
			btConvexShape* m_shape;
#endif
		public:
			CCharacterController();

			virtual ~CCharacterController();

			virtual void initComponent();

			virtual void startComponent();

			virtual void updateComponent();

			virtual void setCollisionGroupAndFilter(int group, int filter);

			/**
			 * @brief Initializes the character controller in the physics world.
			 * @param stepHeight The maximum height of steps the character can walk over.
			 * @return True if successful, false otherwise.
			 */
			bool initCharacter(float stepHeight);

			/**
			 * @brief Releases the character controller from the physics world.
			 */
			void releaseCharacter();

			/**
			 * @brief Gets the current world position of the character.
			 * @return Position vector.
			 */
			core::vector3df getPosition();

			/**
			 * @brief Sets the world position of the character.
			 * @param pos Position vector.
			 */
			void setPosition(const core::vector3df& pos);

			/**
			 * @brief Gets the world rotation as Euler angles.
			 * @return Euler angles in degrees.
			 */
			core::vector3df getRotationEuler();

			/**
			 * @brief Gets the world rotation as a quaternion.
			 * @return Rotation quaternion.
			 */
			core::quaternion getRotation();

			/**
			 * @brief Sets the world rotation using Euler angles.
			 * @param eulerDeg Euler angles in degrees.
			 */
			void setRotation(const core::vector3df& eulerDeg);

			/**
			 * @brief Sets the world rotation using a quaternion.
			 * @param q Rotation quaternion.
			 */
			void setRotation(const core::quaternion& q);

			/**
			 * @brief Synchronizes the GameObject's transform with the character's physics transform.
			 */
			void syncTransform();

			/**
			 * @brief Sets the direction and speed the character should walk.
			 * @param walk Velocity vector for walking.
			 */
			void setWalkDirection(const core::vector3df& walk);

			/**
			 * @brief Checks if the character is in a state where it can jump.
			 * @return True if character can jump.
			 */
			bool canJump();

			/**
			 * @brief Checks if the character is currently on the ground.
			 * @return True if on ground.
			 */
			bool onGround();

			/**
			 * @brief Sets the maximum slope the character can walk up.
			 * @param slopeRadians Slope angle in radians.
			 */
			void setMaxSlope(float slopeRadians);

			/**
			 * @brief Gets the maximum slope angle.
			 * @return Slope angle in radians.
			 */
			float getMaxSlope() const;

			/**
			 * @brief Sets the maximum depth the character can penetrate other objects before collision response.
			 * @param d Penetration depth.
			 */
			void setMaxPenetrationDepth(float d);

			/**
			 * @brief Gets the maximum penetration depth.
			 * @return Penetration depth.
			 */
			float getMaxPenetrationDepth() const;

			/**
			 * @brief Resets the character controller's state and position in the dynamics world.
			 */
			void reset();

			/**
			 * @brief Sets the step height for the character.
			 * @param h Step height.
			 */
			void setStepHeight(float h);

			/**
			 * @brief Gets the current step height.
			 * @return Step height.
			 */
			float getStepHeight() const;

			/**
			 * @brief Sets the maximum falling speed.
			 * @param fallSpeed Fall speed value.
			 */
			void setFallSpeed(float fallSpeed);

			/**
			 * @brief Gets the maximum falling speed.
			 * @return Fall speed value.
			 */
			float getFallSpeed() const;

			/**
			 * @brief Sets the initial speed when jumping.
			 * @param jumpSpeed Jump speed value.
			 */
			void setJumpSpeed(float jumpSpeed);

			/**
			 * @brief Gets the initial jump speed.
			 * @return Jump speed value.
			 */
			float getJumpSpeed() const;

			/**
			 * @brief Sets the maximum height the character can jump.
			 * @param maxJumpHeight Max jump height.
			 */
			void setMaxJumpHeight(float maxJumpHeight);

			/**
			 * @brief Triggers a jump with an optional initial velocity.
			 * @param v Initial jump velocity (optional).
			 */
			void jump(const core::vector3df& v);

			/**
			 * @brief Sets the custom gravity for this character.
			 * @param gravity Gravity vector.
			 */
			void setGravity(const core::vector3df& gravity);

			/**
			 * @brief Gets the custom gravity of the character.
			 * @return Gravity vector.
			 */
			core::vector3df getGravity() const;

			/**
			 * @brief Applies an instantaneous impulse (wraps jump).
			 * @param v Impulse vector.
			 */
			inline void applyImpulse(const core::vector3df& v)
			{
				jump(v);
			}

			/**
			 * @brief Sets the angular velocity of the character.
			 * @param velocity Angular velocity vector.
			 */
			void setAngularVelocity(const core::vector3df& velocity);

			/**
			 * @brief Gets the angular velocity.
			 * @return Angular velocity vector.
			 */
			core::vector3df getAngularVelocity() const;

			/**
			 * @brief Sets the linear velocity of the character.
			 * @param velocity Linear velocity vector.
			 */
			void setLinearVelocity(const core::vector3df& velocity);

			/**
			 * @brief Gets the linear velocity.
			 * @return Linear velocity vector.
			 */
			core::vector3df getLinearVelocity() const;

			/**
			 * @brief Sets the linear damping coefficient.
			 * @param d Damping value.
			 */
			void setLinearDamping(float d);

			/**
			 * @brief Gets the linear damping coefficient.
			 * @return Damping value.
			 */
			float getLinearDamping() const;

			/**
			 * @brief Sets the angular damping coefficient.
			 * @param d Damping value.
			 */
			void setAngularDamping(float d);

			/**
			 * @brief Gets the angular damping coefficient.
			 * @return Damping value.
			 */
			float getAngularDamping() const;

			/**
			 * @brief Sets the 'up' direction for the character.
			 * @param up Up direction vector.
			 */
			void setUp(const core::vector3df& up);

			/**
			 * @brief Gets the 'up' direction.
			 * @return Up direction vector.
			 */
			core::vector3df getUp();

		protected:

#ifdef USE_BULLET_PHYSIC_ENGINE
			btKinematicCharacterController* getCharacterController()
			{
				return m_controller;
			}

			btPairCachingGhostObject* getGhostObject()
			{
				return m_ghostObject;
			}
#endif
		};
	}
}