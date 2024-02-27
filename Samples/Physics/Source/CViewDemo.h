#pragma once

#include "ViewManager/CView.h"

class CViewDemo :
	public CView,
	public IEventReceiver
{
protected:
	int m_mouseX;
	int m_mouseY;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual bool OnEvent(const SEvent& event);
};