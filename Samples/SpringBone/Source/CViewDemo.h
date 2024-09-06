#pragma once

#include "ViewManager/CView.h"
#include "Verlet/CVerlet.h"

class CViewDemo : public CView
{
protected:
	Verlet::CVerlet m_verlet;

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

	void initDemo1();

};