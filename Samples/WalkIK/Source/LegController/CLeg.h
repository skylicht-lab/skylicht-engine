#pragma once

#include "Transform/CWorldTransformData.h"

using namespace Skylicht;

class CLeg
{
protected:
	std::vector<CWorldTransformData*> m_joints;

	core::vector3df m_footTargetPosition;
	core::vector3df m_targetVector;
	float m_footStepLength;

public:
	CLeg(std::vector<CWorldTransformData*>& joints, const core::vector3df& targetVector, const core::vector3df& footPos, float footStepLength);

	virtual ~CLeg();

	void update();

	virtual void lateUpdate();

	inline void setTargetVector(const core::vector3df& v)
	{
		m_targetVector = v;
	}

	inline const core::vector3df& getTargetVector()
	{
		return m_targetVector;
	}

	core::vector3df getPosition();
	core::vector3df getFootPosition();

	inline void setFootTargetPosition(const core::vector3df& pos)
	{
		m_footTargetPosition = pos;
	}

	inline const core::vector3df& getFootTargetPosition()
	{
		return m_footTargetPosition;
	}
};
