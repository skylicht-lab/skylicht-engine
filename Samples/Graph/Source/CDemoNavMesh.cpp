#include "pch.h"
#include "CDemoNavMesh.h"

#include "imgui.h"

#include "Primitive/CCapsule.h"
#include "IndirectLighting/CIndirectLighting.h"

#include "Debug/CSceneDebug.h"

#include "CMoveAgent.h"

CDemoNavMesh::CDemoNavMesh(CZone* zone) :
	CDemo(zone),
	m_state(0),
	m_agent(NULL),
	m_map(NULL)
{

}

CDemoNavMesh::~CDemoNavMesh()
{

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
	CSceneDebug* debug = CSceneDebug::getInstance()->getNoZDebug();

	if (m_clickPosition.getLength() > 0.0f)
	{
		debug->addPosition(m_clickPosition, 0.25f, SColor(255, 0, 200, 0));
	}
}

void CDemoNavMesh::onGUI()
{
	if (ImGui::Button("Build NavMesh"))
	{

	}

	ImGui::Spacing();
	ImGui::Spacing();
}

void CDemoNavMesh::onLeftClickPosition(bool holdShift, const core::vector3df& pos)
{
	m_clickPosition = pos;
}