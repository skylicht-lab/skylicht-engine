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
	m_tileHeight(2.0f)
{
	m_builder = new Graph::CRecastBuilder();
	m_obstacle = new Graph::CObstacleAvoidance();
	m_walkingMap = new Graph::CWalkingMap();
	m_outputMesh = new CMesh();
}

CDemoNavMesh::~CDemoNavMesh()
{
	if (m_recastMesh)
		delete m_recastMesh;
	delete m_builder;
	delete m_obstacle;
	delete m_outputMesh;
	delete m_walkingMap;
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

	m_walkingMap->release();
	m_outputMesh->removeAllMeshBuffer();
	m_obstacle->clear();

	m_agent->setVisible(true);
	m_agent->getTransformEuler()->setPosition(core::vector3df(0.0f, 1.0f, 0.0f));

	if (m_map)
		m_map->setVisible(true);

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
		debug->addPosition(m_clickPosition, 0.25f, SColor(255, 0, 200, 0));
	}

	SColor red(255, 100, 0, 0);
	SColor white(255, 100, 100, 100);
	SColor green(255, 0, 100, 0);

	// draw debug recastmesh
	/*
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
	*/

	// draw recast polymesh
	for (u32 i = 0, n = m_outputMesh->getMeshBufferCount(); i < n; i++)
	{
		IMeshBuffer* mb = m_outputMesh->getMeshBuffer(i);
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

	// draw bound obstacle
	core::array<Graph::SObstacleSegment>& segments = m_obstacle->getSegments();
	for (u32 i = 0, n = segments.size(); i < n; i++)
	{
		debug->addLine(segments[i].Begin, segments[i].End, red);
	}

	m_walkingMap->resetVisit();

	// draw tilemap
	core::array<Graph::STile*>& tiles = m_walkingMap->getTiles();
	for (u32 i = 0, n = tiles.size(); i < n; i++)
	{
		Graph::STile* tile = tiles[i];

		for (u32 j = 0, n = tile->Neighbours.size(); j < n; j++)
		{
			Graph::STile* nei = tile->Neighbours[j];
			if (nei->Visit == false)
			{
				debug->addLine(tile->Position, nei->Position, green);
			}
		}

		tile->Visit = true;
	}

	m_walkingMap->resetVisit();
}

void CDemoNavMesh::onGUI()
{
	ImGui::Text("Step 1");

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

	ImGui::Text("Step 2");

	if (ImGui::CollapsingHeader("Config Tile"))
	{
		ImGui::SliderFloat("TileWidth", &m_tileWidth, 0.5f, 4.0f);
		ImGui::SliderFloat("TileHeight", &m_tileHeight, 0.5f, 4.0f);
	}

	if (ImGui::Button("Build WalkingMap"))
	{
		buildWalkingMap();
	}

	ImGui::Spacing();
	ImGui::Spacing();
}

void CDemoNavMesh::onLeftClickPosition(bool holdShift, const core::vector3df& pos)
{
	m_clickPosition = pos;
}

void CDemoNavMesh::buildNavMesh()
{
	m_builder->build(m_recastMesh, m_outputMesh, m_obstacle);
}

void CDemoNavMesh::buildWalkingMap()
{
	m_walkingMap->generate(m_tileWidth, m_tileHeight, m_outputMesh);
}
