#ifndef _GRID_PLANE_H_
#define _GRID_PLANE_H_

#include "Components/CComponentSystem.h"
#include "CGridPlaneData.h"

namespace Skylicht
{
	class CGridPlane : public CComponentSystem
	{
	protected:
		CGridPlaneData *m_gridData;

	public:
		CGridPlane();

		virtual ~CGridPlane();

		virtual void initComponent();

		virtual void updateComponent();

		void setGridData(int numGrid, float gridSize);
	};
}

#endif