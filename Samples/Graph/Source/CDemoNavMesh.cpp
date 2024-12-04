#include "pch.h"
#include "CDemoNavMesh.h"

#include "imgui.h"

#include "Primitive/CCapsule.h"
#include "IndirectLighting/CIndirectLighting.h"
#include "MeshManager/CMeshManager.h"
#include "Debug/CSceneDebug.h"

#include "CMoveAgent.h"

CDemoNavMesh::CDemoNavMesh(CZone* zone) :
	CDemo(zone),
	m_state(0),
	m_agent(NULL),
	m_map(NULL),
	m_recastMesh(NULL),
	m_tileWidth(2.0f),
	m_tileHeight(2.0f),
	m_pickTile(NULL),
	m_drawDebugRecastMesh(false),
	m_drawDebugNavMesh(true),
	m_drawDebugObstacle(true),
	m_drawDebugQueryBox(false),
	m_drawDebugTileMap(true)
{
	m_builder = new Graph::CRecastBuilder();
	m_obstacle = new Graph::CObstacleAvoidance();
	m_walkingTileMap = new Graph::CWalkingTileMap();
	m_query = new Graph::CGraphQuery();
	m_outputNavMesh = new CMesh();
}

CDemoNavMesh::~CDemoNavMesh()
{
	if (m_recastMesh)
		delete m_recastMesh;
	delete m_builder;
	delete m_obstacle;
	delete m_outputNavMesh;
	delete m_walkingTileMap;
	delete m_query;
}

void CDemoNavMesh::init()
{
	if (m_map == NULL)
		m_map = m_zone->searchObjectInChild(L"Map");

	if (m_agent == NULL)
	{
		m_agent = m_zone->createEmptyObject();

		CCapsule* capsule = m_agent->addComponent<CCapsule>();
		capsule->setRadius(0.5f);
		capsule->setHeight(0.5f);
		capsule->init();

		m_agent->addComponent<CIndirectLighting>();
		m_agent->addComponent<CMoveAgent>();
	}

	if (m_recastMesh == NULL)
	{
		m_recastMesh = new Graph::CRecastMesh();

		CEntityPrefab* mapPrefab = CMeshManager::getInstance()->loadModel(COLLISION_MODEL, "");
		if (mapPrefab)
			m_recastMesh->addMeshPrefab(mapPrefab, core::IdentityMatrix);
	}

	m_walkingTileMap->release();
	m_outputNavMesh->removeAllMeshBuffer();
	m_obstacle->clear();

	m_agent->setVisible(true);
	m_agent->getTransformEuler()->setPosition(core::vector3df(0.0f, 1.0f, 0.0f));

	if (m_map)
		m_map->setVisible(true);

	m_agent->getComponent<CMoveAgent>()->setGraphQuery(m_query);

	m_clickPosition.set(0.0f, 0.0f, 0.0f);
}

void CDemoNavMesh::close()
{
	m_agent->setVisible(false);

	if (m_map)
		m_map->setVisible(false);
}

void CDemoNavMesh::update()
{
	CSceneDebug* debug = CSceneDebug::getInstance();

	if (m_clickPosition.getLength() > 0.0f)
	{
		debug->getNoZDebug()->addPosition(m_clickPosition, 0.25f, SColor(255, 0, 200, 0));
	}

	SColor red(255, 100, 0, 0);
	SColor grey(255, 20, 20, 20);
	SColor white(255, 100, 100, 100);
	SColor green(255, 0, 10, 0);
	SColor greenL(255, 0, 100, 0);
	SColor yellow(255, 100, 100, 0);

	// draw debug recastmesh
	if (m_drawDebugRecastMesh)
	{
		int* tris = m_recastMesh->getTris();
		float* verts = m_recastMesh->getVerts();

		for (u32 i = 0, n = m_recastMesh->getTriCount(); i < n; i++)
		{
			int a = tris[i * 3];
			int b = tris[i * 3 + 1];
			int c = tris[i * 3 + 2];

			core::vector3df va(verts[a * 3], verts[a * 3 + 1], verts[a * 3 + 2]);
			core::vector3df vb(verts[b * 3], verts[b * 3 + 1], verts[b * 3 + 2]);
			core::vector3df vc(verts[c * 3], verts[c * 3 + 1], verts[c * 3 + 2]);

			debug->addLine(va, vb, white);
			debug->addLine(vb, vc, white);
			debug->addLine(vc, va, white);
		}
	}

	// draw nav polymesh
	if (m_drawDebugNavMesh)
	{
		for (u32 i = 0, n = m_outputNavMesh->getMeshBufferCount(); i < n; i++)
		{
			IMeshBuffer* mb = m_outputNavMesh->getMeshBuffer(i);
			IVertexBuffer* vb = mb->getVertexBuffer();
			IIndexBuffer* ib = mb->getIndexBuffer();

			for (u32 i = 0, n = ib->getIndexCount(); i < n; i += 3)
			{
				u32 a = ib->getIndex(i);
				u32 b = ib->getIndex(i + 1);
				u32 c = ib->getIndex(i + 2);

				S3DVertex* p1 = (S3DVertex*)vb->getVertex(a);
				S3DVertex* p2 = (S3DVertex*)vb->getVertex(b);
				S3DVertex* p3 = (S3DVertex*)vb->getVertex(c);

				debug->addLine(p1->Pos, p2->Pos, white);
				debug->addLine(p2->Pos, p3->Pos, white);
				debug->addLine(p3->Pos, p1->Pos, white);
			}
		}
	}

	// draw bound obstacle
	if (m_drawDebugObstacle)
	{
		core::array<core::line3df>& segments = m_obstacle->getSegments();
		for (u32 i = 0, n = segments.size(); i < n; i++)
		{
			debug->addLine(segments[i], red);
		}
	}

	// draw query box
	if (m_drawDebugQueryBox)
	{
		if (m_query->getRootNode())
		{
			std::queue<Graph::COctreeNode*> nodes;
			nodes.push(m_query->getRootNode());

			while (nodes.size() > 0)
			{
				Graph::COctreeNode* node = nodes.front();
				nodes.pop();

				debug->addBoudingBox(node->Box, grey);

				for (int i = 0; i < 8; i++)
				{
					if (node->Childs[i])
						nodes.push(node->Childs[i]);
				}
			}
		}
	}

	// draw tilemap
	if (m_drawDebugTileMap)
	{
		core::vector3df halfHeight(0.0f, 0.2f, 0.0f);
		m_walkingTileMap->resetVisit();
		core::array<Graph::STile*>& tiles = m_walkingTileMap->getTiles();
		for (u32 i = 0, n = tiles.size(); i < n; i++)
		{
			Graph::STile* tile = tiles[i];
			debug->addLine(tile->Position, tile->Position + halfHeight, yellow);

			for (u32 j = 0, n = tile->Neighbours.size(); j < n; j++)
			{
				Graph::STile* nei = tile->Neighbours[j];
				if (nei->Visit == false)
					debug->addLine(tile->Position, nei->Position, green);
			}

			tile->Visit = true;
		}
		m_walkingTileMap->resetVisit();
	}

	if (m_pickTile)
	{
		debug->addBoudingBox(m_pickTile->BBox, greenL);
		debug->addLine(m_clickPosition, m_pickTile->Position, red);
	}
}

void CDemoNavMesh::onGUI()
{
	ImGui::Text("Demo NavMesh");
	ImGui::Text("After build NavMesh");
	ImGui::Text("- Left mouse to set Agent position");
	ImGui::Text("- Right mouse to move Agent");

	if (ImGui::CollapsingHeader("Draw Debug"))
	{
		ImGui::Checkbox("Recast mesh", &m_drawDebugRecastMesh);
		ImGui::Checkbox("Navmesh", &m_drawDebugNavMesh);
		ImGui::Checkbox("Obstacle", &m_drawDebugObstacle);
		ImGui::Checkbox("Query box", &m_drawDebugQueryBox);
		ImGui::Checkbox("Walk tilemap", &m_drawDebugTileMap);
	}

	ImGui::Text("Build step 1");

	if (ImGui::CollapsingHeader("Config NavMesh"))
	{
		Graph::SBuilderConfig config = m_builder->getConfig();
		ImGui::SliderFloat("CellSize", &config.CellSize, 0.2f, 1.0f);
		ImGui::SliderFloat("CellHeight", &config.CellHeight, 0.2f, 1.0f);
		ImGui::SliderFloat("AgentHeight", &config.AgentHeight, 0.5f, 2.0f);
		ImGui::SliderFloat("AgentRadius", &config.AgentRadius, 0.1f, 1.0f);
		ImGui::SliderFloat("AgentMaxClimb", &config.AgentMaxClimb, 0.1f, 1.0f);
		ImGui::SliderFloat("AgentMaxSlope", &config.AgentMaxClimb, 5.0f, 60.0f);
		m_builder->setConfig(config);
	}

	if (ImGui::Button("Build NavMesh"))
	{
		buildNavMesh();
	}

	ImGui::Spacing();

	ImGui::Text("Build step 2");

	if (ImGui::CollapsingHeader("Config Tile"))
	{
		ImGui::SliderFloat("TileWidth", &m_tileWidth, 0.5f, 4.0f);
		ImGui::SliderFloat("TileHeight", &m_tileHeight, 0.5f, 4.0f);
	}

	if (ImGui::Button("Build Walking TileMap"))
	{
		buildWalkingMap();
	}

	ImGui::Spacing();
	ImGui::Spacing();
}

void CDemoNavMesh::onViewRayClick(const core::line3df& ray, int button, bool holdShift)
{
	core::vector3df outIntersection;
	core::triangle3df outTriangle;
	float rayDistance = 10000.0f;

	if (m_query->getCollisionPoint(ray, rayDistance, outIntersection, outTriangle) == true)
	{
		m_clickPosition = outIntersection;
		m_pickTile = m_walkingTileMap->getTileByPosition(m_clickPosition);

		CMoveAgent* moveAgent = m_agent->getComponent<CMoveAgent>();

		if (button == 0)
		{
			// left click
			moveAgent->setPosition(m_clickPosition);
			moveAgent->setTargetPosition(m_clickPosition);
		}
		else
		{
			// right click
			moveAgent->setTargetPosition(m_clickPosition);
		}
	}
}

void CDemoNavMesh::buildNavMesh()
{
	m_builder->build(m_recastMesh, m_outputNavMesh, m_obstacle);
	m_query->buildIndexNavMesh(m_outputNavMesh, m_obstacle);
}

void CDemoNavMesh::buildWalkingMap()
{
	m_walkingTileMap->generate(m_tileWidth, m_tileHeight, m_outputNavMesh, m_obstacle);
}
