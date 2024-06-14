#pragma once
#pragma once

#include "LegController/CLegController.h"

struct SRecordFrame
{
	float Time; // Second
	std::vector<core::vector3df> FootPosition;
	core::vector3df HipPosition;
	core::vector3df ObjectPosition;
};

class CRecorder
{
protected:
	float m_time; // Second
	float m_lastTime;

	std::vector<SRecordFrame*> m_frames;

public:
	CRecorder();

	virtual ~CRecorder();

	void addFrame(CLegController* legController);

	void start();

	void clear();

	inline int getFrameCount()
	{
		return (int)m_frames.size();
	}

	void draw();
};