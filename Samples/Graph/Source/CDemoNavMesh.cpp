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
	m_recastMesh(NULL)
{
	m_builder = new Graph::CRecastBuilder();
	m_obstacle = new Graph::CObstacleAvoidance();
	m_tileMap = new Graph::CTileMap();
	m_outputMesh = new CMesh();
}

CDemoNavMesh::~CDemoNavMesh()
{
	if (m_recastMesh)
		delete m_recastMesh;
	delete m_builder;
	delete m_obstacle;
	delete m_outputMesh;
	delete m_tileMap;
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

	SColor red(255, 255, 0, 0);
	SColor white(255, 100, 100, 100);
	SColor green(255, 0,255,0);

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
	
	// draw tilemap
	core::array<Graph::STile*>& tiles = m_tileMap->getTiles();
	for (u32 i = 0, n = tiles.size(); i < n; i++)
	{
		Graph::STile* t = tiles[i];
		debug->addPosition(t->Position, 0.2f, green);
	}
}

void CDemoNavMesh::onGUI()
{
	if (ImGui::Button("Step 1 - Build NavMesh"))
	{
		buildNavMesh();
	}

	if (ImGui::Button("Step 2 - Build TileMap"))
	{
		buildTileMap();
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

void CDemoNavMesh::buildTileMap()
{
	m_tileMap->generate(1.0, 1.0, m_outputMesh);
}
