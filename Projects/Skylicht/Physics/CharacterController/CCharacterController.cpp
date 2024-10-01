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

// reference: https://github.com/kripken/bullet/tree/master/Demos/CharacterDemo

#include "pch.h"
#include "CCharacterController.h"
#include "GameObject/CGameObject.h"

#include "Collider/CCollider.h"
#include "PhysicsEngine/CPhysicsEngine.h"

namespace Skylicht
{
	namespace Physics
	{
		ACTIVATOR_REGISTER(CCharacterController);

		CATEGORY_COMPONENT(CCharacterController, "Character Controller", "Physics/Character");

		CCharacterController::CCharacterController()
#ifdef USE_BULLET_PHYSIC_ENGINE
			:m_ghostObject(NULL),
			m_controller(NULL),
			m_shape(NULL)
#endif
		{

		}

		CCharacterController::~CCharacterController()
		{
			releaseCharacter();
		}

		void CCharacterController::initComponent()
		{
			// convert to readonly transform
			m_gameObject->setupMatrixTransform();
		}

		void CCharacterController::startComponent()
		{

		}

		void CCharacterController::updateComponent()
		{

		}

		bool CCharacterController::initCharacter(float stepHeight)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			CTransformMatrix* transform = m_gameObject->getTransformMatrix();
			if (transform == NULL)
				return false;

			CPhysicsEngine* engine = CPhysicsEngine::getInstance();
			if (engine == NULL)
				return false;

			CCollider* collider = m_gameObject->getComponent<CCollider>();
			if (collider == NULL)
				return false;

			if (m_ghostObject && m_shape)
			{
				releaseCharacter();
			}

			// init shape
			m_shape = dynamic_cast<btConvexShape*>(collider->initCollisionShape());
			if (m_shape == NULL)
				return false;

			m_ghostObject = new btPairCachingGhostObject();
			m_ghostObject->setCollisionShape(m_shape);
			m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

			m_controller = new btKinematicCharacterController(
				m_ghostObject,
				m_shape,
				stepHeight,
				btVector3(0.0, 1.0, 0.0));

			engine->addCharacter(this);
			return true;
#else
			return false;
#endif	
		}

		void CCharacterController::releaseCharacter()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			CPhysicsEngine* engine = CPhysicsEngine::getInstance();
			if (engine)
				engine->removeCharacter(this);

			if (m_controller)
			{
				delete m_controller;
				m_controller = NULL;
			}

			if (m_ghostObject)
			{
				delete m_ghostObject;
				m_ghostObject = NULL;
			}

			if (m_shape)
			{
				CCollider* collider = (CCollider*)m_shape->getUserPointer();
				if (collider)
					collider->dropCollisionShape();

				delete m_shape;
				m_shape = NULL;
			}
#endif
		}

		core::vector3df CCharacterController::getPosition()
		{
			core::vector3df position;
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_ghostObject)
			{
				const btTransform& world = m_ghostObject->getWorldTransform();
				const btVector3& origin = world.getOrigin();
				position.X = origin.x();
				position.Y = origin.y();
				position.Z = origin.z();
			}
#endif
			return position;
		}

		void CCharacterController::setPosition(const core::vector3df& pos)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_ghostObject)
			{
				btTransform& world = m_ghostObject->getWorldTransform();
				world.setOrigin(btVector3(pos.X, pos.Y, pos.Z));
			}
#endif
		}

		core::vector3df CCharacterController::getRotationEuler()
		{
			core::vector3df rotation;
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_ghostObject)
			{
				const btTransform& world = m_ghostObject->getWorldTransform();
				btQuaternion q = world.getRotation();

				btVector3 euler;
				Bullet::quaternionToEuler(q, euler);

				rotation.set(euler.x(), euler.y(), euler.z());
			}
#endif
			return rotation;
		}

		core::quaternion CCharacterController::getRotation()
		{
			core::quaternion rotation;
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_ghostObject)
			{
				const btTransform& world = m_ghostObject->getWorldTransform();
				btQuaternion q = world.getRotation();
				rotation.set(q.x(), q.y(), q.z(), q.w());
			}
#endif
			return rotation;
		}

		void CCharacterController::setRotation(const core::vector3df& eulerDeg)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_ghostObject)
			{
				btTransform& world = m_ghostObject->getWorldTransform();
				btQuaternion rot(eulerDeg.Y * core::DEGTORAD,
					eulerDeg.X * core::DEGTORAD,
					eulerDeg.Z * core::DEGTORAD);
				world.setRotation(rot);
			}
#endif
		}

		void CCharacterController::setRotation(const core::quaternion& q)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_ghostObject)
			{
				btTransform& world = m_ghostObject->getWorldTransform();
				btQuaternion rot(q.X, q.Y, q.Z, q.W);
				world.setRotation(rot);
			}
#endif
		}

		void CCharacterController::syncTransform()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_ghostObject)
			{
				const btTransform& world = m_ghostObject->getWorldTransform();
				CTransformMatrix* transform = m_gameObject->getTransformMatrix();

				core::matrix4 mat;
				Bullet::bulletTransformToIrrMatrix(world, mat);

				transform->setWorldMatrix(mat);
			}
#endif
		}

		void CCharacterController::setWalkDirection(const core::vector3df& walk)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->setWalkDirection(btVector3(walk.X, walk.Y, walk.Z));
#endif
		}

		bool CCharacterController::canJump()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return m_controller->canJump();
#else
			return false;
#endif
		}

		bool CCharacterController::onGround()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return m_controller->onGround();
#else
			return false;
#endif
		}

		void CCharacterController::setMaxSlope(float slopeRadians)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->setMaxSlope(slopeRadians);
#endif
		}

		float CCharacterController::getMaxSlope() const
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return m_controller->getMaxSlope();
#else
			return 0.0f;
#endif
		}

		void CCharacterController::setMaxPenetrationDepth(float d)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return m_controller->setMaxPenetrationDepth(d);
#endif
		}

		float CCharacterController::getMaxPenetrationDepth() const
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return m_controller->getMaxSlope();
#else
			return 0.0f;
#endif
		}

		void CCharacterController::reset()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			CPhysicsEngine* engine = CPhysicsEngine::getInstance();
			if (engine != NULL)
				m_controller->reset(engine->getDynamicsWorld());
#endif
		}

		void CCharacterController::setStepHeight(float h)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->setStepHeight(h);
#endif
		}

		float CCharacterController::getStepHeight() const
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return m_controller->getStepHeight();
#else
			return 0.0f;
#endif
		}

		void CCharacterController::setFallSpeed(float fallSpeed)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->setFallSpeed(fallSpeed);
#endif
		}

		float CCharacterController::getFallSpeed() const
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return m_controller->getFallSpeed();
#else
			return 0.0f;
#endif
		}

		void CCharacterController::setJumpSpeed(float jumpSpeed)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->setJumpSpeed(jumpSpeed);
#endif
		}

		float CCharacterController::getJumpSpeed() const
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return m_controller->getJumpSpeed();
#else
			return 0.0f;
#endif
		}

		void CCharacterController::setMaxJumpHeight(float maxJumpHeight)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->setMaxJumpHeight(maxJumpHeight);
#endif
		}

		void CCharacterController::jump(const core::vector3df& v)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->jump(btVector3(v.X, v.Y, v.Z));
#endif
		}

		void CCharacterController::setGravity(const core::vector3df& gravity)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->setGravity(btVector3(gravity.X, gravity.Y, gravity.Z));
#endif
		}

		core::vector3df CCharacterController::getGravity() const
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return Bullet::bulletVectorToIrrVector(m_controller->getGravity());
#else
			return core::vector3df();
#endif
		}

		void CCharacterController::setAngularVelocity(const core::vector3df& velocity)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->setAngularVelocity(btVector3(velocity.X, velocity.Y, velocity.Z));
#endif
		}

		core::vector3df CCharacterController::getAngularVelocity() const
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return Bullet::bulletVectorToIrrVector(m_controller->getAngularVelocity());
#else
			return core::vector3df();
#endif
		}

		void CCharacterController::setLinearVelocity(const core::vector3df& velocity)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->setLinearVelocity(btVector3(velocity.X, velocity.Y, velocity.Z));
#endif
		}

		core::vector3df CCharacterController::getLinearVelocity() const
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return Bullet::bulletVectorToIrrVector(m_controller->getLinearVelocity());
#else
			return core::vector3df();
#endif
		}

		void CCharacterController::setLinearDamping(float d)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->setLinearDamping(d);
#endif
		}

		float CCharacterController::getLinearDamping() const
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return m_controller->getLinearDamping();
#else
			return 0.0f;
#endif
		}

		void CCharacterController::setAngularDamping(float d)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->setAngularDamping(d);
#endif
		}

		float CCharacterController::getAngularDamping() const
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return m_controller->getAngularDamping();
#else
			return 0.0f;
#endif
		}

		void CCharacterController::setUp(const core::vector3df& up)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_controller->setUp(btVector3(up.X, up.Y, up.Z));
#endif
		}

		core::vector3df CCharacterController::getUp()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			return Bullet::bulletVectorToIrrVector(m_controller->getUp());
#else
			return core::vector3df(0.0f, 1.0f, 0.0f);
#endif
		}
	}
}