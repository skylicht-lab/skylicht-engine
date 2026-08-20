#pragma once

#include "Lighting/CPointLight.h"
#include "SpriteDraw/CSprite.h"

class CLightBlinkComponent : public CComponentSystem
{
protected:
	CPointLight* m_pointLight;
	CSprite* m_sprite;
	float m_onIntensity;
	float m_time;
	float m_duration;
	float m_minDuration;
	float m_maxDuration;
	bool m_lightOn;

protected:
	float getRandomDuration();
	void applyLightState();
	void switchLight();

public:
	CLightBlinkComponent();

	virtual ~CLightBlinkComponent();

	virtual void initComponent();

	virtual void updateComponent();

	void setDuration(float minMs, float maxMs);
};
