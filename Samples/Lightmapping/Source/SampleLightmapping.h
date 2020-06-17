#pragma once

#include "IApplicationEventReceiver.h"

class SampleLightmapping : public IApplicationEventReceiver
{
public:
	SampleLightmapping();
	virtual ~SampleLightmapping();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();
};