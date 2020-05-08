#pragma once

#include "IApplicationEventReceiver.h"

#include "Scene/CScene.h"
#include "Camera/CCamera.h"
#include "Graphics2D/SpriteFrame/CGlyphFont.h"

class TankScene : public IApplicationEventReceiver
{
private:

public:
	TankScene();
	virtual ~TankScene();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();
};