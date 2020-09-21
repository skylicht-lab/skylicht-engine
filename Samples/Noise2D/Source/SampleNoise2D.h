#pragma once

#include "IApplicationEventReceiver.h"

class SampleNoise2D : public IApplicationEventReceiver
{
private:
	CScene *m_scene;
	CCamera *m_guiCamera;
	CCamera *m_camera;

	CMaterial *m_noiseMaterial;
	CMaterial *m_electricMaterial;
	CMaterial *m_electricLightningMaterial;

#if defined(USE_FREETYPE)	
	CGlyphFont *m_largeFont;
#endif

	CForwardRP *m_forwardRP;

	core::vector3df m_noiseOffset;

public:
	SampleNoise2D();
	virtual ~SampleNoise2D();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();
};