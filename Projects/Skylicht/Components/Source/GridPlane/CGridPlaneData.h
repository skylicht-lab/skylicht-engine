#pragma once

#include "Entity/IEntityData.h"
#include "LineDraw/CLineDrawData.h"

namespace Skylicht
{
	class CGridPlaneData :
		public CLineDrawData,
		public IEntityData
	{
	public:
		int NumGrid;
		float GridSize;
		DECLARE_DATA_TYPE_INDEX;

	public:
		CGridPlaneData();

		virtual ~CGridPlaneData();

		void init();
	};
}