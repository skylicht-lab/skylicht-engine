#pragma once

#include "IApplicationEventReceiver.h"

class SampleShader : public IApplicationEventReceiver
{
private:
	CScene *m_scene;
	CCamera *m_guiCamera;
	CCamera *m_camera;
	CForwardRP *m_forwardRP;

	bool m_bakeSHLighting;

	std::vector<CGameObject*> m_objects;

public:
	SampleShader();
	virtual ~SampleShader();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();
};