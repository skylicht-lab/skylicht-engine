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
#include "CWalkingMap.h"

namespace Skylicht
{
	namespace Graph
	{
		CWalkingMap::CWalkingMap() :
			m_tileWidth(0.0f),
			m_tileHeight(0.0f)
		{

		}

		CWalkingMap::~CWalkingMap()
		{
			release();
		}

		void CWalkingMap::generate(float tileWidth, float tileHeight, const core::aabbox3df& bbox)
		{
			release();

			m_tileWidth = tileWidth;
			m_tileHeight = tileHeight;

			core::vector3df size = bbox.getExtent();

			int nY = (int)ceilf(size.Y / tileHeight);
			int nX = (int)ceilf(size.X / tileWidth);
			int nZ = (int)ceilf(size.Z / tileWidth);

			core::vector3df boxSize(tileWidth, tileHeight, tileWidth);

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

		void CWalkingMap::generate(float tileWidth, float tileHeight, CMesh* recastMesh, CObstacleAvoidance* obstacle)
		{
			generate(tileWidth, tileHeight, recastMesh->getBoundingBox());

			// check used tile
			IMeshBuffer* mb = recastMesh->getMeshBuffer(0);
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

			// core::vector3df edges[8];
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

					/*
					bbox.getEdges(edges);

					lines[0].setLine(edges[1], edges[0]);
					lines[1].setLine(edges[3], edges[2]);
					lines[2].setLine(edges[5], edges[4]);
					lines[3].setLine(edges[7], edges[6]);
					*/

					core::vector3df center = bbox.getCenter();
					core::vector3df centerTop = center;
					core::vector3df centerBottom = center;
					centerTop.Y = bbox.MaxEdge.Y;
					centerBottom.Y = bbox.MinEdge.Y;
					centerLine.setLine(centerTop, centerBottom);

					// lines[4].setLine(centerTop, centerBottom);

					bool allHit = true;

					/*
					for (int j = 0; j < 5; j++)
					{
						if (!hitTris(lines[j], m_tiles[i]->Tris, m_tiles[i]->Position))
						{
							allHit = false;
							break;
						}
					}
					*/

					allHit = hitTris(centerLine, m_tiles[i]->Tris, m_tiles[i]->Position);

					if (!allHit)
					{
						delete m_tiles[i];
						m_tiles.erase(i);
					}
				}
			}

			for (u32 i = 0, n = m_tiles.size(); i < n; i++)
			{
				STile* t = m_tiles[i];
				t->Id = i;

				STileXYZ tile(t->X, t->Y, t->Z);
				m_hashTiles[tile] = t;

				obstacle->copySegments(&t->Obstacle, t->BBox);
			}

			// link Neighbour
			for (u32 i = 0, n = m_tiles.size(); i < n; i++)
			{
				STile* t = m_tiles[i];

				for (int y = -1; y <= 1; y++)
				{
					for (int x = -1; x <= 1; x++)
					{
						for (int z = -1; z <= 1; z++)
						{
							if (x == 0 && y == 0 && z == 0)
								continue;

							STile* nei = getTile(
								t->X + x,
								t->Y + y,
								t->Z + z);
							if (nei)
							{
								if (!t->Obstacle.isLineHit(t->Position, nei->Position, tileHeight) &&
									!nei->Obstacle.isLineHit(t->Position, nei->Position, tileHeight))
								{
									t->Neighbours.push_back(nei);
								}
							}
						}
					}
				}
			}
		}

		STile* CWalkingMap::getTile(int x, int y, int z)
		{
			STileXYZ tile(x, y, z);
			auto it = m_hashTiles.find(tile);
			if (it == m_hashTiles.end())
				return NULL;
			return it->second;
		}

		bool CWalkingMap::hitTris(const core::line3df& line, core::array<core::triangle3df>& tris, core::vector3df& outPoint)
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

		void CWalkingMap::release()
		{
			for (u32 i = 0, n = m_tiles.size(); i < n; i++)
			{
				delete m_tiles[i];
			}
			m_tiles.clear();
			m_hashTiles.clear();
		}

		void CWalkingMap::resetVisit()
		{
			for (u32 i = 0, n = m_tiles.size(); i < n; i++)
			{
				m_tiles[i]->Visit = false;
			}
		}
	}
}
