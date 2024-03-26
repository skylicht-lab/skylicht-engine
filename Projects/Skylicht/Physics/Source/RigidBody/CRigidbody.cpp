#include "pch.h"
#include "CRigidbody.h"

#include "GameObject/CGameObject.h"

#include "Collider/CCollider.h"
#include "Collider/CBoxCollider.h"
#include "Collider/CStaticPlaneCollider.h"
#include "Collider/CMeshCollider.h"

#include "PhysicsEngine/CPhysicsEngine.h"

namespace Skylicht
{
	namespace Physics
	{
		ACTIVATOR_REGISTER(CRigidbody);

		CATEGORY_COMPONENT(CRigidbody, "Rigidbody", "Physics");

		CRigidbody::CRigidbody() :
			m_mass(1.0f),
			m_isDynamic(true),
			m_needUpdateTransform(true)
#ifdef USE_BULLET_PHYSIC_ENGINE
			, m_shape(NULL),
			m_rigidBody(NULL)
#endif
		{

		}

		CRigidbody::~CRigidbody()
		{
			releaseRigidbody();
		}

		void CRigidbody::initComponent()
		{
			// convert to readonly transform
			m_gameObject->setupMatrixTransform();
		}

		void CRigidbody::startComponent()
		{
			initRigidbody();
		}

		void CRigidbody::updateComponent()
		{

		}

		void CRigidbody::setDynamic(bool b)
		{
			m_isDynamic = b;

#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				float mass = m_mass;
				if (!m_isDynamic)
					mass = 0.0f;

				btVector3 inertia;
				m_rigidBody->getCollisionShape()->calculateLocalInertia(mass, inertia);
				m_rigidBody->setMassProps(mass, inertia);
			}
#endif
		}

		void CRigidbody::setMass(float m)
		{
			m_mass = m;

#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				btVector3 localInertia(0, 0, 0);
				m_rigidBody->setMassProps(m_mass, localInertia);
			}
#endif
		}

		bool CRigidbody::initRigidbody()
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

			if (m_rigidBody && m_shape)
			{
				releaseRigidbody();
			}

			// init shape
			m_shape = collider->initCollisionShape();
			if (m_shape == NULL)
				return false;

			btTransform startTransform;

			const f32* matrix = transform->calcWorldTransform().pointer();
			startTransform.setFromOpenGLMatrix(matrix);

			btVector3 localInertia(0, 0, 0);
			float mass = m_mass;

			if (m_isDynamic)
				m_shape->calculateLocalInertia(m_mass, localInertia);
			else
				mass = 0.0f;

			btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, m_shape, localInertia);

			m_rigidBody = new btRigidBody(rbInfo);
			m_rigidBody->setUserPointer(this);
			m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

			// add rigid body
			engine->addBody(this);

			// Transform component isnot in relative in Parent
			// It is sync from Physics Engine
			transform->setIsWorldTransform(true);

			return true;
#else
			return false;
#endif
		}

		void CRigidbody::releaseRigidbody()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			CPhysicsEngine* engine = CPhysicsEngine::getInstance();
			if (engine)
				engine->removeBody(this);

			if (m_rigidBody)
			{
				delete m_rigidBody->getMotionState();
				delete m_rigidBody;
				m_rigidBody = NULL;
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

		void CRigidbody::syncTransform()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				const btTransform& world = m_rigidBody->getWorldTransform();
				CTransformMatrix* transform = m_gameObject->getTransformMatrix();

				core::matrix4 mat;
				Bullet::bulletTransformToIrrMatrix(world, mat);

				transform->setWorldMatrix(mat);
			}
#endif
		}

		core::vector3df CRigidbody::getPosition()
		{
			core::vector3df position;
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				const btTransform& world = m_rigidBody->getWorldTransform();
				const btVector3& origin = world.getOrigin();
				position.X = origin.x();
				position.Y = origin.y();
				position.Z = origin.z();
			}
#endif
			return position;
		}

		void CRigidbody::setPosition(const core::vector3df& pos)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				btTransform& world = m_rigidBody->getWorldTransform();
				world.setOrigin(btVector3(pos.X, pos.Y, pos.Z));
			}
#endif
			m_needUpdateTransform = true;
		}

		core::vector3df CRigidbody::getRotationEuler()
		{
			core::vector3df rotation;
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				const btTransform& world = m_rigidBody->getWorldTransform();
				btQuaternion q = world.getRotation();

				btVector3 euler;
				Bullet::quaternionToEuler(q, euler);

				rotation.set(euler.x(), euler.y(), euler.z());
			}
#endif
			return rotation;
		}

		core::quaternion CRigidbody::getRotation()
		{
			core::quaternion rotation;
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				const btTransform& world = m_rigidBody->getWorldTransform();
				btQuaternion q = world.getRotation();
				rotation.set(q.x(), q.y(), q.z(), q.w());
			}
#endif
			return rotation;
		}

		void CRigidbody::setRotation(const core::vector3df& eulerDeg)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				btTransform& world = m_rigidBody->getWorldTransform();
				btQuaternion rot(eulerDeg.Y * core::DEGTORAD,
					eulerDeg.X * core::DEGTORAD,
					eulerDeg.Z * core::DEGTORAD);
				world.setRotation(rot);
			}
#endif
			m_needUpdateTransform = true;
		}

		void CRigidbody::setRotation(const core::quaternion& q)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				btTransform& world = m_rigidBody->getWorldTransform();
				btQuaternion rot(q.X, q.Y, q.Z, q.W);
				world.setRotation(rot);
			}
#endif
			m_needUpdateTransform = true;
		}

		void CRigidbody::setState(EActivationState state)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				switch (state)
				{
				case Activate:
					m_rigidBody->setActivationState(ACTIVE_TAG);
					break;
				case Sleep:
					m_rigidBody->setActivationState(ISLAND_SLEEPING);
					break;
				case Alway:
					m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
					break;
				case Disable:
					m_rigidBody->setActivationState(DISABLE_SIMULATION);
					break;
				}
			}
#endif
			m_needUpdateTransform = true;
		}

		CRigidbody::EActivationState CRigidbody::getState()
		{
			int state = m_rigidBody->getActivationState();
			if (state == ACTIVE_TAG)
				return Activate;
			else if (state == ISLAND_SLEEPING)
				return Sleep;
			else if (state == DISABLE_DEACTIVATION)
				return Alway;
			else if (state == DISABLE_SIMULATION)
				return Disable;
		}

		const char* CRigidbody::getStateName()
		{
			int state = m_rigidBody->getActivationState();
			if (state == ACTIVE_TAG)
				return "Activate";
			else if (state == ISLAND_SLEEPING)
				return "Sleep";
			else if (state == WANTS_DEACTIVATION)
				return "Wants Deactivation";
			else if (state == DISABLE_DEACTIVATION)
				return "Alway";
			else if (state == DISABLE_SIMULATION)
				return "Disable";
			return "Unknown";
		}

		void CRigidbody::applyCenterForce(const core::vector3df& force)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->applyCentralForce(btVector3(force.X, force.Y, force.Z));
			}
#endif
		}

		void CRigidbody::applyCenterImpulse(const core::vector3df& impulse)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->applyCentralImpulse(btVector3(impulse.X, impulse.Y, impulse.Z));
			}
#endif
		}

		void CRigidbody::applyCenterPushImpulse(const core::vector3df& impulse)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->applyCentralPushImpulse(btVector3(impulse.X, impulse.Y, impulse.Z));
			}
#endif
		}

		void CRigidbody::applyForce(const core::vector3df& force, const core::vector3df& localPosition)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->applyForce(
					btVector3(force.X, force.Y, force.Z),
					btVector3(localPosition.X, localPosition.Y, localPosition.Z)
				);
			}
#endif
		}

		void CRigidbody::applyImpulse(const core::vector3df& impulse, const core::vector3df& localPosition)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->applyImpulse(
					btVector3(impulse.X, impulse.Y, impulse.Z),
					btVector3(localPosition.X, localPosition.Y, localPosition.Z)
				);
			}
#endif
		}

		void CRigidbody::applyPushImpulse(const core::vector3df& impulse, const core::vector3df& localPosition)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->applyPushImpulse(
					btVector3(impulse.X, impulse.Y, impulse.Z),
					btVector3(localPosition.X, localPosition.Y, localPosition.Z)
				);
			}
#endif
		}

		void CRigidbody::applyTorque(const core::vector3df& torque)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->applyTorque(btVector3(torque.X, torque.Y, torque.Z));
			}
#endif
		}

		void CRigidbody::applyTorqueImpulse(const core::vector3df& torqueImpulse)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->applyTorqueImpulse(btVector3(torqueImpulse.X, torqueImpulse.Y, torqueImpulse.Z));
			}
#endif
		}

		void CRigidbody::applyTorqueTurnImpulse(const core::vector3df& torqueImpulse)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->applyTorqueTurnImpulse(btVector3(torqueImpulse.X, torqueImpulse.Y, torqueImpulse.Z));
			}
#endif
		}

		void CRigidbody::clearForce()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->clearForces();
			}
#endif
		}
	}
}