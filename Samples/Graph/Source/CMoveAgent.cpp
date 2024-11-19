#include "pch.h"
#include "CMoveAgent.h"

#include "GameObject/CGameObject.h"

#include "Utils/CVector.h"

CMoveAgent::CMoveAgent() :
	m_obstacle(NULL),
	m_agentRadius(0.5f)
{

}

CMoveAgent::~CMoveAgent()
{

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
		core::vector3df velocity = newPosition - position;
		newPosition = m_obstacle->collide(position, velocity, m_agentRadius);
	}

	transform->setPosition(newPosition + heightOffset);
}