#include "pch.h"
#include "CRecorder.h"
#include "Utils/CVector.h"
#include "Debug/CSceneDebug.h"

CRecorder::CRecorder() :
	m_time(0.0f),
	m_lastTime(0.0f),
	m_hint(0)
{
	m_clip[0] = 0.0f;
	m_clip[1] = 1.0f;

	for (int i = 0; i < 10; i++)
	{
		u32 r = (u32)(os::Randomizer::frand() * 200.0f + 20.0f);
		u32 g = (u32)(os::Randomizer::frand() * 200.0f + 20.0f);
		u32 b = (u32)(os::Randomizer::frand() * 200.0f + 20.0f);
		m_randomColor[i].set(255, r, g, b);
	}
}

CRecorder::~CRecorder()
{
	clear();
}

float CRecorder::getDuration()
{
	if (m_frames.size() == 0)
		return 0.0f;
	return m_frames.back()->Time;
}

SRecordFrame CRecorder::getFrameData(float frame)
{
	if (m_hint >= m_frames.size())
		m_hint = 0;

	if (m_frames.size() == 0)
		return SRecordFrame();

	if (m_frames[m_hint]->Time > frame)
		m_hint = 0;

	for (size_t i = m_hint, n = m_frames.size() - 1; i < n; i++)
	{
		SRecordFrame* f0 = m_frames[i];
		SRecordFrame* f1 = m_frames[i + 1];

		if (i == 0 && frame < f0->Time)
		{
			return *f0;
		}
		else if (f0->Time <= frame && frame <= f1->Time)
		{
			SRecordFrame ret;
			float frameTime = f1->Time - f0->Time;
			if (frameTime > 0)
			{
				float f = (frame - f0->Time) / frameTime;
				ret.Time = frame;
				ret.HipRelativePosition = CVector::lerp(f0->HipRelativePosition, f1->HipRelativePosition, f);
				ret.ObjectPosition = CVector::lerp(f0->ObjectPosition, f1->ObjectPosition, f);
				ret.ObjectRotation.slerp(f0->ObjectRotation, f1->ObjectRotation, f);

				size_t nLeg = f0->FootPosition.size();
				for (int j = 0; j < nLeg; j++)
					ret.FootPosition.push_back(CVector::lerp(f0->FootPosition[j], f1->FootPosition[j], f));
			}
			else
			{
				ret = *f0;
			}
			m_hint = i;
			return ret;
		}
	}

	return *(m_frames.back());
}

void CRecorder::addFrame(CLegController* legController)
{
	float fps = 30.0f;
	m_time = m_time + getTimeStep() * 0.001f;

	if (m_time > m_lastTime + 1.0f / fps)
	{
		SRecordFrame* frame = new SRecordFrame();

		// time
		frame->Time = m_time;

		// foot position
		std::vector<CLeg*>& legs = legController->getLegs();
		for (CLeg* leg : legs)
			frame->FootPosition.push_back(leg->getFootPosition());

		// hip & object position
		frame->ObjectPosition = legController->getGameObject()->getPosition();
		frame->ObjectRotation = legController->getGameObject()->getRotation();
		frame->HipRelativePosition = legController->getRoot()->getRelativePosition();

		m_frames.push_back(frame);
		m_lastTime = m_time;
	}
}

void CRecorder::start()
{
	clear();
	m_time = 0.0f;
	m_lastTime = 0.0f;
}

void CRecorder::clear()
{
	for (SRecordFrame* frame : m_frames)
		delete frame;
	m_frames.clear();
}

void CRecorder::draw()
{
	float duration = getDuration();

	float clipFrom = duration * m_clip[0];
	float clipTo = duration * m_clip[1];
	if (clipFrom >= clipTo)
		clipFrom = clipTo;

	float time = clipFrom;

	SColor blue(255, 0, 0, 255);

	CSceneDebug* debug = CSceneDebug::getInstance();

	float fps = 30.0f;
	float deltaTime = 1.0f / fps;

	SRecordFrame f0, f1;

	while (time < clipTo)
	{
		f0 = getFrameData(time);

		time = time + deltaTime;
		if (time > clipTo)
			time = clipTo;

		f1 = getFrameData(time);

		size_t nLeg = f0.FootPosition.size();
		for (int j = 0; j < nLeg; j++)
			debug->addLine(f0.FootPosition[j], f1.FootPosition[j], m_randomColor[j % 10]);

		debug->addLine(f0.ObjectPosition + f0.HipRelativePosition, f1.ObjectPosition + f1.HipRelativePosition, blue);
	}
}