#pragma once

#include "ViewManager/CView.h"
#include "Verlet/CVerlet.h"
#include "Wind/CWind.h"

#include "CDemoTree.h"
#include "CDemoCloth.h"

class CViewDemo : public CView
{
protected:
	Verlet::CVerlet m_verlet;
	Verlet::CWind m_wind;

	CDemo* m_demo[2];

	int m_demoId;
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