#pragma once
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

	float m_stepHeight;
	float m_stepTime;

	std::vector<CLeg*> m_legs;

	bool m_drawDebug;

	core::vector3df m_lastPosition;

public:
	CLegController();

	virtual ~CLegController();

	virtual void initComponent();

	virtual void updateComponent();

	CLeg* addLeg(CWorldTransformData* root, CWorldTransformData* leg);

	virtual void lateUpdate();
};