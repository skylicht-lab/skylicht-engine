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
#include "CTileMap.h"

namespace Skylicht
{
	namespace Graph
	{
		CTileMap::CTileMap() :
			m_tileWidth(0.0f),
			m_tileHeight(0.0f)
		{

		}

		CTileMap::~CTileMap()
		{
			release();
		}

		void CTileMap::generate(float tileWidth, float tileHeight, const core::aabbox3df& bbox)
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
						tile->Position = tile->BBox.getCenter();
						m_tiles.push_back(tile);
					}
				}
			}
		}

		void CTileMap::generate(float tileWidth, float tileHeight, CMesh* recastMesh)
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
				
				S3DVertex *a = (S3DVertex*)vb->getVertex(ixA);
				S3DVertex *b = (S3DVertex*)vb->getVertex(ixB);
				S3DVertex *c = (S3DVertex*)vb->getVertex(ixC);
				
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
			
			for (int i = (int)m_tiles.size() - 1; i >= 0; i--)
			{
				if (m_tiles[i]->Tris.size() == 0)
				{
					delete m_tiles[i];
					m_tiles.erase(i);
				}
			}
		}
	
		void CTileMap::release()
		{
			for (u32 i = 0, n = m_tiles.size(); i < n; i++)
			{
				delete m_tiles[i];
			}
			m_tiles.clear();
		}
	}
}
