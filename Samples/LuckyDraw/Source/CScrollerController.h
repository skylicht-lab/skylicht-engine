#pragma once

#include "CScroller.h"

class CScrollerController
{
public:
	enum EScrollerState
	{
		Stop,
		Scrolling,
		WaitStop,
		Finish
	};

	struct SScrollerInfo
	{
		CScroller *Scroller;
		float Speed;
		float TargetSpeed;
		float WaitScrollTime;
		EScrollerState State;
	};

protected:
	std::vector<SScrollerInfo> m_scrollers;

public:
	CScrollerController(std::vector<CScroller*>& scroller);

	virtual ~CScrollerController();

	void update();

	void beginScroll();

	bool stopReady();
};