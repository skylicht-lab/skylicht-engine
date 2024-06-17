#pragma once

#include "Transform/CWorldTransformData.h"

using namespace Skylicht;

class CLegIK
{
protected:
	CWorldTransformData* m_root;

	std::vector<CWorldTransformData*> m_joints;
	std::vector<core::matrix4> m_worlds;
	std::vector<core::vector3df> m_upVector;
	std::vector<float> m_lengths;

	bool m_drawDebug;

	core::vector3df m_scale;
	core::vector3df m_footPosition;

public:
	CLegIK(CWorldTransformData* root, std::vector<CWorldTransformData*>& joints);

	virtual ~CLegIK();

	virtual void update();

	virtual void lateUpdate();


	inline void setFootPosition(const core::vector3df& pos)
	{
		m_footPosition = pos;
	}

	inline core::vector3df& getFootPosition()
	{
		return m_footPosition;
	}

private:

	void initLinkLength();

	void fabricIK(const core::vector3df& target);

	void setTransform(core::matrix4& mat, const core::vector3df& pos, core::vector3df& front, core::vector3df up);
};
