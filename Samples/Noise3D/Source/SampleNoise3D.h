#pragma once

#include "IApplicationEventReceiver.h"

class SampleNoise3D : public IApplicationEventReceiver
{
private:
	CScene* m_scene;
	CCamera* m_guiCamera;
	CCamera* m_camera;

	CGlyphFont* m_largeFont;

	CForwardRP* m_forwardRP;
	CPostProcessorRP* m_postProcessorRP;

	core::vector3df m_noiseOffset;
	std::vector<CMaterial*> m_materials;

public:
	SampleNoise3D();
	virtual ~SampleNoise3D();

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