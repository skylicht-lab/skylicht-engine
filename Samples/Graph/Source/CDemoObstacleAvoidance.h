#pragma once

#include "CDemo.h"
#include "ObstacleAvoidance/CObstacleAvoidance.h"

class CDemoObstacleAvoidance : public CDemo
{
protected:
	int m_state;

	Graph::CObstacleAvoidance* obstacle;

	CGameObject* m_agent;

	core::vector3df m_lastClickPosition;

public:
	CDemoObstacleAvoidance(CZone* zone);

	virtual ~CDemoObstacleAvoidance();

	virtual void init();

	virtual void update();

	virtual void onGUI();

	virtual void onLeftClickPosition(const core::vector3df& pos);

	void setAgentPosition(const core::vector3df& pos);
};