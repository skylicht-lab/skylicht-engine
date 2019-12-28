#pragma once

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