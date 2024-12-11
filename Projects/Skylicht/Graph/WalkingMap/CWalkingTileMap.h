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

#pragma once

#include "Entity/CEntityPrefab.h"
#include "RenderMesh/CRenderMeshData.h"
#include "ObstacleAvoidance/CObstacleAvoidance.h"

namespace Skylicht
{
	namespace Graph
	{
		struct STile
		{
			int Id;
			int X;
			int Y;
			int Z;
			core::vector3df Position;
			core::aabbox3df BBox;
			core::array<core::triangle3df> Tris;
			core::array<STile*> Neighbours;
			bool Visit;

			STile()
			{
				Id = 0;
				X = 0;
				Y = 0;
				Z = 0;
				Visit = false;
			}
		};

		struct STileXYZ
		{
			int X;
			int Y;
			int Z;

			STileXYZ(int x, int y, int z)
			{
				X = x;
				Y = y;
				Z = z;
			}
		};

		struct CompareTile
		{
			bool operator()(const STileXYZ& a, const STileXYZ& b) const
			{
				if (a.Z < b.Z)
					return true;
				else if (a.Z > b.Z)
					return false;

				if (a.Y < b.Y)
					return true;
				else if (a.Y > b.Y)
					return false;

				if (a.X < b.X)
					return true;

				return false;
			}
		};

		typedef std::map<STileXYZ, STile*, CompareTile> TileValueMap;

		class CWalkingTileMap
		{
		protected:
			enum EGenerateStep
			{
				None = 0,
				CollectTile,
				RemoveEmptyTile,
				LinkNeighbours,
				Finish
			};

		protected:
			core::array<STile*> m_tiles;

			TileValueMap m_hashTiles;

			float m_tileWidth;
			float m_tileHeight;

			core::aabbox3df m_bbox;

			CMesh* m_navMesh;
			CObstacleAvoidance* m_obstacle;

			EGenerateStep m_generateStep;
			float m_generatePercent;
			int m_generateId;
			int m_generateMax;

		public:
			CWalkingTileMap();

			virtual ~CWalkingTileMap();

			void generate(float tileWidth, float tileHeight, CMesh* navMesh, CObstacleAvoidance* obstacle);

			void beginGenerate(float tileWidth, float tileHeight, CMesh* navMesh, CObstacleAvoidance* obstacle);

			bool updateGenerate();

			float getGeneratePercent();

			void release();

			void resetVisit();

			STile* getTile(int x, int y, int z);

			STile* getTileByPosition(const core::vector3df& pos);

			inline float getTileWidth()
			{
				return m_tileWidth;
			}

			inline float getTileHeight()
			{
				return m_tileHeight;
			}

			inline core::array<STile*>& getTiles()
			{
				return m_tiles;
			}

			inline u32 getNumTile()
			{
				return m_tiles.size();
			}

		protected:

			void generate(float tileWidth, float tileHeight, const core::aabbox3df& bbox);

			void updateGenerateTile();

			void updateGenerateRemoveEmptyTile();

			void updateGenerateNeighbours();

			bool hitTris(const core::line3df& line, core::array<core::triangle3df>& tris, core::vector3df& outPoint);
		};
	}
}
