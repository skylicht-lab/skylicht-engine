/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CMeshTriangleSelector.h"
#include "RenderMesh/CRenderMeshData.h"
#include "Transform/CWorldTransformData.h"

namespace Skylicht
{
	CMeshTriangleSelector::CMeshTriangleSelector(CEntity* entity) :
		CTriangleSelector(entity)
	{
		init();
	}

	CMeshTriangleSelector::~CMeshTriangleSelector()
	{

	}

	void CMeshTriangleSelector::init()
	{
		CRenderMeshData* renderMeshData = (CRenderMeshData*)m_entity->getDataByIndex(CRenderMeshData::DataTypeIndex);
		CMesh* mesh = renderMeshData->getMesh();

		u32 meshBuffers = mesh->getMeshBufferCount();

		u32 totalTris = 0;
		u32 triangleCount = 0;

		for (u32 i = 0; i < meshBuffers; ++i)
		{
			IMeshBuffer* bufer = mesh->getMeshBuffer(i);
			u32 numTris = bufer->getIndexBuffer()->getIndexCount() / 3;
			totalTris += numTris;
		}

		m_triangles.set_used(totalTris);

		for (u32 i = 0; i < meshBuffers; ++i)
		{
			updateFromMeshBuffer(mesh->getMeshBuffer(i), triangleCount);
		}

		m_bbox = mesh->getBoundingBox();
	}

	template <typename TIndex>
	static void updateTriangles(u32& triangleCount, core::array<core::triangle3df>& triangles, u32 idxCnt, const TIndex* indices, const u8* vertices, u32 vertexPitch)
	{
		for (u32 index = 0; index < idxCnt; index += 3)
		{
			core::triangle3df& tri = triangles[triangleCount++];
			tri.pointA = (*reinterpret_cast<const video::S3DVertex*>(&vertices[indices[index + 0] * vertexPitch])).Pos;
			tri.pointB = (*reinterpret_cast<const video::S3DVertex*>(&vertices[indices[index + 1] * vertexPitch])).Pos;
			tri.pointC = (*reinterpret_cast<const video::S3DVertex*>(&vertices[indices[index + 2] * vertexPitch])).Pos;
		}
	}

	void CMeshTriangleSelector::updateFromMeshBuffer(const IMeshBuffer* meshBuffer, u32& triangleCount)
	{
		if (!meshBuffer)
			return;

		IIndexBuffer* indexBuffer = meshBuffer->getIndexBuffer();
		IVertexBuffer* vertexBuffer = meshBuffer->getVertexBuffer();

		u32 idxCnt = indexBuffer->getIndexCount();
		u32 vertexPitch = getVertexPitchFromType(meshBuffer->getVertexType());
		u8* vertices = (u8*)vertexBuffer->getVertices();

		switch (indexBuffer->getType())
		{
		case video::EIT_16BIT:
		{
			const u16* indices = (const u16*)indexBuffer->getIndices();
			updateTriangles(triangleCount, m_triangles, idxCnt, indices, vertices, vertexPitch);
		}
		break;
		case video::EIT_32BIT:
		{
			const u32* indices = (const u32*)indexBuffer->getIndices();
			updateTriangles(triangleCount, m_triangles, idxCnt, indices, vertices, vertexPitch);
		}
		break;
		}
	}

	const core::aabbox3df& CMeshTriangleSelector::getBBox()
	{
		return m_bbox;
	}
}