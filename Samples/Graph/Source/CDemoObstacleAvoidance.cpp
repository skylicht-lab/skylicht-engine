#include "pch.h"
#include "CDemoObstacleAvoidance.h"

#include "imgui.h"

#include "Primitive/CCapsule.h"
#include "IndirectLighting/CIndirectLighting.h"

#include "Debug/CSceneDebug.h"

#include "CMoveAgent.h"

CDemoObstacleAvoidance::CDemoObstacleAvoidance(CZone* zone) :
	CDemo(zone),
	m_state(0),
	m_agent(NULL),
	m_map(NULL),
	m_obstacle(NULL),
	m_clickCount(0),
	m_circleRadius(1.0f)
{
	m_obstacle = new Graph::CObstacleAvoidance();
}

CDemoObstacleAvoidance::~CDemoObstacleAvoidance()
{
	delete m_obstacle;
}

void CDemoObstacleAvoidance::init()
{
	if (m_map == NULL)
		m_map = m_zone->searchObjectInChild(L"Plane");

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

	m_agent->setVisible(true);
	m_agent->getTransformEuler()->setPosition(core::vector3df(0.0f, 1.0f, 0.0f));
	m_agent->getComponent<CMoveAgent>()->setTargetPosition(core::vector3df());

	if (m_map)
		m_map->setVisible(true);

	m_clickPosition.set(0.0f, 0.0f, 0.0f);
	m_lastClickPosition.set(0.0f, 0.0f, 0.0f);
	m_clickCount = 0;
	m_obstacle->clear();
}

void CDemoObstacleAvoidance::close()
{
	m_agent->setVisible(false);

	if (m_map)
		m_map->setVisible(false);
}

void CDemoObstacleAvoidance::update()
{
	CSceneDebug* debug = CSceneDebug::getInstance()->getNoZDebug();

	if (m_clickPosition.getLength() > 0.0f)
	{
		debug->addPosition(m_clickPosition, 0.25f, SColor(255, 0, 200, 0));
	}

	SColor red(255, 200, 0, 0);

	core::array<core::line3df>& segments = m_obstacle->getSegments();
	for (u32 i = 0, n = segments.size(); i < n; i++)
	{
		debug->addLine(segments[i], red);
	}
}

void CDemoObstacleAvoidance::onGUI()
{
	int lastState = m_state;

	ImGui::RadioButton("Do Nothing", &m_state, 0);
	ImGui::RadioButton("Create Wall", &m_state, 1);
	ImGui::RadioButton("Move Agent", &m_state, 2);

	if (lastState != m_state)
	{
		m_clickCount = 0;
	}

	ImGui::Spacing();

	if (m_state == 1)
	{
		ImGui::Text("Create Wall");
		ImGui::Text("Left Click on scene to add Wall Segment Point");
		ImGui::Text("Hold Shift & Click to break segment");
	}
	else if (m_state == 2)
	{
		ImGui::Text("Move Agent");
		ImGui::Text("Left Click on scene to move Agent");
	}

	ImGui::Spacing();
	ImGui::Spacing();
}

void CDemoObstacleAvoidance::onViewRayClick(const core::line3df& ray, int button, bool holdShift)
{
	core::vector3df collide;

	// plane test
	core::plane3df p(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	if (p.getIntersectionWithLimitedLine(ray.start, ray.end, collide))
	{
		onLeftClickPosition(collide, holdShift);
	}
}

void CDemoObstacleAvoidance::onLeftClickPosition(const core::vector3df& pos, bool holdShift)
{
	m_lastClickPosition = m_clickPosition;
	m_clickPosition = pos;

	if (m_state == 1)
	{
		// Add wall
		if (holdShift)
		{
			m_clickCount = 0;
		}
		else
		{
			if (m_clickCount >= 1)
			{
				m_obstacle->addSegment(m_lastClickPosition, m_clickPosition);
			}
		}

		m_clickCount++;
	}
	if (m_state == 2)
	{
		// Move Agent
		CMoveAgent* move = m_agent->getComponent<CMoveAgent>();
		if (move)
		{
			move->setTargetPosition(m_clickPosition);
			move->setObstacle(m_obstacle);
		}
	}
}