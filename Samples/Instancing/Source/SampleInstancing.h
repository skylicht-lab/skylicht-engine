#pragma once

#include "IApplicationEventReceiver.h"

class SampleInstancing : public IApplicationEventReceiver
{
private:
	CScene* m_scene;
	CCamera* m_guiCamera;
	CCamera* m_camera;

	CForwardRP* m_forwardRP;
public:
	SampleInstancing();
	virtual ~SampleInstancing();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onResize(int w, int h);

	virtual void onInitApp();

	virtual void onQuitApp();
};