#pragma once

#include "Transform/CWorldTransformData.h"

using namespace Skylicht;

class CLeg
{
protected:
	CWorldTransformData* m_root;

	bool m_drawDebug;

	std::vector<CWorldTransformData*> m_joints;
	std::vector<core::matrix4> m_worlds;
	std::vector<core::vector3df> m_upVector;
	std::vector<float> m_lengths;

	core::vector3df m_scale;

	core::vector3df m_footPosition;
	core::vector3df m_lastFootPosition;
	core::vector3df m_footTargetPosition;

	core::vector3df m_targetVector;
	float m_footStepLength;
	float m_stepHeight;
	float m_stepTime;

	float m_animTime;

	std::vector<CLeg*> m_link;
public:
	CLeg(CWorldTransformData* root, std::vector<CWorldTransformData*>& joints, const core::vector3df& targetVector, const core::vector3df& footPos, float footStepLength, float stepHeight, float stepTime);

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

	inline void setFootTargetPosition(const core::vector3df& pos)
	{
		m_lastFootPosition = m_footTargetPosition;
		m_footTargetPosition = pos;
		m_animTime = 0.0f;
	}

	inline core::vector3df& getFootPosition()
	{
		return m_footPosition;
	}

	inline const core::vector3df& getFootTargetPosition()
	{
		return m_footTargetPosition;
	}

	inline float getAnimTime()
	{
		return m_animTime;
	}

	void addLink(CLeg* leg);

	std::vector<CLeg*>& getLink()
	{
		return m_link;
	}

private:

	void initLinkLength();

	void fabricIK(const core::vector3df& target);

	void setTransform(core::matrix4& mat, const core::vector3df& pos, core::vector3df& front, core::vector3df up);
};
