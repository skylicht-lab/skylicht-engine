#ifndef _GRID_PLANE_DATA_H_
#define _GRID_PLANE_DATA_H_

#include "Entity/IEntityData.h"
#include "LineDraw/CLineDrawData.h"

namespace Skylicht
{
	class CGridPlaneData : public CLineDrawData
	{
	public:
		int NumGrid;
		float GridSize;
	public:
		CGridPlaneData();

		virtual ~CGridPlaneData();

		void init();
	};
}

#endif