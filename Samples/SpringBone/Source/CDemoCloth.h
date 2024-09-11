#pragma once

#include "CDemo.h"

class CDemoCloth : public CDemo
{
protected:
	bool m_moveParticle;
	float m_moveValue;
	float m_moveRadius;
	int m_moveId;

public:
	CDemoCloth(Verlet::CVerlet* verlet);

	virtual ~CDemoCloth();

	virtual void init();

	virtual void update();

	virtual void onGUI();
};