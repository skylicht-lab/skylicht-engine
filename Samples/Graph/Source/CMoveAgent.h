#pragma once

#include "Components/CComponentSystem.h"
#include "ObstacleAvoidance/CObstacleAvoidance.h"

class CMoveAgent : public CComponentSystem
{
protected:
	Graph::CObstacleAvoidance* m_obstacle;

	core::vector3df m_targetPosition;

	float m_agentRadius;

public:
	CMoveAgent();

	virtual ~CMoveAgent();

	virtual void initComponent();

	virtual void updateComponent();

	void setPosition(const core::vector3df& position);

	inline void setTargetPosition(const core::vector3df& position)
	{
		m_targetPosition = position;
	}

	inline void setObstacle(Graph::CObstacleAvoidance* obstacle)
	{
		m_obstacle = obstacle;
	}

	inline void setAgentRadius(float radius)
	{
		m_agentRadius = radius;
	}
};