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

	virtual void projectOnGround(core::vector3df& position);

	void resetFootPosition();

	CLeg* addLeg(CWorldTransformData* root, CWorldTransformData* leg);

	inline void setFootStepLength(float length)
	{
		m_footStepLength = length;
	}

	inline float getFootStepLength()
	{
		return m_footStepLength;
	}

	void setStepHeight(float height);

	inline float getStepHeight()
	{
		return m_stepHeight;
	}

	inline void setMoveStepDistance(float length)
	{
		m_moveStepDistance = length;
	}

	inline float getMoveStepDistance()
	{
		return m_moveStepDistance;
	}

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

	core::vector3df getStandFootPosition(CLeg* leg);

	virtual void lateUpdate();
};