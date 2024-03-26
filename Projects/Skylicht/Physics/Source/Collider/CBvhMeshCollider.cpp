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
#include "CBvhMeshCollider.h"

namespace Skylicht
{
	namespace Physics
	{
		ACTIVATOR_REGISTER(CBvhMeshCollider);

		CATEGORY_COMPONENT(CBvhMeshCollider, "Bvh Mesh Collider", "Physics/Collider");

		CBvhMeshCollider::CBvhMeshCollider()
		{
			m_colliderType = CCollider::BvhMesh;
		}

		CBvhMeshCollider::~CBvhMeshCollider()
		{

		}

#ifdef USE_BULLET_PHYSIC_ENGINE
		btCollisionShape* CBvhMeshCollider::initCollisionShape()
		{
			CEntityPrefab* prefab = getMeshPrefab();
			if (prefab == NULL)
				return NULL;

			btTriangleMesh* triangleMesh = new btTriangleMesh();

			initFromPrefab(prefab, [triangleMesh](const core::matrix4& transform, CMesh* mesh) {

				int numMeshBuffer = mesh->getMeshBufferCount();

				btVector3 vertices[3];
				core::vector3df v;

				for (int i = 0; i < numMeshBuffer; i++)
				{
					IMeshBuffer* meshBuffer = mesh->getMeshBuffer(i);

					IVertexBuffer* vertexBuffer = meshBuffer->getVertexBuffer();
					IIndexBuffer* indexBuffer = meshBuffer->getIndexBuffer();

					unsigned char* vertex = (unsigned char*)vertexBuffer->getVertices();
					int vertexSize = vertexBuffer->getVertexSize();

					int numIndex = indexBuffer->getIndexCount();

					u16* indexBuffer16 = NULL;
					u32* indexBuffer32 = NULL;

					if (indexBuffer->getType() == video::EIT_16BIT)
						indexBuffer16 = (u16*)indexBuffer->getIndices();
					else
						indexBuffer32 = (u32*)indexBuffer->getIndices();

					for (int j = 0; j < numIndex; j += 3)
					{
						for (int k = 0; k < 3; k++)
						{
							u32 index = 0;

							if (indexBuffer16 != NULL)
								index = indexBuffer16[j + k];
							else if (indexBuffer32 != NULL)
								index = indexBuffer32[j + k];

							unsigned char* vPos = vertex + index * vertexSize;
							video::S3DVertex* vtx = (video::S3DVertex*)vPos;

							v = vtx->Pos;
							transform.transformVect(v);

							vertices[k] = btVector3(v.X, v.Y, v.Z);
						}

						triangleMesh->addTriangle(vertices[0], vertices[1], vertices[2]);
					}
				}

				});

			btBvhTriangleMeshShape* bvhMeshShape = new btBvhTriangleMeshShape(triangleMesh, true, true);
			m_shape = bvhMeshShape;
			m_shape->setUserPointer(this);
			return m_shape;
		}
#endif
	}
}