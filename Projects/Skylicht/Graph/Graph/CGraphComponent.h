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

#include "Components/CComponentSystem.h"

#include "Graph/CGraphQuery.h"
#include "RecastMesh/CRecastBuilder.h"
#include "RecastMesh/CRecastMesh.h"
#include "WalkingMap/CWalkingTileMap.h"

namespace Skylicht
{
	namespace Graph
	{
		class CGraphComponent : public CComponentSystem
		{
		protected:
			CGraphQuery* m_query;
			CRecastBuilder* m_builder;
			CRecastMesh* m_recastMesh;
			CObstacleAvoidance* m_obstacle;
			CWalkingTileMap* m_walkingTileMap;

			float m_cellSize;
			float m_cellHeight;
			float m_agentHeight;
			float m_agentRadius;
			float m_agentMaxClimb;
			float m_agentMaxSlope;
			float m_walkTileWidth;
			float m_walkTileHeight;

			std::string m_inputCollision;
			std::string m_inputRecastMesh;
			std::string m_inputObstacle;

			CMesh* m_navMesh;
		public:
			CGraphComponent();

			virtual ~CGraphComponent();

			virtual void initComponent();

			virtual void startComponent();

			virtual void updateComponent();

			virtual CObjectSerializable* createSerializable();

			virtual void loadSerializable(CObjectSerializable* object);

			inline void setInputMeshCollision(const char* source)
			{
				m_inputCollision = source;
			}

			inline const char* getInputMeshCollision()
			{
				return m_inputCollision.c_str();
			}

			inline void setInputRecastMesh(const char* source)
			{
				m_inputRecastMesh = source;
			}

			inline const char* getInputRecastMesh()
			{
				return m_inputRecastMesh.c_str();
			}

			inline void setInputObstacle(const char* source)
			{
				m_inputObstacle = source;
			}

			inline const char* getInputObstacle()
			{
				return m_inputObstacle.c_str();
			}

			inline CMesh* getNavMesh()
			{
				return m_navMesh;
			}

			bool buildRecastMesh();

			DECLARE_GETTYPENAME(CGraphComponent)
		};
	}
}