#pragma once

#include "Entity/IEntityData.h"
#include "DataTypeIndex.h"
#include "Transform/CWorldTransformData.h"
#include "Material/CMaterial.h"

namespace Skylicht
{
	class COMPONENT_API CLineRenderData : public IEntityData
	{
	public:
		float Width;

		bool Billboard;

		IMeshBuffer* MeshBuffer;

		core::array<CWorldTransformData*> Points;

		CMaterial* Material;

	public:
		CLineRenderData();

		virtual ~CLineRenderData();
	};

	DECLARE_COMPONENT_DATA_TYPE_INDEX(CLineRenderData);
}