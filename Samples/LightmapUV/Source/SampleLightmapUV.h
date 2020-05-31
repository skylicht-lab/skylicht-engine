#pragma once

#include "IApplicationEventReceiver.h"

class SampleLightmapUV : public IApplicationEventReceiver
{
private:
	CScene *m_scene;
	
	CForwardRP *m_forwardRP;
	CCamera *m_camera;
	CCamera *m_guiCamera;

	ITexture *m_UVChartsTexture;
public:
	SampleLightmapUV();
	virtual ~SampleLightmapUV();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();
};