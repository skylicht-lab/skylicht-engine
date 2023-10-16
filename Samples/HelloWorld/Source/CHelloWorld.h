#pragma once

#include "IApplicationEventReceiver.h"

class CHelloWorld : public IApplicationEventReceiver
{
private:
	CScene* m_scene;
	CCamera* m_guiCamera;

	CGlyphFont* m_largeFont;

public:
	CHelloWorld();
	virtual ~CHelloWorld();

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
