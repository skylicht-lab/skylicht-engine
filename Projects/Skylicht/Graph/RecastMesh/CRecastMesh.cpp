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
#include "CRecastMesh.h"

namespace Skylicht
{
	namespace Graph
	{
		CRecastMesh::CRecastMesh()
		{

		}

		CRecastMesh::~CRecastMesh()
		{
			release();
		}

		void CRecastMesh::release()
		{
			m_verts.clear();
			m_tris.clear();
			m_normals.clear();
		}

		void CRecastMesh::addMeshPrefab(CEntityPrefab* prefab, const core::matrix4& world)
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

			// apply transform
			core::matrix4 m;
			for (int i = 0; i < numEntities; i++)
			{
				m = world * m_transforms[i];
				addMesh(m_meshs[i], m);
			}
		}

		void CRecastMesh::addMesh(CMesh* mesh, const core::matrix4& transform)
		{
			int numMeshBuffer = mesh->getMeshBufferCount();

			core::vector3df v, n;
			int a = 0, b = 0, c = 0;

			u16* indexBuffer16 = NULL;
			u32* indexBuffer32 = NULL;

			for (int i = 0; i < numMeshBuffer; i++)
			{
				IMeshBuffer* meshBuffer = mesh->getMeshBuffer(i);

				IVertexBuffer* vertexBuffer = meshBuffer->getVertexBuffer();
				IIndexBuffer* indexBuffer = meshBuffer->getIndexBuffer();

				unsigned char* vertex = (unsigned char*)vertexBuffer->getVertices();
				int vertexSize = vertexBuffer->getVertexSize();

				int numIndex = indexBuffer->getIndexCount();
				int numVertex = vertexBuffer->getVertexCount();

				if (indexBuffer->getType() == video::EIT_16BIT)
					indexBuffer16 = (u16*)indexBuffer->getIndices();
				else
					indexBuffer32 = (u32*)indexBuffer->getIndices();

				u32 batchVtx = m_verts.size() / 3;

				for (int j = 0; j < numVertex; j++)
				{
					unsigned char* vPos = vertex + j * vertexSize;
					video::S3DVertex* vtx = (video::S3DVertex*)vPos;

					v = vtx->Pos;
					transform.transformVect(v);

					n = vtx->Normal;
					transform.rotateVect(n);
					n.normalize();

					addVertex(v.X, v.Y, v.Z);
					addNormal(n.X, n.Y, n.Z);
				}

				for (int j = 0; j < numIndex; j += 3)
				{
					if (indexBuffer16 != NULL)
					{
						a = (int)indexBuffer16[j];
						b = (int)indexBuffer16[j + 1];
						c = (int)indexBuffer16[j + 2];
					}
					else if (indexBuffer32 != NULL)
					{
						a = (int)indexBuffer32[j];
						b = (int)indexBuffer32[j + 1];
						c = (int)indexBuffer32[j + 2];
					}

					addTriangle(batchVtx + a, batchVtx + b, batchVtx + c);
				}
			}
		}

		int CRecastMesh::addVertex(float x, float y, float z)
		{
			m_verts.push_back(x);
			m_verts.push_back(y);
			m_verts.push_back(z);

			if (m_verts.size() == 3)
				m_bbox.reset(x, y, z);
			else
				m_bbox.addInternalPoint(x, y, z);

			return m_verts.size() / 3;
		}

		int CRecastMesh::addNormal(float x, float y, float z)
		{
			m_normals.push_back(x);
			m_normals.push_back(y);
			m_normals.push_back(z);
			return m_normals.size() / 3;
		}

		void CRecastMesh::addTriangle(int a, int b, int c)
		{
			m_tris.push_back(a);
			m_tris.push_back(b);
			m_tris.push_back(c);
		}
	}
}