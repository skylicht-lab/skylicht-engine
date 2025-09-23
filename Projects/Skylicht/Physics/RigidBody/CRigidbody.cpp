#include "pch.h"
#include "CRigidbody.h"

#include "GameObject/CGameObject.h"

#include "Collider/CCollider.h"
#include "PhysicsEngine/CPhysicsEngine.h"

namespace Skylicht
{
	namespace Physics
	{
		ACTIVATOR_REGISTER(CRigidbody);

		CATEGORY_COMPONENT(CRigidbody, "Rigidbody", "Physics");

		CRigidbody::CRigidbody() :
			m_mass(1.0f),
			m_friction(0.5f),
			m_restitution(0.0f),
			m_rollingFriction(0.0f),
			m_spinningFriction(0.0f),
			m_isDynamic(true),
			m_needUpdateTransform(true),
			m_drawDebug(false)
#ifdef USE_BULLET_PHYSIC_ENGINE
			, m_shape(NULL),
			m_rigidBody(NULL)
#endif
		{
			m_collisionType = ICollisionObject::RigidBody;
		}

		CRigidbody::~CRigidbody()
		{
			releaseRigidbody();
		}

		void CRigidbody::initComponent()
		{
			CPhysicsEngine* engine = CPhysicsEngine::getInstance();
			if (engine != NULL)
			{
				if (engine->IsInEditor)
					initRigidbody();
			}
		}

		void CRigidbody::startComponent()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (!m_rigidBody)
				initRigidbody();
#endif
		}

		void CRigidbody::updateComponent()
		{

		}

		CObjectSerializable* CRigidbody::createSerializable()
		{
			CObjectSerializable* object = CComponentSystem::createSerializable();

			object->autoRelease(new CBoolProperty(object, "isDynamic", m_isDynamic));
			object->autoRelease(new CFloatProperty(object, "mass", m_mass, 0.0f));
			object->autoRelease(new CFloatProperty(object, "friction", m_friction, 0.0f));
			object->autoRelease(new CFloatProperty(object, "rollingFriction", m_rollingFriction, 0.0f));
			object->autoRelease(new CFloatProperty(object, "spinningFriction", m_spinningFriction, 0.0f));

			return object;
		}

		void CRigidbody::loadSerializable(CObjectSerializable* object)
		{
			CComponentSystem::loadSerializable(object);

			m_isDynamic = object->get<bool>("isDynamic", true);
			m_mass = object->get<float>("mass", 1.0f);
			m_friction = object->get<float>("friction", 1.0f);
			m_rollingFriction = object->get<float>("rollingFriction", 0.0f);
			m_spinningFriction = object->get<float>("spinningFriction", 0.0f);

			initRigidbody();
		}

		void CRigidbody::setFriction(float f)
		{
			m_friction = f;
		}

		void CRigidbody::setRollingFriction(float f)
		{
			m_rollingFriction = f;
		}

		void CRigidbody::setSpinningFriction(float f)
		{
			m_spinningFriction = f;
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
				btVector3 inertia;
				m_rigidBody->getCollisionShape()->calculateLocalInertia(m_mass, inertia);
				m_rigidBody->setMassProps(m_mass, inertia);
			}
#endif
		}

		void CRigidbody::setRestitution(float s)
		{
			m_restitution = s;
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->setRestitution(s);
			}
#endif
		}

		void CRigidbody::setCcdSweptSphereRadius(float r)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->setCcdSweptSphereRadius(r);
			}
#endif
		}

		void CRigidbody::setCcdMotionThreshold(float m)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->setCcdMotionThreshold(m);
			}
#endif
		}

		bool CRigidbody::initRigidbody()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			CPhysicsEngine* engine = CPhysicsEngine::getInstance();
			if (engine == NULL || !engine->isInitialized())
			{
				os::Printer::log("[CRigidbody] initRigidbody failed because Physics engine is not init");
				return false;
			}

			CCollider* collider = m_gameObject->getComponent<CCollider>();
			if (collider == NULL)
				return false;

			if (!engine->IsInEditor)
			{
				// note: MatrixTransform is faster
				m_gameObject->setupMatrixTransform();
			}

			CTransform* transform = m_gameObject->getTransform();
			if (transform == NULL)
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

			core::matrix4 matrix, nonScale;

			if (engine->IsInEditor)
				matrix = transform->calcWorldTransform();
			else
				matrix = transform->getRelativeTransform();

			Transform::getNonScaleTransform(matrix, nonScale);

			startTransform.setFromOpenGLMatrix(nonScale.pointer());
			m_shape->setLocalScaling(Bullet::irrVectorToBulletVector(matrix.getScale()));

			btVector3 localInertia(0, 0, 0);
			float mass = m_mass;

			if (m_isDynamic && collider->isDynamicSupport())
				m_shape->calculateLocalInertia(m_mass, localInertia);
			else
				mass = 0.0f;

			btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, m_shape, localInertia);

			rbInfo.m_friction = m_friction;
			rbInfo.m_rollingFriction = m_rollingFriction;
			rbInfo.m_spinningFriction = m_spinningFriction;

			m_rigidBody = new btRigidBody(rbInfo);
			m_rigidBody->setUserPointer(this);
			m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

			// add rigid body
			engine->addBody(this);

			if (!engine->IsInEditor)
			{
				// Transform component isnot in relative in Parent
				// It is sync from Physics Engine
				transform->setIsWorldTransform(true);
			}

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

				core::matrix4 scale;
				scale.setScale(getLocalScale());

				core::matrix4 mat;
				Bullet::bulletTransformToIrrMatrix(world, mat);

				transform->setWorldMatrix(mat * scale);
			}
#endif
		}

		void CRigidbody::setLocalScale(const core::vector3df& scale)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_shape)
				m_shape->setLocalScaling(Bullet::irrVectorToBulletVector(scale));
#endif
		}

		core::vector3df CRigidbody::getLocalScale()
		{
			core::vector3df scale(1.0f, 1.0f, 1.0f);
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_shape)
				scale = Bullet::bulletVectorToIrrVector(m_shape->getLocalScaling());
#endif
			return scale;
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

		void CRigidbody::activate()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->activate(true);
			}
#endif
		}

		CRigidbody::EActivationState CRigidbody::getState()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			int state = m_rigidBody->getActivationState();
			if (state == ACTIVE_TAG)
				return Activate;
			else if (state == ISLAND_SLEEPING)
				return Sleep;
			else if (state == DISABLE_DEACTIVATION)
				return Alway;
			else if (state == DISABLE_SIMULATION)
				return Disable;
			return Activate;
#else
			return Disable;
#endif
		}

		const char* CRigidbody::getStateName()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
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
#else
			return "Disable";
#endif
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

		core::matrix4 CRigidbody::getWorldTransform()
		{
			core::matrix4 world;

#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				btTransform& transform = m_rigidBody->getWorldTransform();
#ifdef BT_USE_NEON
				float ptr[16] __attribute__((aligned(16)));
				transform.getOpenGLMatrix(ptr);

				float* m = world.pointer();
				for (int i = 0; i < 16; i++)
					m[i] = ptr[i];
#else
				f32* ptr = world.pointer();
				transform.getOpenGLMatrix(ptr);
#endif
			}
#endif
			return world;
		}
	}
}