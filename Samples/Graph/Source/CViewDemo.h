#pragma once

#include "ViewManager/CView.h"
#include "CDemo.h"

class CViewDemo :
	public CView,
	public IEventReceiver
{
protected:

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

	void initDemo();

	virtual bool OnEvent(const SEvent& event);
};