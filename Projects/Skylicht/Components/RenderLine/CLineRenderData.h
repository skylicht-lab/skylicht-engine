#pragma once

#include "Entity/IEntityData.h"
#include "DataTypeIndex.h"
#include "Transform/CWorldTransformData.h"
#include "Material/CMaterial.h"
#include "RenderMesh/CMesh.h"

namespace Skylicht
{
	class COMPONENT_API CLineRenderData : public IEntityData
	{
	public:
		float Width;

		bool Billboard;

		CMesh* Mesh;

		IMeshBuffer* MeshBuffer;

		core::array<CWorldTransformData*> Points;

		CMaterial* Material;

		bool NeedValidate;

	public:
		CLineRenderData();

		virtual ~CLineRenderData();
	};

	DECLARE_COMPONENT_DATA_TYPE_INDEX(CLineRenderData);
}