#pragma once

#include "IApplicationEventReceiver.h"

class SampleBlendShape : public IApplicationEventReceiver
{
public:
	SampleBlendShape();

	virtual ~SampleBlendShape();

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