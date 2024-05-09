#pragma once

#include "CBoldData.h"
#include "Animation/CAnimationClip.h"
#include "Entity/IEntitySystem.h"
#include "Entity/CEntityGroup.h"
#include "Entity/CEntityManager.h"
#include "SkinnedInstancing/CSkinnedInstanceData.h"
#include "Transform/CWorldTransformData.h"

struct SMovingAnimation
{
	int ClipId;
	CAnimationClip* Clip;
	int FPS;
	float Speed;
};


class CBoldAnimationSystem : public Skylicht::IEntitySystem
{
protected:
	Skylicht::CEntityGroup* m_group;

	core::array<SMovingAnimation> m_clips;

	CFastArray<CBoldData*> m_bolds;
	CFastArray<CSkinnedInstanceData*> m_skinnedInstances;

public:
	CBoldAnimationSystem();

	virtual ~CBoldAnimationSystem();

	virtual void beginQuery(CEntityManager* entityManager);

	virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int count);

	virtual void init(CEntityManager* entityManager);

	virtual void update(CEntityManager* entityManager);

	void addClip(CAnimationClip* clip, int clipId, int fps, float runSpeed);

private:

};