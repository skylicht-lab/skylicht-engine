#pragma once
#pragma once

#include "LegController/CLegController.h"

struct SRecordFrame
{
	float Time; // Second
	std::vector<core::vector3df> FootPosition;
	core::vector3df HipRelativePosition;
	core::vector3df ObjectPosition;
	core::quaternion ObjectRotation;
};

class CRecorder
{
protected:
	float m_time; // Second
	float m_lastTime;
	size_t m_hint;

	std::vector<SRecordFrame*> m_frames;

	float m_clip[2];

	SColor m_randomColor[10];
public:
	CRecorder();

	virtual ~CRecorder();

	float getDuration();

	SRecordFrame getFrameData(float frame);

	void addFrame(CLegController* legController);

	void start();

	void clear();

	inline float* getClip()
	{
		return m_clip;
	}

	inline void setClip(const float* clip)
	{
		m_clip[0] = core::clamp(clip[0], 0.0f, 1.0f);
		m_clip[1] = core::clamp(clip[1], 0.0f, 1.0f);
	}

	inline int getFrameCount()
	{
		return (int)m_frames.size();
	}

	void draw();
};