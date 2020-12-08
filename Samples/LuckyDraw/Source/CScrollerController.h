#pragma once

#include "CScroller.h"

class CScrollerController
{
public:
	enum EScrollerState
	{
		Stop,
		NewRound,
		Scrolling,
		WaitStop,
		Finish
	};

	struct SScrollerInfo
	{
		CScroller* Scroller;
		float Speed;
		float TargetSpeed;
		float WaitScrollTime;
		float TargetStop;
		float StopLengthDistance;
		EScrollerState State;
	};

protected:
	std::vector<SScrollerInfo> m_scrollers;

	int m_targetNumber;
	int m_stopPosition;

public:
	CScrollerController(std::vector<CScroller*>& scroller);

	virtual ~CScrollerController();

	void update();

	void beginScroll();

	bool stopReady();

	void newRound();

	bool isFinished();

	bool isLastStopPosition();

	void stopOnNumber(int number);

	void setVisible(bool b)
	{
		for (SScrollerInfo& s : m_scrollers)
		{
			s.Scroller->setVisible(b);
		}
	}
};