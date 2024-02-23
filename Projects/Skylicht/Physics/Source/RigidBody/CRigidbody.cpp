#include "pch.h"
#include "CRigidbody.h"

#include "GameObject/CGameObject.h"

#include "Collider/CCollider.h"
#include "Collider/CBoxCollider.h"
#include "Collider/CStaticPlaneCollider.h"

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
#ifdef USE_BULLET_PHYSIC_ENGINE
			m_shape(NULL),
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
			m_gameObject->getTransformMatrix()->setIsWorldTransform(true);
		}

		void CRigidbody::updateComponent()
		{

		}

		void CRigidbody::setDynamic(bool b)
		{
			m_isDynamic;

#ifdef USE_BULLET_PHYSIC_ENGINE
			CPhysicsEngine* engine = CPhysicsEngine::getInstance();

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

			switch (collider->getColliderType())
			{
			case CCollider::Box:
			{
				CBoxCollider* box = m_gameObject->getComponent<CBoxCollider>();

				core::vector3df s = box->getSize() * 0.5f;
				m_shape = new btBoxShape(btVector3(s.X, s.Y, s.Z));
			}
			break;
			case CCollider::Plane:
			{
				CStaticPlaneCollider* plane = m_gameObject->getComponent<CStaticPlaneCollider>();

				const core::vector3df& n = plane->getNormal();
				m_shape = new btStaticPlaneShape(btVector3(n.X, n.Y, n.Z), plane->getD());
			}
			default:
			{

			}
			}

			if (m_shape == NULL)
				return false;

			btTransform startTransform;

			const f32* matrix = transform->getRelativeTransform().pointer();
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
			{
				btDiscreteDynamicsWorld* world = engine->getDynamicsWorld();
				world->removeRigidBody(m_rigidBody);
			}
#endif
		}
	}
}