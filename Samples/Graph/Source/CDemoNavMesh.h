#pragma once

#include "CDemo.h"
#include "ObstacleAvoidance/CObstacleAvoidance.h"
#include "RecastMesh/CRecastMesh.h"
#include "RecastMesh/CRecastBuilder.h"
#include "WalkingMap/CWalkingTileMap.h"
#include "Graph/CGraphQuery.h"

class CDemoNavMesh : public CDemo
{
protected:
	int m_state;

	CGameObject* m_map;
	CGameObject* m_agent;

	core::vector3df m_clickPosition;

	Graph::STile* m_fromTile;
	Graph::STile* m_toTile;
	core::array<Graph::STile*> m_path;

	Graph::CRecastMesh* m_recastMesh;
	Graph::CRecastBuilder* m_builder;
	Graph::CObstacleAvoidance* m_obstacle;
	Graph::CWalkingTileMap* m_walkingTileMap;

	Graph::CGraphQuery* m_query;

	float m_tileWidth;
	float m_tileHeight;

	CMesh* m_navMesh;

	bool m_drawDebugRecastMesh;
	bool m_drawDebugNavMesh;
	bool m_drawDebugObstacle;
	bool m_drawDebugQueryBox;
	bool m_drawDebugTileMap;

public:
	CDemoNavMesh(CZone* zone);

	virtual ~CDemoNavMesh();

	virtual void init();

	virtual void close();

	virtual void update();

	virtual void onGUI();

	virtual void onViewRayClick(const core::line3df& ray, int button, bool holdShift);

	void buildNavMesh();

	void buildWalkingMap();
};
