#pragma once

#include "ViewManager/CView.h"

#include "AudioStream/CZipAudioStreamFactory.h"
#include "AudioStream/CZipAudioFileStream.h"

#include "CScroller.h"
#include "CScrollerController.h"
#include "CLocalize.h"
#include "CButton.h"
#include "CList.h"

#define MAX_STATE 4

class CViewDemo :
	public CView,
	public IScrollerCallback
{
public:
	struct SPrize
	{
		SFrame* Icon;
		ITexture* Background;
		std::string Name;
		int TotalPeopleCount;
		int PeopleCount;

		SPrize()
		{
			Icon = NULL;
			Background = NULL;
			TotalPeopleCount = 0;
			PeopleCount = 0;
		}
	};

	struct SPeople
	{
		int ID;
		int WinPrize;
		bool IsLucky;

		SPeople()
		{
			ID = -1;
			WinPrize = -1;
			IsLucky = false;
		}
	};

protected:
	CZipAudioStreamFactory* m_streamFactory;

	CGlyphFont* m_largeFont;
	CGlyphFont* m_textMediumFont;
	CGlyphFont* m_textMedium2Font;
	CGlyphFont* m_textSmallFont;

	CCanvas* m_canvas;
	CGUISprite* m_iconSprite;
	CGUISprite* m_iconPeople;
	CGUIImage* m_backgroundImage;
	CGUIText* m_title;
	CGUIText* m_switchPrize;
	CGUIText* m_peopleText;

	CSpriteAtlas* m_sprite;

	std::vector<CScroller*> m_scrollers;
	CScrollerController* m_controller;

	CList* m_list;

	CButton* m_spin;
	CButton* m_stop;
	CButton* m_accept;
	CButton* m_ignore;

	CButton* m_left;
	CButton* m_right;

	Audio::CAudioEmitter* m_musicBG;
	Audio::CAudioEmitter* m_soundTada;
	Audio::CAudioEmitter* m_soundSpin;
	Audio::CAudioEmitter* m_soundStop;
	Audio::CAudioEmitter* m_soundShowStop;
	Audio::CAudioEmitter* m_soundCountDown;
	Audio::CAudioEmitter* m_soundAccept;
	Audio::CAudioEmitter* m_soundIgnore;
	Audio::CAudioEmitter* m_soundUp;

	int m_randomPeople;
	int m_randomNumber;
	int m_state;
	bool m_countDown;

	SPrize m_prize[MAX_STATE];
	std::vector<SPeople> m_people;
	core::array<int> m_listPeople;
public:
	CViewDemo();

	virtual ~CViewDemo();

	virtual void onInit();

	virtual void onDestroy();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	int getNumState();

	int getNumScroller();

	void setState(int state);

	void checkToShowListLuckyPeople();

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

	virtual CGUIElement* createScrollElement(CScroller* scroller, CGUIElement* parent, const core::rectf& itemRect);

	virtual void updateScrollElement(CScroller* scroller, CGUIElement* item, int itemID);
};
