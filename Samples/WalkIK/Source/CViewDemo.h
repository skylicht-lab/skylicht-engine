#pragma once

#include "ViewManager/CView.h"

#include "LegController/CLegController.h"
#include "Record/CRecorder.h"

class CViewDemo :
	public CView,
	public IEventReceiver
{
protected:
	CLegController* m_legController;

	bool m_isRecording;
	CRecorder* m_recorder;

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