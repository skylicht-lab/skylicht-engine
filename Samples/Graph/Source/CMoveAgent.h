#pragma once

#include "Components/CComponentSystem.h"
#include "ObstacleAvoidance/CObstacleAvoidance.h"
#include "Graph/CGraphQuery.h"

class CMoveAgent : public CComponentSystem
{
protected:
	Graph::CObstacleAvoidance* m_obstacle;
	Graph::CGraphQuery* m_graphQuery;

	core::array<core::vector3df> m_points;

	float m_distance;
	float m_moveTime;

	core::vector3df m_targetPosition;

	float m_agentRadius;

public:
	CMoveAgent();

	virtual ~CMoveAgent();

	virtual void initComponent();

	virtual void updateComponent();

	void setPosition(const core::vector3df& position);

	core::vector3df getPosition();

	inline void setGraphQuery(Graph::CGraphQuery* query)
	{
		m_graphQuery = query;
	}

	inline void setTargetPosition(const core::vector3df& position)
	{
		m_targetPosition = position;
	}

	inline void setObstacle(Graph::CObstacleAvoidance* obstacle)
	{
		if (m_obstacle)
		{
			m_obstacle->clear();
			m_obstacle->addSegments(obstacle->getSegments());
		}
	}

	inline void setAgentRadius(float radius)
	{
		m_agentRadius = radius;
	}

	void setPath(const core::array<Graph::STile*>& path, const core::vector3df& target);

	inline void clearPath()
	{
		m_points.clear();
		m_distance = 0.0f;
		m_moveTime = 0.0f;
	}

protected:

	core::vector3df folowPath();

};