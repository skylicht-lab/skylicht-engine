#include "pch.h"
#include "CLineRenderData.h"

namespace Skylicht
{
	IMPLEMENT_DATA_TYPE_INDEX(CLineRenderData);

	CLineRenderData::CLineRenderData() :
		Width(1.0f),
		Billboard(false),
		Mesh(NULL),
		Material(NULL),
		NeedValidate(true)
	{
		Mesh = new CMesh();

		MeshBuffer = new CMeshBuffer<video::S3DVertex>(getVideoDriver()->getVertexDescriptor(EVT_STANDARD), EIT_32BIT);
		MeshBuffer->setHardwareMappingHint(EHM_STREAM);

		Mesh->addMeshBuffer(MeshBuffer, "default");
	}

	CLineRenderData::~CLineRenderData()
	{
		if (Mesh)
			Mesh->drop();

		if (MeshBuffer)
			MeshBuffer->drop();
	}
}