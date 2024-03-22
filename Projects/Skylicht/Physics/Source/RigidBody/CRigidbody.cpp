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
			m_isDynamic(true)
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
			case CCollider::Mesh:
			case CCollider::BvhMesh:
			case CCollider::ConvexMesh:
			{
				initMeshCollider(collider);
			}
			break;
			default:
			{
				m_shape = NULL;
			}
			}

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

			m_shape->setUserPointer(collider);

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

		void CRigidbody::initMeshCollider(CCollider* collider)
		{
			CMeshCollider* meshCollider = dynamic_cast<CMeshCollider*>(collider);
			if (meshCollider == NULL)
				return;

			CEntityPrefab* prefab = meshCollider->getMeshPrefab();
			if (prefab == NULL)
				return;
/*
	// MESH COLLIDER
	m_indexVertexArrays = new btTriangleMesh();

	int numMeshBuffer = mesh->getMeshBufferCount();

	btVector3 vertices[3];
	core::vector3df v;

	for (int i = 0; i < numMeshBuffer; i++)
	{
		IMeshBuffer *meshBuffer = mesh->getMeshBuffer(i);

		IVertexBuffer	*vertexBuffer = meshBuffer->getVertexBuffer();
		IIndexBuffer	*indexBuffer = meshBuffer->getIndexBuffer();

		unsigned char* vertex = (unsigned char*)vertexBuffer->getVertices();
		int vertexSize = vertexBuffer->getVertexSize();

		int numIndex = indexBuffer->getIndexCount();

		u16 *indexBuffer16 = NULL;
		u32 *indexBuffer32 = NULL;

		if (indexBuffer->getType() == video::EIT_16BIT)
			indexBuffer16 = (u16*)indexBuffer->getIndices();
		else
			indexBuffer32 = (u32*)indexBuffer->getIndices();

		for (int j = 0; j <numIndex; j+=3)
		{
			for (int k = 0; k < 3; k++)
			{
				u32 index = 0;

				if (indexBuffer16 != NULL)
					index = indexBuffer16[j + k];
				else if (indexBuffer32 != NULL)
					index = indexBuffer32[j + k];

				unsigned char *vPos = vertex + index * vertexSize;
				video::S3DVertex *vtx = (video::S3DVertex*)vPos;

				v = vtx->Pos;
				transform.transformVect(v);

				vertices[k] = btVector3(v.X, v.Y, v.Z);
			}

			m_indexVertexArrays->addTriangle(vertices[0], vertices[1], vertices[2]);
		}

	}

	btGImpactMeshShape *meshShape = new btGImpactMeshShape(m_indexVertexArrays);
	meshShape->updateBound();
	return meshShape;


	// HULL
	int numMeshBuffer = mesh->getMeshBufferCount();

	btVector3 vertices;
	core::vector3df v;

	for (int i = 0; i < numMeshBuffer; i++)
	{
		IMeshBuffer *meshBuffer = mesh->getMeshBuffer(i);

		IVertexBuffer	*vertexBuffer = meshBuffer->getVertexBuffer();
		IIndexBuffer	*indexBuffer = meshBuffer->getIndexBuffer();

		unsigned char *vertex = (unsigned char*)vertexBuffer->getVertices();
		int vertexSize = vertexBuffer->getVertexSize();

		int numIndex = indexBuffer->getIndexCount();

		u16 *indexBuffer16 = NULL;
		u32 *indexBuffer32 = NULL;

		if (indexBuffer->getType() == video::EIT_16BIT)
			indexBuffer16 = (u16*)indexBuffer->getIndices();
		else
			indexBuffer32 = (u32*)indexBuffer->getIndices();

		for (int j = 0; j <numIndex; j+=3)
		{
			for (int k = 0; k < 3; k++)
			{
				u32 index = 0;

				if (indexBuffer16 != NULL)
					index = (u32)indexBuffer16[j + k];
				else
					index = (u32)indexBuffer32[j + k];

				unsigned char *vPos = vertex + index * vertexSize;
				video::S3DVertex *vtx = (video::S3DVertex*)vPos;

				v = vtx->Pos;
				transform.transformVect(v);

				vertices = btVector3(v.X, v.Y, v.Z);

				// add hull shape
				hullShape->addPoint(vertices, false);
			}
		}
	}

	hullShape->recalcLocalAabb();

	// BVH
	IMeshBuffer *meshBuffer = mesh->getMeshBuffer(0);

	IVertexBuffer	*vertexBuffer = meshBuffer->getVertexBuffer();
	IIndexBuffer	*indexBuffer = meshBuffer->getIndexBuffer();


	unsigned char *vertex = (unsigned char*)vertexBuffer->getVertices();
	int vertexSize = vertexBuffer->getVertexSize();
	int numVertex = vertexBuffer->getVertexCount();

	int numIndex = indexBuffer->getIndexCount();

	u16 *indexBuffer16 = NULL;
	u32 *indexBuffer32 = NULL;

	if (indexBuffer->getType() == video::EIT_16BIT)
		indexBuffer16 = (u16*)indexBuffer->getIndices();
	else
		indexBuffer32 = (u32*)indexBuffer->getIndices();

	m_vertices = new btVector3[numVertex];
	m_indices = new int[numIndex];

	for (int i = 0; i < numVertex; i++)
	{
		video::S3DVertex *v = (video::S3DVertex*)vertex;

		core::vector3df pos = v->Pos;
		transform.transformVect(pos);

		m_vertices[i] = btVector3(pos.X, pos.Y, pos.Z);

		vertex += vertexSize;
	}

	if (indexBuffer16 != NULL)
	{
		for (int i = 0; i < numIndex; i++)
		{
			m_indices[i] = (int)indexBuffer16[i];
		}
	}
	else
	{
		for (int i = 0; i < numIndex; i++)
		{
			m_indices[i] = (int)indexBuffer32[i];
		}
	}

	m_indexVertexArrays = new btTriangleIndexVertexArray(
		numIndex/3,
		m_indices,
		sizeof(int)*3,
		numVertex,
		(btScalar*)&m_vertices[0].x(),
		sizeof(btVector3)
	);

	bool useQuantizedAabbCompression = true;
	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(m_indexVertexArrays,useQuantizedAabbCompression);
*/
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

		void CRigidbody::applyTorque(const core::vector3df& torque)
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			if (m_rigidBody)
			{
				m_rigidBody->applyTorque(btVector3(torque.X, torque.Y, torque.Z));
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