#pragma once

#include "ViewManager/CView.h"

#include "AudioStream/CZipAudioStreamFactory.h"
#include "AudioStream/CZipAudioFileStream.h"

#include "CScroller.h"
#include "CScrollerController.h"
#include "CButton.h"

class CViewDemo :
	public CView,
	public IScrollerCallback
{
protected:
	CZipAudioStreamFactory *m_streamFactory;

	CGlyphFont *m_largeFont;
	CGlyphFont *m_smallFont;

	CGUIImage *m_backgroundImage;

	CSpriteAtlas *m_sprite;

	std::vector<CScroller*> m_scrollers;
	CScrollerController *m_controller;

	CButton* m_spin;
	CButton* m_stop;
	CButton* m_accept;
	CButton* m_ignore;

	SkylichtAudio::CAudioEmitter *m_musicBG;
	SkylichtAudio::CAudioEmitter *m_soundTada;
	SkylichtAudio::CAudioEmitter *m_soundSpin;
	SkylichtAudio::CAudioEmitter *m_soundStop;
	SkylichtAudio::CAudioEmitter *m_soundShowStop;
	SkylichtAudio::CAudioEmitter *m_soundCountDown;
	SkylichtAudio::CAudioEmitter *m_soundAccept;
	SkylichtAudio::CAudioEmitter *m_soundIgnore;

	int m_state;
	bool m_countDown;

public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

protected:

	void onSpinClick();

	void onStopClick();

	void onAcceptClick();

	void onIgnoreClick();

public:

	virtual CGUIElement* createScrollElement(CScroller *scroller, CGUIElement *parent, const core::rectf& itemRect);

	virtual void updateScrollElement(CScroller *scroller, CGUIElement *item, int itemID);
};