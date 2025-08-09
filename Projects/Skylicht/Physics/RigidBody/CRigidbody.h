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
	/// @brief The namespace contains classes related to physics and colliders.
	namespace Physics
	{
		class CRigidbody : public ICollisionObject
		{
			friend class CPhysicsEngine;
		public:

			enum EActivationState
			{
				Activate,
				Sleep,
				Alway,
				Disable
			};

		protected:
			bool m_isDynamic;
			float m_mass;

			float m_friction;
			float m_rollingFriction;
			float m_spinningFriction;

			bool m_needUpdateTransform;

			bool m_drawDebug;

#ifdef USE_BULLET_PHYSIC_ENGINE
			btRigidBody* m_rigidBody;
			btCollisionShape* m_shape;
#endif

		public:
			CRigidbody();

			virtual ~CRigidbody();

			virtual void initComponent();

			virtual void startComponent();

			virtual void updateComponent();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			void setDynamic(bool b);

			inline bool isDynamic()
			{
				return m_isDynamic;
			}

			void setFriction(float f);

			inline float getFriction()
			{
				return m_friction;
			}

			void setRollingFriction(float f);

			inline float getRollingFriction()
			{
				return m_rollingFriction;
			}

			void setSpinningFriction(float f);

			inline float getSpinningFriction()
			{
				return m_spinningFriction;
			}

			void setMass(float m);

			inline float getMass()
			{
				return m_mass;
			}

			bool initRigidbody();

			void releaseRigidbody();

			void setLocalScale(const core::vector3df& scale);

			core::vector3df getLocalScale();

			core::vector3df getPosition();

			void setPosition(const core::vector3df& pos);

			core::vector3df getRotationEuler();

			core::quaternion getRotation();

			void setRotation(const core::vector3df& eulerDeg);

			void setRotation(const core::quaternion& q);

			void syncTransform();

			void setState(EActivationState state);

			void activate();

			EActivationState getState();

			const char* getStateName();

			void applyCenterForce(const core::vector3df& force);

			void applyForce(const core::vector3df& force, const core::vector3df& localPosition);

			void applyTorque(const core::vector3df& torque);

			void clearForce();

			void applyCenterImpulse(const core::vector3df& impulse);

			void applyImpulse(const core::vector3df& impulse, const core::vector3df& localPosition);

			void applyTorqueImpulse(const core::vector3df& torqueImpulse);

			void applyCenterPushImpulse(const core::vector3df& impulse);

			void applyPushImpulse(const core::vector3df& impulse, const core::vector3df& localPosition);

			void applyTorqueTurnImpulse(const core::vector3df& torqueImpulse);

			inline bool needUpdateTransform()
			{
				return m_needUpdateTransform;
			}

			inline void notifyUpdateTransform(bool b)
			{
				m_needUpdateTransform = b;
			}

			inline void setDrawDebug(bool b)
			{
				m_drawDebug = b;
			}

			inline bool enableDrawDebug()
			{
				return m_drawDebug;
			}

#ifdef USE_BULLET_PHYSIC_ENGINE
			inline btRigidBody* getBody()
			{
				return m_rigidBody;
			}

			inline btCollisionShape* getShape()
			{
				return m_shape;
			}
#endif
			core::matrix4 getWorldTransform();

			DECLARE_GETTYPENAME(CRigidbody)
		};
	}
}