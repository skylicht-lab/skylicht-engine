#pragma once

#include "CBoldData.h"
#include "Entity/CArrayUtils.h"

class CNeighbor
{
protected:
	float m_minX;
	float m_maxX;
	float m_minZ;
	float m_maxZ;

	float m_cellSize;
	int m_poolSize;
	int m_bucketSizeX;
	int m_bucketSizeZ;
	CFastArray<CBoldData*>* m_buckets;

public:
	CNeighbor();

	virtual ~CNeighbor();

	void init(float minX, float maxX, float minZ, float maxZ, float cellsize = 4.0f, int poolSize = 30);

	void clear();

	void add(CBoldData** bolds, int count);

	void queryNeighbor(const core::vector3df& position, CFastArray<CBoldData*>& result);

protected:

	void addTo(CFastArray<CBoldData*>& bucket, CFastArray<CBoldData*>& result);
};