#include "pch.h"
#include "SkylichtEngine.h"
#include "CScrollerController.h"

CScrollerController::CScrollerController(std::vector<CScroller*>& scrollers)
{
	for (CScroller *s : scrollers)
	{
		m_scrollers.push_back(SScrollerInfo());
		SScrollerInfo &scroller = m_scrollers.back();

		scroller.Scroller = s;
		scroller.WaitScrollTime = 0.0f;
		scroller.Speed = 0.0f;
		scroller.TargetSpeed = 0.0f;
		scroller.State = Stop;
	}
}

CScrollerController::~CScrollerController()
{

}

void CScrollerController::update()
{
	float dt = getTimeStep();

	// gui scroller update
	for (u32 i = 0, n = m_scrollers.size(); i < n; i++)
	{
		SScrollerInfo &s = m_scrollers[i];

		if (s.State == CScrollerController::Stop)
		{
			s.WaitScrollTime = 0.0f;
			s.Speed = 0.0f;
			s.TargetSpeed = 0.0f;
			s.Scroller->setOffset(0.0f);
		}
		else if (s.State == CScrollerController::Scrolling)
		{
			if (s.WaitScrollTime >= 0)
			{
				s.WaitScrollTime = s.WaitScrollTime - dt;
			}
			else
			{
				s.Speed = s.Speed + (s.TargetSpeed - s.Speed) * dt * 0.001f;

				float f = s.Scroller->getOffset();
				f = f + getTimeStep() * s.Speed;

				float maxOffset = s.Scroller->getItemSize() * 10.0f;
				f = fmodf(f, maxOffset);

				s.Scroller->setOffset(f);
			}
		}
		else if (s.State == CScrollerController::WaitStop)
		{

		}
		else if (s.State == CScrollerController::Finish)
		{

		}

		s.Scroller->update();
	}
}

void CScrollerController::beginScroll()
{
	for (u32 i = 0, n = m_scrollers.size(); i < n; i++)
	{
		SScrollerInfo& s = m_scrollers[i];
		s.TargetSpeed = 2.0f;
		s.WaitScrollTime = i * 500.0f;
		s.State = Scrolling;
	}
}

bool CScrollerController::stopReady()
{
	for (u32 i = 0, n = m_scrollers.size(); i < n; i++)
	{
		if (m_scrollers[i].Speed >= 1.5f ||
			m_scrollers[i].State != Scrolling)
			return false;
	}

	return true;
}