#pragma once

#include "IApplicationEventReceiver.h"

class SampleBoidSystem : public IApplicationEventReceiver
{
public:
	SampleBoidSystem();
	virtual ~SampleBoidSystem();

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