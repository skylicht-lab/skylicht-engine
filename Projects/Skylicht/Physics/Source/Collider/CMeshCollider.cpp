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
#include "CMeshCollider.h"
#include "MeshManager/CMeshManager.h"
#include "GameObject/CGameObject.h"

#ifdef USE_BULLET_PHYSIC_ENGINE
#include <BulletCollision/Gimpact/btGImpactShape.h>
#endif

namespace Skylicht
{
	namespace Physics
	{
		ACTIVATOR_REGISTER(CMeshCollider);

		CATEGORY_COMPONENT(CMeshCollider, "Mesh Collider", "Physics/Mesh Collider");

		CMeshCollider::CMeshCollider()
		{
			m_colliderType = CCollider::Mesh;
		}

		CMeshCollider::~CMeshCollider()
		{

		}

		void CMeshCollider::initComponent()
		{

		}

		void CMeshCollider::updateComponent()
		{

		}

		CObjectSerializable* CMeshCollider::createSerializable()
		{
			std::vector<std::string> meshExts = { "dae","obj","smesh" };

			CObjectSerializable* obj = CComponentSystem::createSerializable();
			obj->addProperty(new CFilePathProperty(obj, "source", m_source.c_str(), meshExts));
			return obj;
		}

		void CMeshCollider::loadSerializable(CObjectSerializable* object)
		{
			CComponentSystem::loadSerializable(object);
		}

		CEntityPrefab* CMeshCollider::getMeshPrefab()
		{
			return CMeshManager::getInstance()->loadModel(m_source.c_str(), NULL, false, false);
		}

		core::matrix4 CMeshCollider::getWorldTransform()
		{
			return m_gameObject->getTransform()->calcWorldTransform();
		}

#ifdef USE_BULLET_PHYSIC_ENGINE
		btCollisionShape* CMeshCollider::initCollisionShape()
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

			btGImpactMeshShape* meshShape = new btGImpactMeshShape(triangleMesh);
			meshShape->updateBound();

			m_shape = meshShape;
			m_shape->setUserPointer(this);
			return m_shape;
		}

		void CMeshCollider::initFromPrefab(CEntityPrefab* prefab, std::function<void(const core::matrix4&, CMesh*)> callback)
		{
			int numEntities = prefab->getNumEntities();

			// we just find the renderer entity
			core::array<CEntity*> renderEntities;
			for (int i = 0; i < numEntities; i++)
			{
				CEntity* srcEntity = prefab->getEntity(i);
				CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
				if (srcRender != NULL && srcRender->getMesh() != NULL)
				{
					if (!srcRender->isSkinnedMesh())
					{
						// just add static mesh
						renderEntities.push_back(srcEntity);
					}
				}
			}

			numEntities = renderEntities.size();

			core::array<core::matrix4> m_transforms;
			core::array<CMesh*> m_meshs;

			for (int i = 0; i < numEntities; i++)
			{
				CEntity* srcEntity = renderEntities[i];

				// get transform data
				CWorldTransformData* srcTransform = srcEntity->getData<CWorldTransformData>();
				if (srcTransform != NULL)
				{
					// get the world matrix
					core::matrix4 m = srcTransform->Relative;
					int parentID = srcTransform->ParentIndex;
					while (parentID != -1)
					{
						CEntity* parent = prefab->getEntity(parentID);
						CWorldTransformData* parentTransform = parent->getData<CWorldTransformData>();

						m = parentTransform->Relative * m;
						parentID = parentTransform->ParentIndex;
					}

					m_transforms.push_back(m);
				}

				// get render data
				CRenderMeshData* srcRender = srcEntity->getData<CRenderMeshData>();
				if (srcRender != NULL)
				{
					m_meshs.push_back(srcRender->getMesh());
				}
			}

			core::matrix4 world = getWorldTransform();
			core::matrix4 m;

			for (int i = 0; i < numEntities; i++)
			{
				m = world * m_transforms[i];
				callback(m, m_meshs[i]);
			}
		}
#endif
	}
}