#pragma once

#include "LegController/CLeg.h"
#include "RenderMesh/CRenderMesh.h"
#include "Components/CComponentSystem.h"
#include "Components/ILateUpdate.h"

using namespace Skylicht;

class CLegController :
	public CComponentSystem,
	public ILateUpdate
{
protected:
	CRenderMesh* m_renderMesh;

	float m_targetDistance;
	float m_footStepLength;

	std::vector<CLeg*> m_legs;

	bool m_drawDebug;

public:
	CLegController();

	virtual ~CLegController();

	virtual void initComponent();

	virtual void updateComponent();

	void addLeg(CWorldTransformData* leg);

	virtual void lateUpdate();
};