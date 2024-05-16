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
		CBoldData* bold;
		float x, z;
		int cx, cz, id;

		for (int i = 0; i < count; i++)
		{
			bold = bolds[i];

			x = bold->Location.X - m_minX;
			z = bold->Location.Z - m_minZ;

			cx = (int)floorf(x / m_cellSize);
			cz = (int)floorf(z / m_cellSize);

			if (cx >= 0 && cx < m_bucketSizeX &&
				cz >= 0 && cz < m_bucketSizeZ)
			{
				id = cz * m_bucketSizeX + cx;

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

		CFastArray<CBoldData*>* bucket;
		CBoldData** src;
		int x, z, id, n, i, j;

		for (i = -1; i <= 1; i++)
		{
			for (j = -1; j <= 1; j++)
			{
				x = cx + i;
				z = cz + j;

				if (x >= 0 && x < m_bucketSizeX &&
					z >= 0 && z < m_bucketSizeZ)
				{
					id = z * m_bucketSizeX + x;
					bucket = &m_buckets[id];

					// add entity from cell bucket: id to result
					src = bucket->pointer();
					n = bucket->count() - 1;
					while (n >= 0)
					{
						result.push(src[n]);
						--n;
					}
				}
			}
		}
	}
}