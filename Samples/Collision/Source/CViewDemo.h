#pragma once

#include "SkylichtEngine.h"
#include "ViewManager/CView.h"
#include "Decal/CDecals.h"

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

	float m_decalSizeX;
	float m_decalSizeY;
	float m_decalSizeZ;
	float m_decalLifeTime;
	float m_decalRotation;

	bool m_addDecal;

	CDecals* m_decals;

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