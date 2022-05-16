#pragma once

#include "ViewManager/CView.h"

class CViewDemo :
	public CView,
	public IEventReceiver
{
protected:
	float m_mouseX;
	float m_mouseY;

	int m_currentTest;
	float m_bboxSizeX;
	float m_bboxSizeY;
	float m_bboxSizeZ;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual bool OnEvent(const SEvent& event);

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void onGUI();
};