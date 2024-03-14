#pragma once

#include "Components/ILateUpdate.h"
#include "Components/CComponentSystem.h"
#include "Debug/CSceneDebug.h"
#include "Animation/Skeleton/CSkeleton.h"
#include "Animation/Skeleton/CAnimationTransformData.h"
#include "Animation/IK/CIKSolver.h"

class CIKHand :
	public CComponentSystem,
	public ILateUpdate
{
protected:
	struct SHand
	{
		CAnimationTransformData* Nodes[3];

		SHand()
		{
			for (int i = 0; i < 3; i++)
				Nodes[i] = NULL;
		}
	};

	CSkeleton* m_skeleton;
	bool m_drawSekeleton;

	SHand m_leftHand;
	SHand m_rightHand;
	CAnimationTransformData* m_gun;

	core::vector3df m_aimTarget;

	CIKSolver m_ikSolver;

	float m_aimWeight;
	float m_runWeight;

	bool m_enable;

public:
	CIKHand();

	virtual ~CIKHand();

	virtual void initComponent();

	virtual void updateComponent();

	core::vector3df getWorldPosition(CAnimationTransformData* node);

	const core::matrix4& getWorldTransform(CAnimationTransformData* node);

	void notifyLateUpdate(CAnimationTransformData* node);

	virtual void lateUpdate();

	inline void setEnable(bool b)
	{
		m_enable = b;
	}

	void setGun(CAnimationTransformData* gun);

	void setLeftHand(CAnimationTransformData* start, CAnimationTransformData* mid, CAnimationTransformData* end);

	void setRightHand(CAnimationTransformData* start, CAnimationTransformData* mid, CAnimationTransformData* end);

	inline void setAimTarget(const core::vector3df& aimTarget, float aimWeight, float runWeight)
	{
		m_aimTarget = aimTarget;
		m_aimWeight = aimWeight;
		m_runWeight = runWeight;
	}

	inline void setSkeleton(CSkeleton* skeleton)
	{
		m_skeleton = skeleton;
	}

	inline void enableDrawDebugSkeleton(bool b)
	{
		m_drawSekeleton = b;
	}

private:

	core::vector3df getPoleVector(const core::vector3df* p);
};
