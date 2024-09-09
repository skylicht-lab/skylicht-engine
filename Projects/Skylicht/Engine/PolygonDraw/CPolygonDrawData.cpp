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
#include "CPolygonDrawData.h"

namespace Skylicht
{
	CPolygonDrawData::CPolygonDrawData()
	{
		PolygonBuffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD), video::EIT_32BIT);
		PolygonBuffer->getMaterial().MaterialType = CShaderManager::getInstance()->getShaderIDByName("VertexColorAlpha");
		PolygonBuffer->getMaterial().BackfaceCulling = false;
		PolygonBuffer->setHardwareMappingHint(EHM_STREAM);
	}

	CPolygonDrawData::~CPolygonDrawData()
	{
		PolygonBuffer->drop();
	}

	void CPolygonDrawData::clearBuffer()
	{
		PolygonBuffer->getVertexBuffer()->set_used(0);
		PolygonBuffer->getIndexBuffer()->set_used(0);
	}

	void CPolygonDrawData::updateBuffer()
	{
		PolygonBuffer->setDirty();
	}

	void CPolygonDrawData::addPolygonFill(core::vector3df* points, u32 count, const SColor& color)
	{
		IVertexBuffer* vtxBuffer = PolygonBuffer->getVertexBuffer();
		IIndexBuffer* idxBuffer = PolygonBuffer->getIndexBuffer();

		u32 idxCount = vtxBuffer->getVertexCount();

		video::S3DVertex vert;
		vert.Color = color;

		for (u32 i = 0; i < count; i++)
		{
			vert.Pos = points[i];
			vtxBuffer->addVertex(&vert);
		}

		for (u32 i = 2; i < count; i++)
		{
			idxBuffer->addIndex(idxCount);
			idxBuffer->addIndex(idxCount + i - 1);
			idxBuffer->addIndex(idxCount + i);
		}
	}

	void CPolygonDrawData::addTriangleFill(const core::vector3df& a, const core::vector3df& b, const core::vector3df& c, const SColor& color)
	{
		core::vector3df points[] = { a, b, c };
		addPolygonFill(points, 3, color);
	}
}