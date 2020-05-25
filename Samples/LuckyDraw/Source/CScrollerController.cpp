#include "pch.h"
#include "SkylichtEngine.h"
#include "CScrollerController.h"

CScrollerController::CScrollerController(std::vector<CScroller*>& scroller)
{
	m_scrollers = scroller;
}

CScrollerController::~CScrollerController()
{

}

void CScrollerController::update()
{
	// gui scroller update
	for (u32 i = 0, n = m_scrollers.size(); i < n; i++)
	{
		//float speed = 0.5f;
		//float f = m_scrollers[i]->getOffset();
		//f = f + getTimeStep() * speed;

		//m_scrollers[i]->setOffset(f);
		m_scrollers[i]->update();
	}
}