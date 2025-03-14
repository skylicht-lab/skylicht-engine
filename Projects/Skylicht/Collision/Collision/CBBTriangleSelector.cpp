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
#include "CBBTriangleSelector.h"
#include "RenderMesh/CRenderMeshData.h"
#include "Transform/CWorldTransformData.h"

namespace Skylicht
{
	CBBTriangleSelector::CBBTriangleSelector(CEntity* entity) :
		CTriangleSelector(entity)
	{
		init();
	}

	CBBTriangleSelector::~CBBTriangleSelector()
	{

	}

	void CBBTriangleSelector::init()
	{
		CRenderMeshData* renderMeshData = GET_ENTITY_DATA(m_entity, CRenderMeshData);
		CMesh* mesh = renderMeshData->getMesh();

		m_triangles.set_used(12);
		m_bbox = mesh->getBoundingBox();

		core::vector3df edges[8];
		m_bbox.getEdges(edges);

		m_triangles[0].set(edges[3], edges[0], edges[2]);
		m_triangles[1].set(edges[3], edges[1], edges[0]);

		m_triangles[2].set(edges[3], edges[2], edges[7]);
		m_triangles[3].set(edges[7], edges[2], edges[6]);

		m_triangles[4].set(edges[7], edges[6], edges[4]);
		m_triangles[5].set(edges[5], edges[7], edges[4]);

		m_triangles[6].set(edges[5], edges[4], edges[0]);
		m_triangles[7].set(edges[5], edges[0], edges[1]);

		m_triangles[8].set(edges[1], edges[3], edges[7]);
		m_triangles[9].set(edges[1], edges[7], edges[5]);

		m_triangles[10].set(edges[0], edges[6], edges[2]);
		m_triangles[11].set(edges[0], edges[4], edges[6]);
	}

	const core::aabbox3df& CBBTriangleSelector::getBBox()
	{
		return m_bbox;
	}
}