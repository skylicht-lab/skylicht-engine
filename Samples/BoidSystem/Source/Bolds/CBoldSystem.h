#pragma once

#include "CBoldData.h"
#include "CNeighbor.h"
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

	float m_minX;
	float m_maxX;
	float m_minZ;
	float m_maxZ;
	float m_margin;

	CNeighbor m_neighbor;

public:
	CBoldSystem();

	virtual ~CBoldSystem();

	virtual void beginQuery(CEntityManager* entityManager);

	virtual void onQuery(CEntityManager* entityManager, CEntity** entities, int count);

	virtual void init(CEntityManager* entityManager);

	virtual void update(CEntityManager* entityManager);

	inline void setBounds(float minX, float maxX, float minZ, float maxZ, float margin)
	{
		m_minX = minX;
		m_maxX = maxX;
		m_minZ = minZ;
		m_maxZ = maxZ;
		m_margin = margin;
		m_neighbor.init(minX, maxX, minZ, maxZ, 2.0f, 30);
	}

	inline void getBounds(float& minX, float& maxX, float& minZ, float& maxZ)
	{
		minX = m_minX;
		maxX = m_maxX;
		minZ = m_minZ;
		maxZ = m_maxZ;
	}

private:

	void neighbor(CBoldData** bolds, CWorldTransformData** transforms, int numEntity);

	void separation(CBoldData** bolds, CWorldTransformData** transforms, int numEntity);

	void alignment(CBoldData** bolds, CWorldTransformData** transforms, int numEntity);

	void cohesion(CBoldData** bolds, CWorldTransformData** transforms, int numEntity);

	void borders(CBoldData** bolds, CWorldTransformData** transforms, int numEntity);

	void updateTransform(CBoldData** bolds, CWorldTransformData** transforms, int numEntity);
};