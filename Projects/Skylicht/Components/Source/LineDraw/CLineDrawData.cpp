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

	void CLineDrawData::add3DBoxToBuffer(const core::aabbox3d<f32>& box, SColor color)
	{
		core::vector3df edges[8];
		box.getEdges(edges);

		addLineVertexBatch(edges[5], edges[1], color);
		addLineVertexBatch(edges[1], edges[3], color);
		addLineVertexBatch(edges[3], edges[7], color);
		addLineVertexBatch(edges[7], edges[5], color);
		addLineVertexBatch(edges[0], edges[2], color);
		addLineVertexBatch(edges[2], edges[6], color);
		addLineVertexBatch(edges[6], edges[4], color);
		addLineVertexBatch(edges[4], edges[0], color);
		addLineVertexBatch(edges[1], edges[0], color);
		addLineVertexBatch(edges[3], edges[2], color);
		addLineVertexBatch(edges[7], edges[6], color);
		addLineVertexBatch(edges[5], edges[4], color);
	}

	void CLineDrawData::addLineVertexBatch(const core::vector3df& v1, const core::vector3df& v2, const SColor& color)
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