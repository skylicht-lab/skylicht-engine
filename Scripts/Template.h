#pragma once

#include "IApplicationEventReceiver.h"

#include "Scene/CScene.h"
#include "Camera/CCamera.h"
#include "Graphics2D/SpriteFrame/CGlyphFont.h"

class @project_name@ : public IApplicationEventReceiver
{
private:
	CScene *m_scene;
	CCamera *m_guiCamera;
	CGlyphFont *m_largeFont;

public:
	@project_name@();
	virtual ~@project_name@();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();
};