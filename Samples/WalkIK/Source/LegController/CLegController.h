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

	bool m_drawDebug;

	float m_targetDistance;
	float m_moveStepDistance;
	float m_footStepLength;
	float m_stepHeight;
	float m_stepTime;

	float m_rotDirection;
	float m_rotTime;
	float m_moveTime;
	float m_standTime;

	CWorldTransformData* m_root;
	std::vector<CLeg*> m_legs;

	core::vector3df m_lastPosition;
	core::quaternion m_lastRotation;

public:
	CLegController();

	virtual ~CLegController();

	virtual void initComponent();

	virtual void updateComponent();

	CLeg* addLeg(CWorldTransformData* root, CWorldTransformData* leg);

	inline CWorldTransformData* getRoot()
	{
		return m_root;
	}

	inline std::vector<CLeg*>& getLegs()
	{
		return m_legs;
	}

	inline void setTargetDistance(float distance)
	{
		m_targetDistance = distance;
	}

	inline float getTargetDistance()
	{
		return m_targetDistance;
	}

	virtual void lateUpdate();
};