#pragma once

#include "Entity/IEntityData.h"
#include "DataTypeIndex.h"
#include "Transform/CWorldTransformData.h"

namespace Skylicht
{
	class COMPONENT_API CLineRenderData : public IEntityData
	{
	public:
		core::array<CWorldTransformData*> Points;

	public:
		CLineRenderData();

		virtual ~CLineRenderData();
	};

	DECLARE_COMPONENT_DATA_TYPE_INDEX(CLineRenderData);
}