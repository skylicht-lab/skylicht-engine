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
#include "CGraphComponent.h"

#include "MeshManager/CMeshManager.h"

namespace Skylicht
{
	namespace Graph
	{
		ACTIVATOR_REGISTER(CGraphComponent);

		CATEGORY_COMPONENT(CGraphComponent, "Graph", "Graph");

		CGraphComponent::CGraphComponent() :
			m_cellSize(0.3f),
			m_cellHeight(0.2f),
			m_agentHeight(2.0f),
			m_agentRadius(0.6f),
			m_agentMaxClimb(0.9f),
			m_agentMaxSlope(45.0f),
			m_walkTileWidth(2.0f),
			m_walkTileHeight(2.0f)
		{
			m_query = new CGraphQuery();
			m_builder = new CRecastBuilder();
			m_recastMesh = new CRecastMesh();
			m_obstacle = new CObstacleAvoidance();
			m_walkingTileMap = new CWalkingTileMap();
		}

		CGraphComponent::~CGraphComponent()
		{
			delete m_query;
			delete m_builder;
			delete m_recastMesh;
			delete m_obstacle;
			delete m_walkingTileMap;
		}

		void CGraphComponent::initComponent()
		{

		}

		void CGraphComponent::startComponent()
		{

		}

		void CGraphComponent::updateComponent()
		{

		}

		CObjectSerializable* CGraphComponent::createSerializable()
		{
			CObjectSerializable* obj = CComponentSystem::createSerializable();

			CValueProperty* value = NULL;

			value = new CFloatProperty(obj, "cellSize", m_cellSize, 0.2f, 1.0f);
			value->setUIHeader("Recast mesh params");
			obj->autoRelease(value);
			obj->autoRelease(new CFloatProperty(obj, "cellHeight", m_cellHeight, 0.2f, 1.0f));
			obj->autoRelease(new CFloatProperty(obj, "agentHeight", m_agentHeight, 0.5f, 10.0f));
			obj->autoRelease(new CFloatProperty(obj, "agentRadius", m_agentRadius, 0.1f, 10.0f));
			obj->autoRelease(new CFloatProperty(obj, "agentMaxClimb", m_agentMaxClimb, 0.1f, 10.0f));
			obj->autoRelease(new CFloatProperty(obj, "agentMaxSlope", m_agentMaxSlope, 0.1f, 80.0f));

			value = new CFloatProperty(obj, "walkTileWidth", m_walkTileWidth, 1.0f, 10.0f);
			value->setUIHeader("Walk map params");
			obj->autoRelease(value);
			obj->autoRelease(new CFloatProperty(obj, "walkTileHeight", m_walkTileHeight, 1.0f, 10.0f));

			value = new CFilePathProperty(obj, "collisionMesh", m_inputCollision.c_str(), CMeshManager::getMeshExts());
			value->setUIHeader("Input model");
			obj->autoRelease(value);

			value = new CFilePathProperty(obj, "recastMesh", m_inputRecastMesh.c_str(), CMeshManager::getMeshExts());
			value->setUIHeader("Built model");
			obj->autoRelease(value);

			std::vector<std::string> obstacleExts = {
				"obs"
			};
			obj->autoRelease(new CFilePathProperty(obj, "inputObstacle", m_inputObstacle.c_str(), obstacleExts));

			return obj;
		}

		void CGraphComponent::loadSerializable(CObjectSerializable* obj)
		{
			CComponentSystem::loadSerializable(obj);

			m_cellSize = obj->get<float>("cellSize", 0.2f);
			m_cellHeight = obj->get<float>("cellHeight", 0.3f);
			m_agentHeight = obj->get<float>("agentHeight", 2.0f);
			m_agentRadius = obj->get<float>("agentRadius", 0.6f);
			m_agentMaxClimb = obj->get<float>("agentMaxClimb", 0.9f);
			m_agentMaxSlope = obj->get<float>("agentMaxSlope", 45.0f);

			m_walkTileWidth = obj->get<float>("walkTileWidth", 2.0f);
			m_walkTileHeight = obj->get<float>("walkTileHeight", 2.0f);

			m_inputCollision = obj->get<std::string>("collisionMesh", std::string());

			m_inputRecastMesh = obj->get<std::string>("recastMesh", std::string());
			m_inputObstacle = obj->get<std::string>("inputObstacle", std::string());
		}
	}
}