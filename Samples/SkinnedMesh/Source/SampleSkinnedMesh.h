#pragma once

#include "IApplicationEventReceiver.h"

class SampleSkinnedMesh : public IApplicationEventReceiver
{
private:
	CScene *m_scene;
	CCamera *m_guiCamera;
	CCamera *m_camera;
	CForwardRP *m_forwardRP;

	bool m_bakeSHLighting;

	CGameObject *m_character01;
	CGameObject *m_character02;
public:
	SampleSkinnedMesh();
	virtual ~SampleSkinnedMesh();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();
};