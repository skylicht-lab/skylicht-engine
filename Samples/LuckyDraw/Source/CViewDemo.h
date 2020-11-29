#pragma once

#include "ViewManager/CView.h"

#include "AudioStream/CZipAudioStreamFactory.h"
#include "AudioStream/CZipAudioFileStream.h"

#include "CScroller.h"
#include "CScrollerController.h"
#include "CButton.h"

#define MAX_STATE 4

class CViewDemo :
	public CView,
	public IScrollerCallback
{
protected:
	CZipAudioStreamFactory *m_streamFactory;

	CGlyphFont *m_largeFont;
	CGlyphFont *m_smallFont;
	CGlyphFont *m_textMediumFont;
	CGlyphFont *m_textSmallFont;

	CGUISprite *m_iconSprite;
	CGUISprite *m_iconPeople;
	CGUIImage *m_backgroundImage;
	CGUIText *m_title;
	CGUIText *m_switchPrize;
	CGUIText *m_peopleText;

	CSpriteAtlas *m_sprite;

	std::vector<CScroller*> m_scrollers;
	CScrollerController *m_controller;

	CButton* m_spin;
	CButton* m_stop;
	CButton* m_accept;
	CButton* m_ignore;

	CButton* m_left;
	CButton* m_right;

	SFrame* m_icon[MAX_STATE];
	ITexture* m_stateTexture[MAX_STATE];
	std::string m_stateName[MAX_STATE];
	int m_peopleCount[MAX_STATE];

	SkylichtAudio::CAudioEmitter *m_musicBG;
	SkylichtAudio::CAudioEmitter *m_soundTada;
	SkylichtAudio::CAudioEmitter *m_soundSpin;
	SkylichtAudio::CAudioEmitter *m_soundStop;
	SkylichtAudio::CAudioEmitter *m_soundShowStop;
	SkylichtAudio::CAudioEmitter *m_soundCountDown;
	SkylichtAudio::CAudioEmitter *m_soundAccept;
	SkylichtAudio::CAudioEmitter *m_soundIgnore;
	SkylichtAudio::CAudioEmitter *m_soundUp;

	int m_randomNumber;
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

	void setState(int state);

	inline int getState()
	{
		return m_state;
	}

	void showLeftRightButton();

protected:

	void onSpinClick();

	void onStopClick();

	void onAcceptClick();

	void onIgnoreClick();

public:

	virtual CGUIElement* createScrollElement(CScroller *scroller, CGUIElement *parent, const core::rectf& itemRect);

	virtual void updateScrollElement(CScroller *scroller, CGUIElement *item, int itemID);
};