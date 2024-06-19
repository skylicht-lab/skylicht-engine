#pragma once

#include "CLegIK.h"
#include "Transform/CWorldTransformData.h"

using namespace Skylicht;

class CLeg : public CLegIK
{
protected:
	core::vector3df m_lastFootPosition;
	core::vector3df m_footTargetPosition;

	core::vector3df m_targetVector;

	float m_stepHeight;
	float m_stepTime;
	float m_animTime;

	bool m_waiting;
	float m_waitingTime;

	std::vector<CLeg*> m_link;
public:
	CLeg(CWorldTransformData* root, std::vector<CWorldTransformData*>& joints, const core::vector3df& targetVector, const core::vector3df& footPos, float footStepLength, float stepHeight, float stepTime);

	virtual ~CLeg();

	virtual void update();

	inline void setTargetVector(const core::vector3df& v)
	{
		m_targetVector = v;
	}

	inline const core::vector3df& getTargetVector()
	{
		return m_targetVector;
	}

	inline void setFootTargetPosition(const core::vector3df& pos)
	{
		m_lastFootPosition = m_footTargetPosition;
		m_footTargetPosition = pos;
		m_animTime = 0.0f;
		m_waiting = false;
		m_waitingTime = 0.0f;
	}

	inline void forceFootPosition(const core::vector3df& pos)
	{
		m_lastFootPosition = pos;
		m_footTargetPosition = pos;
		m_animTime = m_stepTime;
		m_waiting = false;
		m_waitingTime = 0.0f;
	}

	inline void waiting(bool b)
	{
		m_waiting = b;
		m_waitingTime = !b ? 0.0f : m_waitingTime;
	}

	inline float getWaitingTime()
	{
		return m_waitingTime;
	}

	inline const core::vector3df& getFootTargetPosition()
	{
		return m_footTargetPosition;
	}

	inline float getAnimTime()
	{
		return m_animTime;
	}

	inline void setStepHeight(float height)
	{
		m_stepHeight = height;
	}

	void addLink(CLeg* leg);

	std::vector<CLeg*>& getLink()
	{
		return m_link;
	}
};
