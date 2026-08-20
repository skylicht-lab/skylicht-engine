#include "pch.h"
#include "SkylichtEngine.h"
#include "Lighting/CPointLight.h"
#include "SpriteDraw/CSprite.h"
#include "CLightBlinkComponent.h"

CLightBlinkComponent::CLightBlinkComponent() :
	m_pointLight(NULL),
	m_sprite(NULL),
	m_onIntensity(0.0f),
	m_time(0.0f),
	m_duration(0.0f),
	m_minDuration(2000.0f),
	m_maxDuration(3000.0f),
	m_lightOn(true)
{

}

CLightBlinkComponent::~CLightBlinkComponent()
{

}

float CLightBlinkComponent::getRandomDuration()
{
	return m_minDuration + os::Randomizer::frand() * (m_maxDuration - m_minDuration);
}

void CLightBlinkComponent::applyLightState()
{
	if (m_pointLight == NULL)
		return;

	m_pointLight->setIntensity(m_lightOn ? m_onIntensity : 0.0f);
	// m_pointLight->getGameObject()->setVisible(m_lightOn);

	if (m_sprite != NULL)
	{
		SColor spriteColor = m_pointLight->getColor().toSColor();
		if (m_lightOn == false)
			spriteColor.setAlpha(0);

		m_sprite->setColor(spriteColor);
	}
}

void CLightBlinkComponent::switchLight()
{
	m_lightOn = !m_lightOn;
	m_duration = getRandomDuration();
	m_time = 0.0f;
	applyLightState();
}

void CLightBlinkComponent::initComponent()
{
	m_pointLight = m_gameObject->getComponent<CPointLight>();
	if (m_pointLight != NULL)
		m_onIntensity = m_pointLight->getIntensity();

	m_sprite = m_gameObject->getComponent<CSprite>();

	m_duration = getRandomDuration();
	m_time = os::Randomizer::frand() * m_duration;
	applyLightState();
}

void CLightBlinkComponent::updateComponent()
{
	if (m_pointLight == NULL)
		return;

	m_time = m_time + getTimeStep();
	if (m_time >= m_duration)
		switchLight();
}

void CLightBlinkComponent::setDuration(float minMs, float maxMs)
{
	m_minDuration = core::max_(0.0f, minMs);
	m_maxDuration = core::max_(m_minDuration, maxMs);
	m_duration = getRandomDuration();
}
