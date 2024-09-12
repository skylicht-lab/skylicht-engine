#pragma once

#include "Verlet/CVerlet.h"

class CDemo
{
protected:
	Verlet::CVerlet* m_verlet;

	bool m_drawingParticle;
	bool m_enableWind;

public:
	CDemo(Verlet::CVerlet* verlet);

	virtual ~CDemo();

	virtual void init() = 0;

	virtual void update() = 0;

	virtual void onGUI() = 0;

	inline bool isDrawingParticle()
	{
		return m_drawingParticle;
	}

	inline void setDrawingParticle(bool b)
	{
		m_drawingParticle = b;
	}

	inline bool isEnableWind()
	{
		return m_enableWind;
	}

	inline bool setEnableWind(bool b)
	{
		m_enableWind = b;
	}
};