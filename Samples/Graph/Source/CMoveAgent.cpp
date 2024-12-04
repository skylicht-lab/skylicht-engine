#include "pch.h"
#include "CMoveAgent.h"

#include "GameObject/CGameObject.h"
#include "Debug/CSceneDebug.h"
#include "Utils/CVector.h"

CMoveAgent::CMoveAgent() :
	m_obstacle(NULL),
	m_graphQuery(NULL),
	m_agentRadius(0.5f)
{
	m_obstacle = new Graph::CObstacleAvoidance();
}

CMoveAgent::~CMoveAgent()
{
	delete m_obstacle;
}

void CMoveAgent::initComponent()
{

}

void CMoveAgent::updateComponent()
{
	core::vector3df heightOffset(0.0f, 1.0f, 0.0f);
	CTransformEuler* transform = m_gameObject->getTransformEuler();

	core::vector3df position = transform->getPosition() - heightOffset;
	core::vector3df newPosition = CVector::lerp(position, m_targetPosition, 0.001f * getTimeStep());

	if (m_obstacle)
	{
		if (m_graphQuery)
		{
			core::aabbox3df box;
			core::vector3df r(m_agentRadius, 0.0f, m_agentRadius);

			box.MinEdge = newPosition - r;
			box.MaxEdge = newPosition + core::vector3df(0.0f, 2.0f, 0.0f) + r;

			m_obstacle->clear();
			m_graphQuery->getObstacles(box, *m_obstacle);

			/*
			core::array<core::line3df>& segs = m_obstacle->getSegments();
			SColor c(255, 255, 0, 255);
			for (u32 i = 0, n = segs.size(); i < n; i++)
			{
				CSceneDebug::getInstance()->addLine(segs[i], c);
			}
			*/
		}

		core::vector3df velocity = newPosition - position;
		newPosition = m_obstacle->collide(position, velocity);
	}

	transform->setPosition(newPosition + heightOffset);
}

void CMoveAgent::setPosition(const core::vector3df& position)
{
	core::vector3df heightOffset(0.0f, 1.0f, 0.0f);
	CTransformEuler* transform = m_gameObject->getTransformEuler();
	transform->setPosition(position + heightOffset);
}