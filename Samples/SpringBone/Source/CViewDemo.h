#pragma once

#include "ViewManager/CView.h"
#include "Verlet/CVerlet.h"
#include "Wind/CWind.h"

class CViewDemo : public CView
{
protected:
	Verlet::CVerlet m_verlet;
	Verlet::CWind m_wind;

	bool m_moveParticle;
	float m_moveValue;
	float m_moveRadius;
	int m_moveId;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	void onGUI();

protected:

	void initDemo();

	void updateDemo();

};