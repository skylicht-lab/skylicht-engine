/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "CGGraph.h"

#include "Utils/CActivator.h"
#include "Components/CDependentComponent.h"

#include "GameObject/CGameObject.h"
#include "Selection/CSelection.h"

#include "Editor/CEditor.h"
#include "Handles/CHandles.h"

#ifdef BUILD_SKYLICHT_GRAPH

using namespace Graph;

namespace Skylicht
{
	namespace Editor
	{
		ACTIVATOR_REGISTER(CGGraph);

		DEPENDENT_COMPONENT(CGraphComponent, CGGraph);

		CGGraph::CGGraph() :
			m_graph(NULL)
		{
		}

		CGGraph::~CGGraph()
		{
		}

		void CGGraph::initComponent()
		{
			m_graph = m_gameObject->getComponent<CGraphComponent>();
		}

		inline int bit(int a, int b)
		{
			return (a & (1 << b)) >> b;
		}

		void intToCol(int i, int a, SColor& c)
		{
			int	r = bit(i, 1) + bit(i, 3) * 2 + 1;
			int	g = bit(i, 2) + bit(i, 4) * 2 + 1;
			int	b = bit(i, 0) + bit(i, 5) * 2 + 1;
			c.set(a, r * 120, g * 120, b * 120);
		}

		void CGGraph::updateComponent()
		{
			CSelectObject* selectObject = CSelection::getInstance()->getLastSelected();
			if (selectObject && selectObject->getID() == m_gameObject->getID())
			{
				CMesh* mesh = m_graph->getNavMesh();

				SColor white(255, 255, 255, 255);
				SColor red(255, 255, 0, 0);

				CHandles* handles = CHandles::getInstance();

				for (u32 i = 0, n = mesh->getMeshBufferCount(); i < n; i++)
				{
					IMeshBuffer* mb = mesh->getMeshBuffer(i);
					IVertexBuffer* vb = mb->getVertexBuffer();
					IIndexBuffer* ib = mb->getIndexBuffer();

					for (u32 i = 0, n = ib->getIndexCount(); i < n; i += 3)
					{
						u32 a = ib->getIndex(i);
						u32 b = ib->getIndex(i + 1);
						u32 c = ib->getIndex(i + 2);

						S3DVertex* p1 = (S3DVertex*)vb->getVertex(a);
						S3DVertex* p2 = (S3DVertex*)vb->getVertex(b);
						S3DVertex* p3 = (S3DVertex*)vb->getVertex(c);

						handles->drawLine(p1->Pos, p2->Pos, white);
						handles->drawLine(p2->Pos, p3->Pos, white);
						handles->drawLine(p3->Pos, p1->Pos, white);
					}
				}

				Graph::CObstacleAvoidance* obstacle = m_graph->getObstacle();
				core::array<core::line3df>& segments = obstacle->getSegments();
				for (u32 i = 0, n = segments.size(); i < n; i++)
				{
					handles->drawLine(segments[i].start, segments[i].end, red);
				}

				core::vector3df halfHeight(0.0f, 0.2f, 0.0f);
				SColor color;
				Graph::CWalkingTileMap* walkMap = m_graph->getWalkingMap();

				walkMap->resetVisit();
				core::array<Graph::STile*>& tiles = walkMap->getTiles();
				for (u32 i = 0, n = tiles.size(); i < n; i++)
				{
					Graph::STile* tile = tiles[i];
					intToCol(tile->AreaId, 255, color);
					handles->drawLine(tile->Position, tile->Position + halfHeight, color);
					tile->Visit = true;
				}
				walkMap->resetVisit();
			}
		}
	}
}

#endif