#pragma once

#include "IApplicationEventReceiver.h"

class @project_name@ : public IApplicationEventReceiver
{
private:
	CScene *m_scene;
	CCamera *m_guiCamera;
	CCamera *m_camera;

#if defined(USE_FREETYPE)	
	CGlyphFont *m_largeFont;
#endif

	CForwardRP *m_forwardRP;
public:
	@project_name@();
	virtual ~@project_name@();

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