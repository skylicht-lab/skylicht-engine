#include "pch.h"
#include "CNeighbor.h"

CNeighbor::CNeighbor() :
	m_cellSize(4.0f),
	m_poolSize(30),
	m_buckets(NULL),
	m_bucketSizeX(0),
	m_bucketSizeZ(0)
{

}

CNeighbor::~CNeighbor()
{
	if (m_buckets)
	{
		delete[]m_buckets;
	}
}

void CNeighbor::init(float minX, float maxX, float minZ, float maxZ, float cellsize, int poolSize)
{
	if (cellsize > 0)
	{
		float sizeX = maxX - minX;
		float sizeZ = maxZ - minZ;

		m_minX = minX;
		m_maxX = maxX;
		m_minZ = minZ;
		m_maxZ = maxZ;

		m_cellSize = cellsize;
		m_poolSize = poolSize;

		m_bucketSizeX = (int)ceill(sizeX / cellsize);
		m_bucketSizeZ = (int)ceill(sizeZ / cellsize);

		if (m_buckets)
			delete[]m_buckets;

		m_buckets = new CFastArray<CBoldData*>[m_bucketSizeX * m_bucketSizeZ];
		for (int x = 0; x < m_bucketSizeX; x++)
		{
			for (int z = 0; z < m_bucketSizeZ; z++)
			{
				int id = z * m_bucketSizeX + x;
				m_buckets[id].alloc(poolSize);
			}
		}
	}
}

void CNeighbor::clear()
{
	int n = m_bucketSizeX * m_bucketSizeZ;
	for (int i = 0; i < n; i++)
		m_buckets[i].reset();
}

void CNeighbor::add(CBoldData** bolds, int count)
{
	if (m_cellSize > 0)
	{
		for (int i = 0; i < count; i++)
		{
			CBoldData* bold = bolds[i];

			const core::vector3df& p = bold->Location;

			float x = p.X - m_minX;
			float z = p.Z - m_minZ;

			if (x >= 0 && z >= 0)
			{
				int cx = (int)floorf(x / m_cellSize);
				int cz = (int)floorf(z / m_cellSize);

				int id = cz * m_bucketSizeX + cx;

				if (m_buckets[id].count() < m_poolSize)
				{
					m_buckets[id].push(bold);
				}
			}
		}
	}
}

void CNeighbor::queryNeighbor(const core::vector3df& position, CFastArray<CBoldData*>& result)
{
	float px = position.X - m_minX;
	float pz = position.Z - m_minZ;

	if (px >= 0 && pz >= 0)
	{
		int cx = (int)floorf(px / m_cellSize);
		int cz = (int)floorf(pz / m_cellSize);

		int x, z, id;

		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				x = cx + i;
				z = cz + j;

				if (x >= 0 && x < m_bucketSizeX &&
					z >= 0 && z < m_bucketSizeZ)
				{
					id = z * m_bucketSizeX + x;
					addTo(m_buckets[id], result);
				}
			}
		}
	}
}

void CNeighbor::addTo(CFastArray<CBoldData*>& bucket, CFastArray<CBoldData*>& result)
{
	CBoldData** src = bucket.pointer();
	int i = bucket.count() - 1;
	while (i >= 0)
	{
		result.push(src[i]);
		--i;
	}
}