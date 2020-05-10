#pragma once

#include "IApplicationEventReceiver.h"

class SampleSponza : public IApplicationEventReceiver
{
private:

public:
	SampleSponza();
	virtual ~SampleSponza();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();
};
