/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CWalkingTileMap.h"

namespace Skylicht
{
	namespace Graph
	{
		CWalkingTileMap::CWalkingTileMap() :
			m_tileWidth(0.0f),
			m_tileHeight(0.0f),
			m_navMesh(NULL),
			m_obstacle(NULL),
			m_generateId(0),
			m_generateMax(0),
			m_generatePercent(0.0f),
			m_generateStep(None)
		{

		}

		CWalkingTileMap::~CWalkingTileMap()
		{
			release();
		}

		void CWalkingTileMap::generate(float tileWidth, float tileHeight, const core::aabbox3df& bbox)
		{
			release();

			m_tileWidth = tileWidth;
			m_tileHeight = tileHeight;

			core::vector3df size = bbox.getExtent();

			int nY = (int)ceilf(size.Y / tileHeight);
			int nX = (int)ceilf(size.X / tileWidth);
			int nZ = (int)ceilf(size.Z / tileWidth);

			core::vector3df boxSize(tileWidth, tileHeight, tileWidth);

			m_bbox.MinEdge = bbox.MinEdge;
			m_bbox.MaxEdge = bbox.MinEdge + core::vector3df(nX * tileWidth, nY * tileHeight, nZ * tileWidth);

			m_bbox.MinEdge.Y = m_bbox.MinEdge.Y - 0.1f;
			m_bbox.MaxEdge.Y = m_bbox.MaxEdge.Y + 0.1f;

			for (int y = 0; y <= nY; y++)
			{
				for (int z = 0; z <= nZ; z++)
				{
					for (int x = 0; x <= nX; x++)
					{
						STile* tile = new STile();
						tile->X = x;
						tile->Y = y;
						tile->Z = z;
						tile->BBox.MinEdge = bbox.MinEdge + core::vector3df(x * tileWidth, y * tileHeight, z * tileWidth);
						tile->BBox.MaxEdge = tile->BBox.MinEdge + boxSize;
						m_tiles.push_back(tile);
					}
				}
			}
		}

		void CWalkingTileMap::generate(float tileWidth, float tileHeight, CMesh* navMesh, CObstacleAvoidance* obstacle)
		{
			generate(tileWidth, tileHeight, navMesh->getBoundingBox());

			// check used tile
			IMeshBuffer* mb = navMesh->getMeshBuffer(0);
			IVertexBuffer* vb = mb->getVertexBuffer();
			IIndexBuffer* ib = mb->getIndexBuffer();

			for (u32 i = 0, n = ib->getIndexCount(); i < n; i += 3)
			{
				u32 ixA = ib->getIndex(i);
				u32 ixB = ib->getIndex(i + 1);
				u32 ixC = ib->getIndex(i + 2);

				S3DVertex* a = (S3DVertex*)vb->getVertex(ixA);
				S3DVertex* b = (S3DVertex*)vb->getVertex(ixB);
				S3DVertex* c = (S3DVertex*)vb->getVertex(ixC);

				core::aabbox3df box;
				box.reset(a->Pos);
				box.addInternalPoint(b->Pos);
				box.addInternalPoint(c->Pos);

				core::triangle3df tri(a->Pos, b->Pos, c->Pos);

				for (u32 j = 0, m = m_tiles.size(); j < m; j++)
				{
					if (m_tiles[j]->BBox.intersectsWithBox(box))
					{
						m_tiles[j]->Tris.push_back(tri);
					}
				}
			}

			core::line3df centerLine;

			for (int i = (int)m_tiles.size() - 1; i >= 0; i--)
			{
				if (m_tiles[i]->Tris.size() == 0)
				{
					delete m_tiles[i];
					m_tiles.erase(i);
				}
				else
				{
					const core::aabbox3df& bbox = m_tiles[i]->BBox;
					{
						// try get position is center
						core::vector3df center = bbox.getCenter();
						core::vector3df centerTop = center;
						core::vector3df centerBottom = center;
						centerTop.Y = bbox.MaxEdge.Y;
						centerBottom.Y = bbox.MinEdge.Y;
						centerLine.setLine(centerTop, centerBottom);

						bool hit = false;

						if (hitTris(centerLine, m_tiles[i]->Tris, m_tiles[i]->Position))
						{
							hit = true;
						}

						if (!hit)
						{
							delete m_tiles[i];
							m_tiles.erase(i);
						}
					}
				}
			}

			for (u32 i = 0, n = m_tiles.size(); i < n; i++)
			{
				STile* t = m_tiles[i];

				STileXYZ tile(t->X, t->Y, t->Z);
				m_hashTiles[tile] = t;
			}

			// link Neighbour
			float t = 0.0f;
			for (u32 i = 0, n = m_tiles.size() - 1; i < n; i++)
			{
				for (u32 j = i + 1, m = m_tiles.size(); j < m; j++)
				{
					STile* a = m_tiles[i];
					STile* b = m_tiles[j];

					if (abs(a->Y - b->Y) > 1)
						continue;

					// if (a->Position.getDistanceFromSQ(b->Position) > 400.0f)
					//	continue;

					if (!obstacle->isLineHit(a->Position, b->Position, tileHeight, t))
					{
						a->Neighbours.push_back(b);
						b->Neighbours.push_back(a);
					}
				}
			}

			// remove tile have no Neighbours
			for (int i = (int)m_tiles.size() - 1; i >= 0; i--)
			{
				if (m_tiles[i]->Neighbours.size() == 0)
				{
					// delete hash
					STileXYZ tile(m_tiles[i]->X, m_tiles[i]->Y, m_tiles[i]->Z);
					auto it = m_hashTiles.find(tile);
					if (it == m_hashTiles.end())
						m_hashTiles.erase(it);

					// free data
					delete m_tiles[i];
					m_tiles.erase(i);
				}
			}

			// sort id
			for (u32 i = 0, n = m_tiles.size(); i < n; i++)
			{
				STile* t = m_tiles[i];
				t->Id = i;
			}
		}

		void CWalkingTileMap::beginGenerate(float tileWidth, float tileHeight, CMesh* navMesh, CObstacleAvoidance* obstacle)
		{
			generate(tileWidth, tileHeight, navMesh->getBoundingBox());

			m_navMesh = navMesh;
			m_obstacle = obstacle;
			m_generateStep = CollectTile;
			m_generateId = 0;
			m_generatePercent = 0.0f;
		}

		bool CWalkingTileMap::updateGenerate()
		{
			if (m_generateStep == CollectTile)
				updateGenerateTile();
			else if (m_generateStep == RemoveEmptyTile)
				updateGenerateRemoveEmptyTile();
			else if (m_generateStep == LinkNeighbours)
				updateGenerateNeighbours();

			bool finish = false;

			if (m_generateStep == Finish)
			{
				finish = true;
				m_navMesh = NULL;
				m_obstacle = NULL;
				m_generateStep = None;
			}

			return finish;
		}

		void CWalkingTileMap::updateGenerateTile()
		{
			IMeshBuffer* mb = m_navMesh->getMeshBuffer(0);
			IVertexBuffer* vb = mb->getVertexBuffer();
			IIndexBuffer* ib = mb->getIndexBuffer();

			int t = 10;
			int n = ib->getIndexCount();

			for (; m_generateId < n; m_generateId += 3)
			{
				u32 i = (u32)m_generateId;
				u32 ixA = ib->getIndex(i);
				u32 ixB = ib->getIndex(i + 1);
				u32 ixC = ib->getIndex(i + 2);

				S3DVertex* a = (S3DVertex*)vb->getVertex(ixA);
				S3DVertex* b = (S3DVertex*)vb->getVertex(ixB);
				S3DVertex* c = (S3DVertex*)vb->getVertex(ixC);

				core::aabbox3df box;
				box.reset(a->Pos);
				box.addInternalPoint(b->Pos);
				box.addInternalPoint(c->Pos);

				core::triangle3df tri(a->Pos, b->Pos, c->Pos);

				for (u32 j = 0, m = m_tiles.size(); j < m; j++)
				{
					if (m_tiles[j]->BBox.intersectsWithBox(box))
					{
						m_tiles[j]->Tris.push_back(tri);
					}
				}

				if (--t <= 0)
				{
					// skip for next update
					m_generateId += 3;
					break;
				}
			}

			if (n > 0)
				m_generatePercent = m_generateId / (float)n;

			if (m_generateId >= n)
			{
				m_generateStep = RemoveEmptyTile;
				m_generatePercent = 0.0f;
				m_generateId = m_tiles.size() - 1;
				m_generateMax = m_generateId;
			}
		}

		void CWalkingTileMap::updateGenerateRemoveEmptyTile()
		{
			core::line3df centerLine;
			int t = 200;

			for (; m_generateId >= 0; m_generateId--)
			{
				u32 i = (u32)m_generateId;
				if (m_tiles[i]->Tris.size() == 0)
				{
					delete m_tiles[i];
					m_tiles.erase(i);
				}
				else
				{
					const core::aabbox3df& bbox = m_tiles[i]->BBox;
					{
						// try get position is center
						core::vector3df center = bbox.getCenter();
						core::vector3df centerTop = center;
						core::vector3df centerBottom = center;
						centerTop.Y = bbox.MaxEdge.Y;
						centerBottom.Y = bbox.MinEdge.Y;
						centerLine.setLine(centerTop, centerBottom);

						bool hit = false;

						if (hitTris(centerLine, m_tiles[i]->Tris, m_tiles[i]->Position))
						{
							hit = true;
						}

						if (!hit)
						{
							delete m_tiles[i];
							m_tiles.erase(i);
						}
					}
				}

				if (--t <= 0)
				{
					// skip for next update
					m_generateId--;
					break;
				}
			}

			if (m_generateMax > 0)
				m_generatePercent = (m_generateMax - m_generateId) / (float)m_generateMax;

			if (m_generateId < 0)
			{
				m_generateStep = LinkNeighbours;
				m_generatePercent = 0.0f;
				m_generateId = 0;

				for (u32 i = 0, n = m_tiles.size(); i < n; i++)
				{
					STile* t = m_tiles[i];

					STileXYZ tile(t->X, t->Y, t->Z);
					m_hashTiles[tile] = t;
				}
			}
		}

		void CWalkingTileMap::updateGenerateNeighbours()
		{
			int step = 10;
			float t = 0.0f;
			int n = (int)m_tiles.size() - 1;

			for (; m_generateId < n; m_generateId++)
			{
				u32 i = (u32)m_generateId;
				for (u32 j = i + 1, m = m_tiles.size(); j < m; j++)
				{
					STile* a = m_tiles[i];
					STile* b = m_tiles[j];

					if (abs(a->Y - b->Y) > 1)
						continue;

					// if (a->Position.getDistanceFromSQ(b->Position) > 400.0f)
					//	continue;

					if (!m_obstacle->isLineHit(a->Position, b->Position, m_tileHeight, t))
					{
						a->Neighbours.push_back(b);
						b->Neighbours.push_back(a);
					}
				}

				if (--step <= 0)
				{
					m_generateId++;
					break;
				}
			}

			if (n > 0)
				m_generatePercent = m_generateId / (float)n;

			if (m_generateId >= n)
			{
				// remove tile have no Neighbours
				for (int i = (int)m_tiles.size() - 1; i >= 0; i--)
				{
					if (m_tiles[i]->Neighbours.size() == 0)
					{
						// delete hash
						STileXYZ tile(m_tiles[i]->X, m_tiles[i]->Y, m_tiles[i]->Z);
						auto it = m_hashTiles.find(tile);
						if (it == m_hashTiles.end())
							m_hashTiles.erase(it);

						// free data
						delete m_tiles[i];
						m_tiles.erase(i);
					}
				}

				// sort id
				for (u32 i = 0, n = m_tiles.size(); i < n; i++)
				{
					STile* t = m_tiles[i];
					t->Id = i;
				}

				m_generateStep = Finish;
				m_generatePercent = 0.0f;
			}
		}

		float CWalkingTileMap::getGeneratePercent()
		{
			if (m_generateStep == CollectTile)
			{
				// 25%
				return m_generatePercent * 0.25f;
			}
			else if (m_generateStep == RemoveEmptyTile)
			{
				// 50%
				return 0.25f + m_generatePercent * 0.25f;
			}
			else if (m_generateStep == LinkNeighbours)
			{
				// 100%
				return 0.5f + m_generatePercent * 0.5f;
			}
			return 1.0f;
		}


		STile* CWalkingTileMap::getTile(int x, int y, int z)
		{
			STileXYZ tile(x, y, z);
			auto it = m_hashTiles.find(tile);
			if (it == m_hashTiles.end())
				return NULL;
			return it->second;
		}

		STile* CWalkingTileMap::getTileByPosition(const core::vector3df& pos)
		{
			if (!m_bbox.isPointInside(pos))
				return NULL;

			float dx = pos.X - m_bbox.MinEdge.X;
			float dy = pos.Y - m_bbox.MinEdge.Y;
			float dz = pos.Z - m_bbox.MinEdge.Z;

			int x = (int)floorf(dx / m_tileWidth);
			int y = (int)floorf(dy / m_tileHeight);
			int z = (int)floorf(dz / m_tileWidth);

			return getTile(x, y, z);
		}

		bool CWalkingTileMap::hitTris(const core::line3df& line, core::array<core::triangle3df>& tris, core::vector3df& outPoint)
		{
			bool hit = false;
			for (int i = 0, n = tris.size(); i < n; i++)
			{
				core::triangle3df& t = tris[i];
				if (t.getIntersectionWithLimitedLine(line, outPoint))
				{
					hit = true;
					break;;
				}
			}
			return hit;
		}

		void CWalkingTileMap::release()
		{
			for (u32 i = 0, n = m_tiles.size(); i < n; i++)
			{
				delete m_tiles[i];
			}
			m_tiles.clear();
			m_hashTiles.clear();
		}

		void CWalkingTileMap::resetVisit()
		{
			for (u32 i = 0, n = m_tiles.size(); i < n; i++)
			{
				m_tiles[i]->Visit = false;
			}
		}
	}
}
