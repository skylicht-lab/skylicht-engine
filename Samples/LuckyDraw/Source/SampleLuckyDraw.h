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
	std::vector<CScroller*> m_scrollers;

	CSpriteAtlas *m_sprite;

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

	virtual CGUIElement* createScrollElement(CScroller *scroller, CGUIElement *parent, const core::rectf& itemRect);

	virtual void updateScrollElement(CScroller *scroller, CGUIElement *item, int itemID);

};