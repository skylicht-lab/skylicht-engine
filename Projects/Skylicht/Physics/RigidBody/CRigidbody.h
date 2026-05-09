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
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "Bullet/CBulletUtils.h"

#include "Collider/CCollider.h"
#endif

namespace Skylicht
{
	namespace Physics
	{
		/**
		 * @brief Represents a physical rigid body in the physics simulation.
		 * @ingroup Physics
		 * 
		 * A rigid body can be dynamic (affected by forces and gravity) or static/kinematic.
		 * It must be attached to a GameObject that also has a CCollider component.
		 * 
		 * Example: Creating a dynamic box
		 * @code
		 * CGameObject* cubeObj = zone->createEmptyObject();
		 * cubeObj->addComponent<CCube>(); // Visual mesh
		 * 
		 * // 1. Add collider
		 * cubeObj->addComponent<Physics::CBoxCollider>();
		 * 
		 * // 2. Add and init Rigidbody
		 * Physics::CRigidbody* body = cubeObj->addComponent<Physics::CRigidbody>();
		 * body->setMass(1.0f);
		 * body->initRigidbody();
		 * 
		 * // 3. Set initial state
		 * body->setPosition(core::vector3df(0.0f, 10.0f, 0.0f));
		 * body->syncTransform();
		 * @endcode
		 * 
		 * Example: Creating a static/kinematic floor
		 * @code
		 * CGameObject* floorObj = zone->createEmptyObject();
		 * floorObj->addComponent<Physics::CStaticPlaneCollider>();
		 * 
		 * Physics::CRigidbody* body = floorObj->addComponent<Physics::CRigidbody>();
		 * body->setDynamic(false); // Static
		 * body->initRigidbody();
		 * @endcode
		 */
		class CRigidbody : public ICollisionObject
		{
			friend class CPhysicsEngine;
		public:

			/**
			 * @brief Activation states for the rigid body.
			 * @ingroup Physics
			 */
			enum EActivationState
			{
				Activate, ///< The body is active and simulating.
				Sleep,    ///< The body is currently inactive (sleeping) to save performance.
				Alway,    ///< The body is always active and will not sleep.
				Disable   ///< Simulation is disabled for this body.
			};

		protected:
			bool m_isDynamic;
			float m_mass;
			float m_restitution;

			float m_friction;
			float m_rollingFriction;
			float m_spinningFriction;

			bool m_needUpdateTransform;

			bool m_drawDebug;

#ifdef USE_BULLET_PHYSIC_ENGINE
			btRigidBody* m_rigidBody;
			btCollisionShape* m_shape;
#endif
			int m_tag;
		public:
			CRigidbody();

			virtual ~CRigidbody();

			virtual void initComponent();

			virtual void startComponent();

			virtual void updateComponent();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			virtual void setCollisionGroupAndFilter(int group, int filter);

			/**
			 * @brief Sets a custom tag for the rigid body.
			 * @param i Tag value.
			 */
			inline void setTag(int i)
			{
				m_tag = i;
			}

			/**
			 * @brief Gets the custom tag of the rigid body.
			 * @return The tag value.
			 */
			inline int getTag()
			{
				return m_tag;
			}

			/**
			 * @brief Sets whether the body is dynamic.
			 * @param b True for dynamic, false for static/kinematic.
			 */
			void setDynamic(bool b);

			/**
			 * @brief Checks if the body is dynamic.
			 * @return True if dynamic, false otherwise.
			 */
			inline bool isDynamic()
			{
				return m_isDynamic;
			}

			/**
			 * @brief Sets the friction coefficient.
			 * @param f Friction value.
			 */
			void setFriction(float f);

			/**
			 * @brief Gets the friction coefficient.
			 * @return Friction value.
			 */
			inline float getFriction()
			{
				return m_friction;
			}

			/**
			 * @brief Sets the rolling friction coefficient.
			 * @param f Rolling friction value.
			 */
			void setRollingFriction(float f);

			/**
			 * @brief Gets the rolling friction coefficient.
			 * @return Rolling friction value.
			 */
			inline float getRollingFriction()
			{
				return m_rollingFriction;
			}

			/**
			 * @brief Sets the spinning friction coefficient.
			 * @param f Spinning friction value.
			 */
			void setSpinningFriction(float f);

			/**
			 * @brief Gets the spinning friction coefficient.
			 * @return Spinning friction value.
			 */
			inline float getSpinningFriction()
			{
				return m_spinningFriction;
			}

			/**
			 * @brief Sets the mass of the rigid body.
			 * @param m Mass value.
			 */
			void setMass(float m);

			/**
			 * @brief Gets the mass of the rigid body.
			 * @return Mass value.
			 */
			inline float getMass()
			{
				return m_mass;
			}

			/**
			 * @brief Sets the restitution (bounciness) coefficient.
			 * @param s Restitution value.
			 */
			void setRestitution(float s);

			/**
			 * @brief Gets the restitution coefficient.
			 * @return Restitution value.
			 */
			inline float getRestitution()
			{
				return m_restitution;
			}

			/**
			 * @brief Sets the thresholds for linear and angular velocity to enter sleep mode.
			 * @param linear Linear velocity threshold.
			 * @param angular Angular velocity threshold.
			 */
			void setSleepingThresholds(float linear, float angular);

			/**
			 * @brief Sets the radius for Continuous Collision Detection (CCD).
			 * @param r Radius value.
			 */
			void setCcdSweptSphereRadius(float r = 0.0f);

			/**
			 * @brief Sets the motion threshold for Continuous Collision Detection (CCD).
			 * @param m Motion threshold value.
			 */
			void setCcdMotionThreshold(float m = 0.0f);

			/**
			 * @brief Initializes the rigid body in the physics world.
			 * @return True if successful, false otherwise.
			 */
			bool initRigidbody();

			/**
			 * @brief Releases the rigid body from the physics world.
			 */
			void releaseRigidbody();

			/**
			 * @brief Sets the local scale of the collision shape.
			 * @param scale Scale vector.
			 */
			void setLocalScale(const core::vector3df& scale);

			/**
			 * @brief Gets the local scale of the collision shape.
			 * @return Scale vector.
			 */
			core::vector3df getLocalScale();

			/**
			 * @brief Gets the world position of the rigid body.
			 * @return Position vector.
			 */
			core::vector3df getPosition();

			/**
			 * @brief Sets the world position of the rigid body.
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
			 * @brief Synchronizes the GameObject's transform with the physics body's transform.
			 */
			void syncTransform();

			/**
			 * @brief Sets the activation state of the rigid body.
			 * @param state New activation state.
			 */
			void setState(EActivationState state);

			/**
			 * @brief Manually activates the rigid body (wakes it up).
			 */
			void activate();

			/**
			 * @brief Gets the current activation state.
			 * @return Current state.
			 */
			EActivationState getState();

			/**
			 * @brief Gets the name of the current activation state as a string.
			 * @return State name string.
			 */
			const char* getStateName();

			/**
			 * @brief Applies a force at the center of mass.
			 * @param force Force vector.
			 */
			void applyCenterForce(const core::vector3df& force);

			/**
			 * @brief Applies a force at a specific local position.
			 * @param force Force vector.
			 * @param localPosition Local offset from the center of mass.
			 */
			void applyForce(const core::vector3df& force, const core::vector3df& localPosition);

			/**
			 * @brief Applies a torque (rotational force).
			 * @param torque Torque vector.
			 */
			void applyTorque(const core::vector3df& torque);

			/**
			 * @brief Clears all active forces and torques.
			 */
			void clearForce();

			/**
			 * @brief Applies an instantaneous impulse at the center of mass.
			 * @param impulse Impulse vector.
			 */
			void applyCenterImpulse(const core::vector3df& impulse);

			/**
			 * @brief Applies an instantaneous impulse at a specific local position.
			 * @param impulse Impulse vector.
			 * @param localPosition Local offset from the center of mass.
			 */
			void applyImpulse(const core::vector3df& impulse, const core::vector3df& localPosition);

			/**
			 * @brief Applies a torque impulse (instantaneous rotation).
			 * @param torqueImpulse Torque impulse vector.
			 */
			void applyTorqueImpulse(const core::vector3df& torqueImpulse);

			/**
			 * @brief Applies a push impulse at the center of mass.
			 * @param impulse Push impulse vector.
			 */
			void applyCenterPushImpulse(const core::vector3df& impulse);

			/**
			 * @brief Applies a push impulse at a specific local position.
			 * @param impulse Push impulse vector.
			 * @param localPosition Local offset from the center of mass.
			 */
			void applyPushImpulse(const core::vector3df& impulse, const core::vector3df& localPosition);

			/**
			 * @brief Applies a torque turn impulse.
			 * @param torqueImpulse Torque impulse vector.
			 */
			void applyTorqueTurnImpulse(const core::vector3df& torqueImpulse);

			/**
			 * @brief Gets the current linear velocity.
			 * @return Velocity vector.
			 */
			core::vector3df getLinearVelocity();

			/**
			 * @brief Gets the current angular velocity.
			 * @return Angular velocity vector.
			 */
			core::vector3df getAngularVelocity();

			/**
			 * @brief Sets the linear velocity.
			 * @param v Velocity vector.
			 */
			void setLinearVelocity(const core::vector3df& v);

			/**
			 * @brief Sets the angular velocity.
			 * @param v Angular velocity vector.
			 */
			void setAngularVelocity(const core::vector3df& v);

			/**
			 * @brief Checks if the transform needs updating.
			 * @return True if update is needed.
			 */
			inline bool needUpdateTransform()
			{
				return m_needUpdateTransform;
			}

			/**
			 * @brief Notifies the component about transform update status.
			 * @param b Update status.
			 */
			inline void notifyUpdateTransform(bool b)
			{
				m_needUpdateTransform = b;
			}

			/**
			 * @brief Enables or disables debug drawing for this specific rigid body.
			 * @param b True to enable, false to disable.
			 */
			inline void setDrawDebug(bool b)
			{
				m_drawDebug = b;
			}

			/**
			 * @brief Checks if debug drawing is enabled for this body.
			 * @return True if enabled.
			 */
			inline bool enableDrawDebug()
			{
				return m_drawDebug;
			}

#ifdef USE_BULLET_PHYSIC_ENGINE
			/**
			 * @brief Gets the underlying Bullet rigid body.
			 * @return Pointer to btRigidBody.
			 */
			inline btRigidBody* getBody()
			{
				return m_rigidBody;
			}

			/**
			 * @brief Gets the underlying Bullet collision shape.
			 * @return Pointer to btCollisionShape.
			 */
			inline btCollisionShape* getShape()
			{
				return m_shape;
			}
#endif
			/**
			 * @brief Gets the current world transform matrix.
			 * @return 4x4 transform matrix.
			 */
			core::matrix4 getWorldTransform();

			DECLARE_GETTYPENAME(CRigidbody)
		};
	}
}