#pragma once

class CFireLight : public CComponentSystem
{
protected:
	float m_timer;
	float m_delayChangeIntensity;
	float m_radius;
	float m_targetRadius;

public:
	CFireLight();

	virtual ~CFireLight();

	virtual void initComponent();

	virtual void updateComponent();
};