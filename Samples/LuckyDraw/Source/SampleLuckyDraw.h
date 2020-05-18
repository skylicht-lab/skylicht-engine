#pragma once

#include "IApplicationEventReceiver.h"

class SampleLuckyDraw :
	public IApplicationEventReceiver,
	public IScrollerCallback
{
private:
	CScene *m_scene;
	CCamera *m_guiCamera;

	CGlyphFont *m_largeFont;

	CGUIImage *m_backgroundImage;
	CScroller *m_numberScroll;

	int m_state;
public:
	SampleLuckyDraw();
	virtual ~SampleLuckyDraw();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual bool onBack();

	virtual void onInitApp();

	virtual void onQuitApp();

public:

	void initState(int cfg);

	virtual CGUIElement* createScrollElement(CGUIElement *parent, const core::rectf& itemRect);

	virtual void updateScrollElement(CGUIElement *item, int itemID);

};