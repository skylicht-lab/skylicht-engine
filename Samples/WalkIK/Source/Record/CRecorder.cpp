#include "pch.h"
#include "CRecorder.h"

#include "Debug/CSceneDebug.h"

CRecorder::CRecorder() :
	m_time(0.0f),
	m_lastTime(0.0f)
{

}

CRecorder::~CRecorder()
{
	clear();
}

void CRecorder::addFrame(CLegController* legController)
{
	float fps = 30.0f;
	m_time = m_time + getTimeStep() * 0.001f;

	if (m_time > m_lastTime + 1.0f / fps)
	{
		SRecordFrame* frame = new SRecordFrame();

		std::vector<CLeg*>& legs = legController->getLegs();
		for (CLeg* leg : legs)
			frame->FootPosition.push_back(leg->getFootPosition());

		frame->ObjectPosition = legController->getGameObject()->getPosition();
		frame->HipPosition = legController->getRoot()->getWorldPosition();

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
	CSceneDebug* debug = CSceneDebug::getInstance();
	for (size_t i = 1, n = m_frames.size(); i < n; i++)
	{
		SRecordFrame* f0 = m_frames[i - 1];
		SRecordFrame* f1 = m_frames[i];

		size_t nLeg = f0->FootPosition.size();
		for (int j = 0; j < nLeg; j++)
			debug->addLine(f0->FootPosition[j], f1->FootPosition[j], SColor(255, 255, 0, 0));

		debug->addLine(f0->HipPosition, f1->HipPosition, SColor(255, 0, 0, 255));
	}
}