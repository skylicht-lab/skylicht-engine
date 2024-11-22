#pragma once

#include "CDemo.h"
#include "ObstacleAvoidance/CObstacleAvoidance.h"
#include "RecastMesh/CRecastMesh.h"
#include "RecastMesh/CRecastBuilder.h"

class CDemoNavMesh : public CDemo
{
protected:
	int m_state;

	CGameObject* m_map;
	CGameObject* m_agent;
	core::vector3df m_clickPosition;

	Graph::CRecastMesh* m_recastMesh;
	Graph::CRecastBuilder* m_builder;
	Graph::CObstacleAvoidance* m_obstacle;

	CMesh* m_outputMesh;

public:
	CDemoNavMesh(CZone* zone);

	virtual ~CDemoNavMesh();

	virtual void init();

	virtual void close();

	virtual void update();

	virtual void onGUI();

	virtual void onLeftClickPosition(bool holdShift, const core::vector3df& pos);

	void buildNavMesh();
};