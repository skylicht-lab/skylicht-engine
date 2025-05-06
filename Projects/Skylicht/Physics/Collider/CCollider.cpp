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

#ifdef USE_BULLET_PHYSIC_ENGINE
		class TrangleDrawcallback : public btTriangleCallback, public btInternalTriangleIndexCallback
		{
		protected:
			core::array<core::vector3df> m_vertices;
			core::array<core::vector3df> m_normals;
			core::array<u32> m_n;
			core::array<u32> m_indices;

			std::map<core::vector3df, u32> m_vid;
		public:
			TrangleDrawcallback()
			{

			}

			virtual void internalProcessTriangleIndex(btVector3* triangle, int partId, int triangleIndex)
			{
				processTriangle(triangle, partId, triangleIndex);
			}

			virtual void processTriangle(btVector3* triangle, int partId, int triangleIndex)
			{
				(void)partId;
				(void)triangleIndex;

				core::vector3df a, b, c;
				a.set(triangle[0].x(), triangle[0].y(), triangle[0].z());
				b.set(triangle[1].x(), triangle[1].y(), triangle[1].z());
				c.set(triangle[2].x(), triangle[2].y(), triangle[2].z());

				core::vector3df normal = (b - a).crossProduct(c - a);
				normal.normalize();

				u32 ia, ib, ic;

				// A
				auto it = m_vid.find(a);
				if (it == m_vid.end())
				{
					ia = m_vertices.size();
					m_vid[a] = ia;
					m_vertices.push_back(a);
					m_normals.push_back(normal);
					m_n.push_back(1);
				}
				else
				{
					ia = it->second;
					m_normals[ia] += normal;
					m_n[ia]++;
				}

				// B
				it = m_vid.find(b);
				if (it == m_vid.end())
				{
					ib = m_vertices.size();
					m_vid[b] = ib;
					m_vertices.push_back(b);
					m_normals.push_back(normal);
					m_n.push_back(1);
				}
				else
				{
					ib = it->second;
					m_normals[ib] += normal;
					m_n[ib]++;
				}

				// C
				it = m_vid.find(c);
				if (it == m_vid.end())
				{
					ic = m_vertices.size();
					m_vid[c] = ic;
					m_vertices.push_back(c);
					m_normals.push_back(normal);
					m_n.push_back(1);
				}
				else
				{
					ic = it->second;
					m_normals[ic] += normal;
					m_n[ic]++;
				}

				m_indices.push_back(ia);
				m_indices.push_back(ib);
				m_indices.push_back(ic);
			}

			CMesh* getMesh()
			{
				IVideoDriver* driver = getVideoDriver();
				CMesh* mesh = new CMesh();

				IMeshBuffer* meshBuffer = new CMeshBuffer<S3DVertex>(driver->getVertexDescriptor(EVT_STANDARD), EIT_16BIT);
				IIndexBuffer* ib = meshBuffer->getIndexBuffer();
				IVertexBuffer* vb = meshBuffer->getVertexBuffer();

				for (u32 i = 0, n = m_vertices.size(); i < n; i++)
				{
					S3DVertex v;
					v.Pos = m_vertices[i];
					v.Normal = m_normals[i] / (float)m_n[i];
					v.Normal.normalize();
					vb->addVertex(&v);
				}

				for (u32 i = 0, n = m_indices.size(); i < n; i++)
				{
					ib->addIndex(m_indices[i]);
				}

				mesh->addMeshBuffer(meshBuffer);
				meshBuffer->recalculateBoundingBox();

				mesh->recalculateBoundingBox();
				mesh->setHardwareMappingHint(EHM_STATIC);

				meshBuffer->drop();
				return mesh;
			}
		};
#endif

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

		core::aabbox3df CCollider::getBBox()
		{
			core::aabbox3df box;

#if defined(USE_BULLET_PHYSIC_ENGINE)
			if (m_shape == NULL)
				return box;

			if (m_shape->getShapeType() == STATIC_PLANE_PROXYTYPE)
				return box;

			CRigidbody* rigidBody = m_gameObject->getComponent<CRigidbody>();
			if (!rigidBody)
				return box;

			btRigidBody* btBody = rigidBody->getBody();
			if (btBody == NULL)
				return box;

			btVector3 min, max;
			m_shape->getAabb(btBody->getWorldTransform(), min, max);

			box.MinEdge.set(min.x(), min.y(), min.z());
			box.MaxEdge.set(max.x(), max.y(), max.z());
#endif

			return box;

		}

		CMesh* CCollider::generateMesh(const core::aabbox3df& maxBBox)
		{
#if defined(USE_BULLET_PHYSIC_ENGINE)
			if (m_shape == NULL)
				return NULL;

			const IGeometryCreator* geometry = getIrrlichtDevice()->getSceneManager()->getGeometryCreator();

			CMesh* mesh = NULL;

			// see function btCollisionWorld::debugDrawObject

			if (m_shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
			{
				// todo later
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
					break;
				}
				case CAPSULE_SHAPE_PROXYTYPE:
				{
#ifdef BUILD_SKYLICHT_COMPONENTS
					const btCapsuleShape* capsuleShape = static_cast<const btCapsuleShape*>(m_shape);

					btScalar radius = capsuleShape->getRadius();
					btScalar halfHeight = capsuleShape->getHalfHeight();

					CCapsuleMesh capsuleMesh;
					capsuleMesh.init(radius, halfHeight * 2.0f, NULL, false);
					mesh = capsuleMesh.getMesh();
					mesh->grab();
#endif
					break;
				}
				case CONE_SHAPE_PROXYTYPE:
				{
					break;
				}
				case CYLINDER_SHAPE_PROXYTYPE:
				{
#ifdef BUILD_SKYLICHT_COMPONENTS
					const btCylinderShape* cylinder = static_cast<const btCylinderShape*>(m_shape);
					int upAxis = cylinder->getUpAxis();
					btScalar radius = cylinder->getRadius();
					btScalar halfHeight = cylinder->getHalfExtentsWithMargin()[upAxis];

					CCylinderMesh cylinderMesh;
					cylinderMesh.init(radius, halfHeight * 2.0f, NULL, false);
					mesh = cylinderMesh.getMesh();
					mesh->grab();
#endif
					break;
				}

				case STATIC_PLANE_PROXYTYPE:
				{
					const btStaticPlaneShape* staticPlaneShape = static_cast<const btStaticPlaneShape*>(m_shape);
					const btScalar planeConst = staticPlaneShape->getPlaneConstant();
					const btVector3& planeNormal = staticPlaneShape->getPlaneNormal();

					core::vector3df size = maxBBox.getExtent();
					size.X = core::max_(size.X, 10.0f);
					size.Z = core::max_(size.Z, 10.0f);

					core::plane3df p(Bullet::bulletVectorToIrrVector(planeNormal), planeConst);
					mesh = getPlane(p, size.X, size.Z);

					break;
				}
				default:
				{
					if (m_shape->isPolyhedral())
					{
						return NULL;
					}
					else if (m_shape->isConcave())
					{
						btConcaveShape* concaveMesh = (btConcaveShape*)m_shape;

						btVector3 aabbMax(btScalar(BT_LARGE_FLOAT), btScalar(BT_LARGE_FLOAT), btScalar(BT_LARGE_FLOAT));
						btVector3 aabbMin(btScalar(-BT_LARGE_FLOAT), btScalar(-BT_LARGE_FLOAT), btScalar(-BT_LARGE_FLOAT));

						TrangleDrawcallback triangleCallback;
						concaveMesh->processAllTriangles(&triangleCallback, aabbMin, aabbMax);
						mesh = triangleCallback.getMesh();
					}
					else if (m_shape->getShapeType() == CONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE)
					{
						btConvexTriangleMeshShape* convexMesh = (btConvexTriangleMeshShape*)m_shape;

						btVector3 aabbMax(btScalar(BT_LARGE_FLOAT), btScalar(BT_LARGE_FLOAT), btScalar(BT_LARGE_FLOAT));
						btVector3 aabbMin(btScalar(-BT_LARGE_FLOAT), btScalar(-BT_LARGE_FLOAT), btScalar(-BT_LARGE_FLOAT));

						TrangleDrawcallback triangleCallback;
						convexMesh->getMeshInterface()->InternalProcessAllTriangles(&triangleCallback, aabbMin, aabbMax);
						mesh = triangleCallback.getMesh();
					}

				} // default
				}
			}

			return mesh;
#else
			return NULL;
#endif
		}

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

		CMesh* CCollider::getPlane(const core::plane3df& plane, float sizeX, float sizeZ)
		{
			// Copilot: Can you write a C language function convert a plane 3D (D, Normal) to OpenGL vertex, indices, plane have size param X, Z
			core::vector3df normal = plane.Normal;
			normal.normalize();

			// Find a point on the plane using the distance from the origin
			core::vector3df pointOnPlane(
				plane.D * normal.X,
				plane.D * normal.Y,
				plane.D * normal.Z
			);

			// Generate two perpendicular vectors on the plane
			core::vector3df tangent(1.0f, 0.0f, 0.0f);
			if (fabs(normal.X) > 0.9f)
				tangent.set(0.0f, 1.0f, 0.0f);

			core::vector3df bitangent = normal.crossProduct(tangent);
			tangent = bitangent.crossProduct(normal);

			// Normalize the tangent and bitangent
			tangent.normalize();
			bitangent.normalize();

			// Allocate memory for vertices and indices
			IVideoDriver* driver = getVideoDriver();
			CMesh* mesh = new CMesh();

			IMeshBuffer* meshBuffer = new CMeshBuffer<S3DVertex>(driver->getVertexDescriptor(EVT_STANDARD), EIT_16BIT);
			IIndexBuffer* ib = meshBuffer->getIndexBuffer();
			IVertexBuffer* vb = meshBuffer->getVertexBuffer();

			S3DVertex v;
			v.Normal = normal;

			{
				// Calculate the plane's vertices
				v.Pos = core::vector3df(
					pointOnPlane.X + (-sizeX / 2) * tangent.X + (-sizeZ / 2) * bitangent.X,
					pointOnPlane.Y + (-sizeX / 2) * tangent.Y + (-sizeZ / 2) * bitangent.Y,
					pointOnPlane.Z + (-sizeX / 2) * tangent.Z + (-sizeZ / 2) * bitangent.Z
				);
				vb->addVertex(&v);
			}

			{
				v.Pos = core::vector3df(
					pointOnPlane.X + (sizeX / 2) * tangent.X + (-sizeZ / 2) * bitangent.X,
					pointOnPlane.Y + (sizeX / 2) * tangent.Y + (-sizeZ / 2) * bitangent.Y,
					pointOnPlane.Z + (sizeX / 2) * tangent.Z + (-sizeZ / 2) * bitangent.Z
				);
				vb->addVertex(&v);
			}

			{
				v.Pos = core::vector3df(
					pointOnPlane.X + (sizeX / 2) * tangent.X + (sizeZ / 2) * bitangent.X,
					pointOnPlane.Y + (sizeX / 2) * tangent.Y + (sizeZ / 2) * bitangent.Y,
					pointOnPlane.Z + (sizeX / 2) * tangent.Z + (sizeZ / 2) * bitangent.Z
				);
				vb->addVertex(&v);
			}

			{
				v.Pos = core::vector3df(
					pointOnPlane.X + (-sizeX / 2) * tangent.X + (sizeZ / 2) * bitangent.X,
					pointOnPlane.Y + (-sizeX / 2) * tangent.Y + (sizeZ / 2) * bitangent.Y,
					pointOnPlane.Z + (-sizeX / 2) * tangent.Z + (sizeZ / 2) * bitangent.Z
				);
				vb->addVertex(&v);
			}

			// Define the indices for the plane's two triangles
			ib->addIndex(0);
			ib->addIndex(1);
			ib->addIndex(2);

			ib->addIndex(2);
			ib->addIndex(3);
			ib->addIndex(0);


			mesh->addMeshBuffer(meshBuffer);
			meshBuffer->recalculateBoundingBox();

			mesh->recalculateBoundingBox();
			mesh->setHardwareMappingHint(EHM_STATIC);

			meshBuffer->drop();
			return mesh;
		}
	}
}