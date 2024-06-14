#pragma once

#include "ViewManager/CView.h"

#include "LegController/CLegController.h"
#include "Record/CRecorder.h"
#include "Record/CLegReplayer.h"

#include "Camera/C3rdCamera.h"

class CViewDemo :
	public CView,
	public IEventReceiver
{
protected:
	enum EState
	{
		StateController,
		StateRecording,
		StateEdit
	};

	CLegController* m_legController;
	CLegReplayer* m_legReplayer;

	EState m_state;

	CRecorder* m_recorder;

	C3rdCamera* m_3rdCamera;
	CGameObject* m_robot;
	CGameObject* m_robotReplay;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual bool OnEvent(const SEvent& event);

	void onGUI();
};