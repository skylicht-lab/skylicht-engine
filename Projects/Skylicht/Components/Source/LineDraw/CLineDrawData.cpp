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
		int idxCount = LineBuffer->getVertexBuffer()->getVertexCount();

		video::S3DVertex vert;
		vert.Color = color;

		vert.Pos = v1;
		LineBuffer->getVertexBuffer()->addVertex(&vert);
		vert.Pos = v2;
		LineBuffer->getVertexBuffer()->addVertex(&vert);

		LineBuffer->getIndexBuffer()->addIndex(idxCount++);
		LineBuffer->getIndexBuffer()->addIndex(idxCount);
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