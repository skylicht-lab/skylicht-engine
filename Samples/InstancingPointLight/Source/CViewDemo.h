#pragma once

#include "ViewManager/CView.h"
#include "Primitive/CCube.h"

class CViewDemo : public CView
{
protected:
	CCube* m_cubes;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void onGUI();
};
