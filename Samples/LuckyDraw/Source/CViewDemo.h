#pragma once

#include "ViewManager/CView.h"

#include "AudioStream/CZipAudioStreamFactory.h"
#include "AudioStream/CZipAudioFileStream.h"

#include "UserInterface/CUIButton.h"
#include "UserInterface/CUIListView.h"

#include "CScroller.h"
#include "CScrollerController.h"
#include "CLocalize.h"

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
	UI::CUIContainer* m_uiContainer;

	CGUISprite* m_iconSprite;
	CGUISprite* m_iconPeople;
	CGUIImage* m_backgroundImage;
	CGUIText* m_title;
	CGUIText* m_switchPrize;
	CGUIText* m_peopleText;

	CSpriteAtlas* m_sprite;

	std::vector<CScroller*> m_scrollers;
	CScrollerController* m_controller;

	UI::CUIListView* m_list;

	UI::CUIButton* m_spin;
	UI::CUIButton* m_stop;
	UI::CUIButton* m_accept;
	UI::CUIButton* m_ignore;

	UI::CUIButton* m_left;
	UI::CUIButton* m_right;

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

	void onSpinClick(UI::CUIBase* base);

	void onStopClick(UI::CUIBase* base);

	void onAcceptClick(UI::CUIBase* base);

	void onIgnoreClick(UI::CUIBase* base);

public:

	virtual CGUIElement* createScrollElement(CScroller* scroller, CGUIElement* parent, const core::rectf& itemRect);

	virtual void updateScrollElement(CScroller* scroller, CGUIElement* item, int itemID);
};
