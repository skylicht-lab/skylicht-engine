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

#pragma once

#include "Entity/CEntityPrefab.h"
#include "RenderMesh/CRenderMeshData.h"

namespace Skylicht
{
	namespace Graph
	{
		class CRecastMesh
		{
		protected:
			core::array<float> m_verts;
			core::array<float> m_normals;
			core::array<int> m_tris;

			core::aabbox3df m_bbox;
		public:
			CRecastMesh();

			virtual ~CRecastMesh();

			void release();

			void addMeshPrefab(CEntityPrefab* prefab, const core::matrix4& world);

			inline float* getVerts()
			{
				return m_verts.pointer();
			}

			inline float* getNormals()
			{
				return m_normals.pointer();
			}

			inline int* getTris()
			{
				return m_tris.pointer();
			}

			inline u32 getVertCount()
			{
				return m_verts.size() / 3;
			}

			inline u32 getTriCount()
			{
				return m_tris.size() / 3;
			}

			inline const core::aabbox3df& getBBox()
			{
				return m_bbox;
			}

			void addMesh(CMesh* mesh, const core::matrix4& transform);

			int addVertex(float x, float y, float z);

			int addNormal(float x, float y, float z);

			void addTriangle(int a, int b, int c);
		};
	}
}