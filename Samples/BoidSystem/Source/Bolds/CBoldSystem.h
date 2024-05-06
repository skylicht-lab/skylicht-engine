#pragma once

#include "CBoldData.h"
#include "Entity/IEntitySystem.h"
#include "Entity/CEntityGroup.h"
#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformData.h"

class CBoldSystem : public Skylicht::IEntitySystem
{
protected:
	Skylicht::CEntityGroup* m_group;

	CFastArray<CBoldData*> m_bolds;
	CFastArray<CWorldTransformData*> m_transforms;

public:
	CBoldSystem();

	virtual ~CBoldSystem();

	virtual void beginQuery(CEntityManager* entityManager);

	virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int count);

	virtual void init(CEntityManager* entityManager);

	virtual void update(CEntityManager* entityManager);

private:

	void borders(CBoldData** bolds, CWorldTransformData** transforms, int numEntity);

	void separation(CBoldData** bolds, CWorldTransformData** transforms, int numEntity);

	void updateTransform(CBoldData** bolds, CWorldTransformData** transforms, int numEntity);
};