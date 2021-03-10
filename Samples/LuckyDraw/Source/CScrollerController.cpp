#include "pch.h"
#include "SkylichtEngine.h"
#include "CScrollerController.h"

// [0 - 9]
#define NUMBER_RANGE 10.0f
#define MAX_SPEED 2.0f
#define SLOT_DELAY_ROLLING 200.0f
#define READY_TARGET_SPEED 1.4f

CScrollerController::CScrollerController(std::vector<CScroller*>& scrollers) :
	m_targetNumber(0),
	m_stopPosition(0)
{
	for (CScroller* s : scrollers)
	{
		m_scrollers.push_back(SScrollerInfo());
		SScrollerInfo& scroller = m_scrollers.back();

		scroller.Scroller = s;
		scroller.WaitScrollTime = 0.0f;
		scroller.Speed = 0.0f;
		scroller.TargetSpeed = 0.0f;
		scroller.TargetStop = 0.0f;
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
	for (int i = 0, n = (int)m_scrollers.size(); i < n; i++)
	{
		SScrollerInfo& s = m_scrollers[i];

		if (s.State == CScrollerController::Stop)
		{
			s.WaitScrollTime = 0.0f;
			s.Speed = 0.0f;
			s.TargetSpeed = 0.0f;
			s.TargetStop = 0.0f;
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

				float maxOffset = s.Scroller->getItemSize() * NUMBER_RANGE;
				f = fmodf(f, maxOffset);

				s.Scroller->setOffset(f);
			}
		}
		else if (s.State == CScrollerController::WaitStop)
		{
			if (i == m_stopPosition)
			{
				// stop at position
				float f = s.Scroller->getOffset();

				float distance = s.TargetStop - f;
				if (distance <= s.Scroller->getItemSize() * 0.3f)
					m_stopPosition++;

				float speedOff = distance / s.StopLengthDistance;
				f = f + getTimeStep() * (s.Speed * speedOff);
				s.Scroller->setOffset(f);

			}
			else if (i < m_stopPosition)
			{
				// finish stop
				float f = s.Scroller->getOffset();

				float distance = s.TargetStop - f;
				if (distance <= s.Scroller->getItemSize() * 0.2f)
					s.State = Finish;
				else
				{
					float speedOff = distance / s.StopLengthDistance;
					f = f + getTimeStep() * (s.Speed * speedOff);
					s.Scroller->setOffset(f);
				}
			}
			else
			{
				// continue rolling
				float f = s.Scroller->getOffset();
				f = f + getTimeStep() * s.Speed;

				float maxOffset = s.Scroller->getItemSize() * NUMBER_RANGE;
				f = fmodf(f, maxOffset);

				s.Scroller->setOffset(f);
			}
		}
		else if (
			s.State == CScrollerController::Finish ||
			s.State == CScrollerController::NewRound
			)
		{
			// keep smooth stop at target
			float f = s.Scroller->getOffset();
			float distance = s.TargetStop - f;
			float speedOff = distance / s.StopLengthDistance;
			f = f + getTimeStep() * (s.Speed * speedOff);
			s.Scroller->setOffset(f);
		}

		s.Scroller->update();
	}
}

void CScrollerController::beginScroll()
{
	m_stopPosition = 0;

	for (u32 i = 0, n = (u32)m_scrollers.size(); i < n; i++)
	{
		SScrollerInfo& s = m_scrollers[i];
		s.TargetSpeed = MAX_SPEED;
		s.Speed = 0.0f;
		s.WaitScrollTime = i * SLOT_DELAY_ROLLING;
		s.State = Scrolling;
	}
}

bool CScrollerController::stopReady()
{
	for (u32 i = 0, n = (u32)m_scrollers.size(); i < n; i++)
	{
		if (m_scrollers[i].Speed <= READY_TARGET_SPEED ||
			m_scrollers[i].State != Scrolling)
			return false;
	}

	return true;
}

void CScrollerController::newRound()
{
	for (u32 i = 0, n = (u32)m_scrollers.size(); i < n; i++)
	{
		m_scrollers[i].State = NewRound;
	}
}

bool CScrollerController::isFinished()
{
	for (u32 i = 0, n = (u32)m_scrollers.size(); i < n; i++)
	{
		if (m_scrollers[i].State != Finish)
			return false;
	}

	return true;
}

bool CScrollerController::isLastStopPosition()
{
	int scroll = (int)m_scrollers.size() - 1;
	if (scroll < 0)
		scroll = 0;

	if (m_stopPosition >= scroll)
		return true;

	return false;
}

void CScrollerController::stopOnNumber(int number)
{
	m_targetNumber = number;

	int numScroller = (int)m_scrollers.size();

	int t = 1;
	for (int i = 0; i < numScroller - 1; i++)
		t = t * 10;

	for (int i = 0; i < numScroller; i++)
	{
		SScrollerInfo& s = m_scrollers[i];
		int num = 0;

		if (number >= t)
			num = number / t;

		float stop = s.Scroller->getItemSize() * num + s.Scroller->getItemSize() * NUMBER_RANGE;
		s.State = WaitStop;
		s.TargetStop = stop;
		s.StopLengthDistance = stop - s.Scroller->getOffset();

		if (s.StopLengthDistance < s.Scroller->getItemSize())
		{
			stop = stop + s.Scroller->getItemSize() * NUMBER_RANGE;
			s.TargetStop = stop;
			s.StopLengthDistance = stop - s.Scroller->getOffset();
		}

		if (number > t)
			number = number % (num * t);

		t = t / 10;
	}

	m_stopPosition = 0;
}