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

#include "pch.h"
#include "CCollider.h"
#include "RigidBody/CRigidbody.h"
#include "GameObject/CGameObject.h"
#include "PhysicsEngine/CPhysicsEngine.h"
#include "Importer/Utils/CMeshUtils.h"

#ifdef USE_BULLET_PHYSIC_ENGINE
#include "BulletCollision/CollisionShapes/btConvexPolyhedron.h"
#endif

#ifdef BUILD_SKYLICHT_COMPONENTS
#include "Primitive/LatheMesh/CCapsuleMesh.h"
#include "Primitive/LatheMesh/CCylinderMesh.h"
#endif

namespace Skylicht
{
	namespace Physics
	{
		CCollider::CCollider() :
			m_colliderType(Unknown),
			m_dynamicSupport(true)
#ifdef USE_BULLET_PHYSIC_ENGINE
			, m_shape(NULL)
#endif
		{

		}

		CCollider::~CCollider()
		{
#ifdef USE_BULLET_PHYSIC_ENGINE
			dropCollisionShape();
#endif
		}

		void CCollider::initComponent()
		{
			CPhysicsEngine* engine = CPhysicsEngine::getInstance();
			if (engine && engine->IsInEditor)
				initRigidbody();
		}

#ifdef USE_BULLET_PHYSIC_ENGINE
		void CCollider::dropCollisionShape()
		{
			if (m_shape)
				m_shape->setUserPointer(NULL);
			m_shape = NULL;
		}
#endif

		void CCollider::initRigidbody()
		{
			CRigidbody* rigidBody = m_gameObject->getComponent<CRigidbody>();
			if (rigidBody)
				rigidBody->initRigidbody();
		}

		void CCollider::clampSize(core::vector3df& size)
		{
			size.X = core::max_(size.X, 0.0f);
			size.Y = core::max_(size.Y, 0.0f);
			size.Z = core::max_(size.Z, 0.0f);
		}

		CMesh* CCollider::generateMesh()
		{
#if defined(USE_BULLET_PHYSIC_ENGINE) && defined(BUILD_SKYLICHT_COMPONENTS)
			if (m_shape == NULL)
				return NULL;

			const IGeometryCreator* geometry = getIrrlichtDevice()->getSceneManager()->getGeometryCreator();

			CMesh* mesh = NULL;

			// see function btCollisionWorld::debugDrawObject

			if (m_shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
			{
				// todo later
				return NULL;
			}
			else
			{
				switch (m_shape->getShapeType())
				{
				case BOX_SHAPE_PROXYTYPE:
				{
					const btBoxShape* boxShape = static_cast<const btBoxShape*>(m_shape);
					btVector3 halfExtents = boxShape->getHalfExtentsWithMargin();
					IMesh* m = geometry->createCubeMesh(
						core::vector3df(2.0f * halfExtents.x(), 2.0f * halfExtents.y(), 2.0f * halfExtents.z())
					);
					mesh = generateMesh(m, false);
					m->drop();
					break;
				}

				case SPHERE_SHAPE_PROXYTYPE:
				{
					const btSphereShape* sphereShape = static_cast<const btSphereShape*>(m_shape);
					btScalar radius = sphereShape->getMargin();  //radius doesn't include the margin, so draw with margin

					IMesh* m = geometry->createSphereMesh(radius);
					mesh = generateMesh(m, false);
					m->drop();

					break;
				}
				case MULTI_SPHERE_SHAPE_PROXYTYPE:
				{
					// later
					return NULL;
				}
				case CAPSULE_SHAPE_PROXYTYPE:
				{
					const btCapsuleShape* capsuleShape = static_cast<const btCapsuleShape*>(m_shape);

					btScalar radius = capsuleShape->getRadius();
					btScalar halfHeight = capsuleShape->getHalfHeight();



					break;
				}
				case CONE_SHAPE_PROXYTYPE:
				{
					// later
					return NULL;
				}
				case CYLINDER_SHAPE_PROXYTYPE:
				{
					const btCylinderShape* cylinder = static_cast<const btCylinderShape*>(m_shape);
					int upAxis = cylinder->getUpAxis();
					btScalar radius = cylinder->getRadius();
					btScalar halfHeight = cylinder->getHalfExtentsWithMargin()[upAxis];



					break;
				}

				case STATIC_PLANE_PROXYTYPE:
				{
					const btStaticPlaneShape* staticPlaneShape = static_cast<const btStaticPlaneShape*>(m_shape);
					btScalar planeConst = staticPlaneShape->getPlaneConstant();
					const btVector3& planeNormal = staticPlaneShape->getPlaneNormal();

					break;
				}
				default:
				{
					/// for polyhedral shapes
					if (m_shape->isPolyhedral())
					{

					}

					if (m_shape->isConcave())
					{

					}

					if (m_shape->getShapeType() == CONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE)
					{

					}

					// todo later
					return NULL;
				} // default
				}
			}

			return mesh;
#else
			return NULL;
#endif
		}

#ifdef USE_BULLET_PHYSIC_ENGINE
		CMesh* CCollider::generateMesh(IMesh* primitive, bool tangent)
		{
			CMesh* mesh = new CMesh();

			IMeshBuffer* mb = primitive->getMeshBuffer(0);

			if (tangent)
				CMeshUtils::convertToTangentVertices(mb);

			mesh->addMeshBuffer(mb);

			mb->recalculateBoundingBox();
			mesh->recalculateBoundingBox();
			mesh->setHardwareMappingHint(EHM_STATIC);

			return mesh;
		}
#endif

	}
}