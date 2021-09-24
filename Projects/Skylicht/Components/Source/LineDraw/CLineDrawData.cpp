/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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
#include "Material/Shader/CShaderManager.h"
#include "CLineDrawData.h"

namespace Skylicht
{
	CLineDrawData::CLineDrawData()
	{
		LineBuffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD));
		LineBuffer->getMaterial().MaterialType = CShaderManager::getInstance()->getShaderIDByName("VertexColor");
		LineBuffer->setPrimitiveType(EPT_LINES);
		LineBuffer->setHardwareMappingHint(EHM_STREAM);
	}

	CLineDrawData::~CLineDrawData()
	{
		LineBuffer->drop();
	}

	void CLineDrawData::add3DBox(const core::aabbox3d<f32>& box, const SColor& color)
	{
		core::vector3df edges[8];
		box.getEdges(edges);

		addLine(edges[5], edges[1], color);
		addLine(edges[1], edges[3], color);
		addLine(edges[3], edges[7], color);
		addLine(edges[7], edges[5], color);
		addLine(edges[0], edges[2], color);
		addLine(edges[2], edges[6], color);
		addLine(edges[6], edges[4], color);
		addLine(edges[4], edges[0], color);
		addLine(edges[1], edges[0], color);
		addLine(edges[3], edges[2], color);
		addLine(edges[7], edges[6], color);
		addLine(edges[5], edges[4], color);
	}

	void CLineDrawData::addLine(const core::vector3df& v1, const core::vector3df& v2, const SColor& color)
	{
		IVertexBuffer* vtxBuffer = LineBuffer->getVertexBuffer();
		IIndexBuffer* idxBuffer = LineBuffer->getIndexBuffer();

		u32 idxCount = vtxBuffer->getVertexCount();

		video::S3DVertex vert;
		vert.Color = color;

		vert.Pos = v1;
		vtxBuffer->addVertex(&vert);
		vert.Pos = v2;
		vtxBuffer->addVertex(&vert);

		idxBuffer->addIndex(idxCount++);
		idxBuffer->addIndex(idxCount);
	}

	void CLineDrawData::addPolyline(const core::vector3df* points, u32 count, bool close, const SColor& color)
	{
		IVertexBuffer* vtxBuffer = LineBuffer->getVertexBuffer();
		IIndexBuffer* idxBuffer = LineBuffer->getIndexBuffer();

		u32 idxCount = vtxBuffer->getVertexCount();

		video::S3DVertex vert;
		vert.Color = color;

		for (u32 i = 1; i < count; i++)
		{
			vert.Pos = points[i - 1];
			vtxBuffer->addVertex(&vert);

			vert.Pos = points[i];
			vtxBuffer->addVertex(&vert);

			idxBuffer->addIndex(idxCount++);
			idxBuffer->addIndex(idxCount++);
		}

		if (close)
		{
			vert.Pos = points[count - 1];
			vtxBuffer->addVertex(&vert);

			vert.Pos = points[0];
			vtxBuffer->addVertex(&vert);

			idxBuffer->addIndex(idxCount++);
			idxBuffer->addIndex(idxCount++);
		}
	}

	void CLineDrawData::clearBuffer()
	{
		LineBuffer->getVertexBuffer()->set_used(0);
		LineBuffer->getIndexBuffer()->set_used(0);
	}

	void CLineDrawData::updateBuffer()
	{
		LineBuffer->setDirty();
	}
}