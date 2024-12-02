#pragma once

#include "CDemo.h"
#include "ObstacleAvoidance/CObstacleAvoidance.h"

class CDemoObstacleAvoidance : public CDemo
{
protected:
	int m_state;

	Graph::CObstacleAvoidance* m_obstacle;

	CGameObject* m_map;
	CGameObject* m_agent;

	core::vector3df m_lastClickPosition;
	core::vector3df m_clickPosition;
	int m_clickCount;

	float m_circleRadius;

public:
	CDemoObstacleAvoidance(CZone* zone);

	virtual ~CDemoObstacleAvoidance();

	virtual void init();

	virtual void close();

	virtual void update();

	virtual void onGUI();

	virtual void onViewRayClick(const core::line3df& ray, int button, bool holdShift);

	virtual void onLeftClickPosition(const core::vector3df& pos, bool holdShift);
};