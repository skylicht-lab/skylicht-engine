#include "pch.h"
#include "SkylichtEngine.h"
#include "CFireLight.h"

CFireLight::CFireLight() :
	m_timer(0.0f),
	m_delayChangeIntensity(0.5f),
	m_radius(-1.0f)
{

}

CFireLight::~CFireLight()
{

}

void CFireLight::initComponent()
{
	m_timer = os::Randomizer::frand() * m_delayChangeIntensity;
}

void CFireLight::updateComponent()
{
	CPointLight *light = m_gameObject->getComponent<CPointLight>();
	if (m_radius < 0.0f)
	{
		m_radius = light->getRadius();
		m_targetRadius = m_radius;
	}

	m_timer = m_timer + getTimeStep();
	if (m_timer > m_delayChangeIntensity)
	{
		m_targetRadius = m_radius * 0.7f + os::Randomizer::frand() * m_radius * 0.3f;
		m_timer = 0.0f;
	}

	float r = light->getRadius() + (m_targetRadius - light->getRadius()) * getTimeStep() * 0.02f;
	light->setRadius(r);
}