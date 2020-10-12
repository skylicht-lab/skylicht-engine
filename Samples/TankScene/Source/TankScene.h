#pragma once

#include "IApplicationEventReceiver.h"

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

	virtual void onResize(int w, int h);

	virtual void onInitApp();

	virtual void onQuitApp();
};
