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

	m_agent->getTransformEuler()->setPosition(core::vector3df(0.0f, 1.0f, 0.0f));

	m_clickPosition.set(0.0f, 0.0f, 0.0f);
	m_lastClickPosition.set(0.0f, 0.0f, 0.0f);
	m_clickCount = 0;
	m_obstacle->clear();
}

void CDemoObstacleAvoidance::update()
{
	CSceneDebug* debug = CSceneDebug::getInstance()->getNoZDebug();

	if (m_clickPosition.getLength() > 0.0f)
	{
		debug->addPosition(m_clickPosition, 0.25f, SColor(255, 0, 200, 0));
	}

	SColor red(255, 200, 0, 0);

	core::array<Graph::SObstacleSegment>& segments = m_obstacle->getSegments();
	for (u32 i = 0, n = segments.size(); i < n; i++)
	{
		debug->addLine(segments[i].Begin, segments[i].End, red);
	}

	core::array<Graph::SObstacleCircle>& circles = m_obstacle->getCircles();
	for (u32 i = 0, n = circles.size(); i < n; i++)
	{
		debug->addCircle(circles[i].Center, circles[i].Radius, Transform::Oy, red);
	}
}

void CDemoObstacleAvoidance::onGUI()
{
	int lastState = m_state;

	ImGui::RadioButton("Do Nothing", &m_state, 0);
	ImGui::RadioButton("Create Wall", &m_state, 1);
	ImGui::RadioButton("Create Circle", &m_state, 2);
	ImGui::RadioButton("Move Agent", &m_state, 3);

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
		ImGui::Text("Create Circle");
		ImGui::Text("Left Click on scene to add Circle");
		ImGui::SliderFloat("Radius", &m_circleRadius, 0.5f, 3.0f);
	}
	else if (m_state == 3)
	{
		ImGui::Text("Move Agent");
		ImGui::Text("Left Click on scene to move Agent");
	}

	ImGui::Spacing();
	ImGui::Spacing();
}

void CDemoObstacleAvoidance::onLeftClickPosition(bool holdShift, const core::vector3df& pos)
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
	else if (m_state == 2)
	{
		// Add Circle
		m_obstacle->addCircle(m_clickPosition, m_circleRadius);
	}
	else if (m_state == 3)
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