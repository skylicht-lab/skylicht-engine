#include "pch.h"
#include "CLineRenderData.h"

namespace Skylicht
{
	IMPLEMENT_DATA_TYPE_INDEX(CLineRenderData);

	CLineRenderData::CLineRenderData() :
		Width(1.0f),
		Billboard(false),
		Material(NULL)
	{
		MeshBuffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD), EIT_32BIT);
		MeshBuffer->setHardwareMappingHint(EHM_STREAM);
	}

	CLineRenderData::~CLineRenderData()
	{
		if (MeshBuffer)
			MeshBuffer->drop();
	}
}