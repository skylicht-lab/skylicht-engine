#include "pch.h"
#include "CDemoObstacleAvoidance.h"

#include "imgui.h"

#include "Primitive/CCapsule.h"
#include "IndirectLighting/CIndirectLighting.h"

#include "Debug/CSceneDebug.h"

CDemoObstacleAvoidance::CDemoObstacleAvoidance(CZone* zone) :
	CDemo(zone),
	m_state(0),
	m_agent(NULL),
	obstacle(NULL)
{
	obstacle = new Graph::CObstacleAvoidance();
}

CDemoObstacleAvoidance::~CDemoObstacleAvoidance()
{
	delete obstacle;
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

		// indirect lighting
		m_agent->addComponent<CIndirectLighting>();
	}

	setAgentPosition(core::vector3df());

	m_lastClickPosition.set(0.0f, 0.0f, 0.0f);
}

void CDemoObstacleAvoidance::setAgentPosition(const core::vector3df& pos)
{
	m_agent->getTransformEuler()->setPosition(pos + core::vector3df(0.0f, 1.0f, 0.0f));
}

void CDemoObstacleAvoidance::update()
{
	if (m_lastClickPosition.getLength() > 0.0f)
	{
		CSceneDebug* debug = CSceneDebug::getInstance()->getNoZDebug();
		debug->addPosition(m_lastClickPosition, 0.25f, SColor(255, 0, 200, 0));
	}
}

void CDemoObstacleAvoidance::onGUI()
{
	ImGui::RadioButton("Do Nothing", &m_state, 0);
	ImGui::RadioButton("Create Wall", &m_state, 1);
	ImGui::RadioButton("Move Agent", &m_state, 2);

	if (m_state == 1)
	{
		ImGui::Text("Left Click on scene to add Wall Segment Point");
	}
	else if (m_state == 2)
	{
		ImGui::Text("Left Click on scene to move Agent");
	}

	ImGui::Spacing();
}

void CDemoObstacleAvoidance::onLeftClickPosition(const core::vector3df& pos)
{
	m_lastClickPosition = pos;
}