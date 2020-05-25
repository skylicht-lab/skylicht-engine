#pragma once

#include "CScroller.h"

class CScrollerController
{
protected:
	std::vector<CScroller*> m_scrollers;

public:
	CScrollerController(std::vector<CScroller*>& scroller);

	virtual ~CScrollerController();

	void update();
};